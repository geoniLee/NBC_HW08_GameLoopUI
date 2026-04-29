// Fill out your copyright notice in the Description page of Project Settings.


#include "NBCCharacter.h"
#include "NBCPlayerController.h"
#include "NBCGameState.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Actor.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"


// Sets default values
ANBCCharacter::ANBCCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 스프링 암
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300;
	SpringArmComp->bUsePawnControlRotation = true;

	// 카메라 컴포넌트
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	/*
	오버헤드 위젯 HP - 미사용
	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidger"));
	OverheadWidget->SetupAttachment(GetMesh());
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);
	*/

	NormalSpeed = 600;
	SprintSpeedMultiplier = 1;
	SlowSpeedMultiplier = 1;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	MaxHealth = 100;
	Health = MaxHealth;
}

float ANBCCharacter::GetHealth() const
{
	return Health;
}

float ANBCCharacter::GetMaxHealth() const
{
	return MaxHealth;
}

void ANBCCharacter::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0, MaxHealth);
	UpdateOverheadHP();
}

void ANBCCharacter::AddSlowTime(float time)
{
	if (!GetWorld())return;

	float remainTime;

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	remainTime = TimerManager.IsTimerActive(SlowTimerHandle)
		? TimerManager.GetTimerRemaining(SlowTimerHandle) : 0;

	FString DebufMessage;
	if (FMath::IsNearlyZero(remainTime)) {
		DebufMessage = TEXT("플레이어가 느려졌습니다");
	}
	else {
		DebufMessage = TEXT("Slow 시간이 연장되었습니다.");
	}
	remainTime += time;

	SlowSpeedMultiplier = 0.5f;

	UpdateMoveSpeed();
	
	if (ANBCPlayerController* NBCPlayerController = Cast<ANBCPlayerController>(GetController())) {
		NBCPlayerController->ShowDebuffUI(TEXT("Img_Slow"), DebufMessage);
	}

	GetWorld()->GetTimerManager().ClearTimer(SlowTimerHandle);

	GetWorld()->GetTimerManager().SetTimer(
		SlowTimerHandle,
		this,
		&ANBCCharacter::EndSlow,
		remainTime,
		false
	);
}

void ANBCCharacter::AddReverseControllerTime(float time)
{
	if (!GetWorld())return;

	float remainTime;

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	remainTime = TimerManager.IsTimerActive(ReverseTimerHandle)
		? TimerManager.GetTimerRemaining(ReverseTimerHandle) : 0;

	FString DebufMessage;
	if (FMath::IsNearlyZero(remainTime)) {
		DebufMessage = TEXT("플레이어가 반대 방향으로 이동합니다");
	}
	else {
		DebufMessage = TEXT("Reverse 시간이 연장되었습니다.");
	}
	remainTime += time;

	bIsReverseController = true;

	if (ANBCPlayerController* NBCPlayerController = Cast<ANBCPlayerController>(GetController())) {
		NBCPlayerController->ShowDebuffUI(TEXT("Img_Reverse"), DebufMessage);
	}

	GetWorld()->GetTimerManager().ClearTimer(ReverseTimerHandle);

	GetWorld()->GetTimerManager().SetTimer(
		ReverseTimerHandle,
		this,
		&ANBCCharacter::EndReverse,
		remainTime,
		false
	);
}

void ANBCCharacter::BeginPlay()
{
	Super::BeginPlay();

	//UpdateOverheadHP();
}

// Called to bind functionality to input
void ANBCCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		if (ANBCPlayerController* PlayerController = Cast<ANBCPlayerController>(GetController())) {
			if (PlayerController->MoveAction) {
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ANBCCharacter::Move
				);
			}

			if (PlayerController->JumpAction) {
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&ANBCCharacter::StartJump
				);

				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&ANBCCharacter::StopJump
				);
			}
			
			if (PlayerController->LookAction) {
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ANBCCharacter::Look
				);
			}

			if (PlayerController->SprintAction) {
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&ANBCCharacter::StartSprint
				);

				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&ANBCCharacter::StopSprint
				);
			}
		}
	}
}

float ANBCCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage  = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - DamageAmount, 0, MaxHealth);
	//UpdateOverheadHP();

	if (Health <= 0) {
		OnDeath();
	}

	return 0.0f;
}

void ANBCCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller) return;

	FVector2D moveInput = Value.Get<FVector2D>();

	if (bIsReverseController) moveInput *= -1;

	if (!FMath::IsNearlyZero(moveInput.X)) {
		AddMovementInput(GetActorForwardVector(), moveInput.X);
	}

	if (!FMath::IsNearlyZero(moveInput.Y)) {
		AddMovementInput(GetActorRightVector(), moveInput.Y);
	}
}

void ANBCCharacter::StartJump(const FInputActionValue& Value)
{
	if (Value.Get<bool>()) {
		Jump();
	}
}

void ANBCCharacter::StopJump(const FInputActionValue& Value)
{
	if (!Value.Get<bool>()) {
		StopJumping();
	}
}

void ANBCCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookInput = Value.Get<FVector2D>();

	// Yaw = 좌우 회전
	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void ANBCCharacter::StartSprint(const FInputActionValue& Value)
{
	SprintSpeedMultiplier = 1.5f;
	UpdateMoveSpeed();
	
}

void ANBCCharacter::StopSprint(const FInputActionValue& Value)
{
	SprintSpeedMultiplier = 1;
	UpdateMoveSpeed();
}

void ANBCCharacter::OnDeath()
{
	ANBCGameState* NBCGameState = GetWorld() ? GetWorld()->GetGameState<ANBCGameState>() : nullptr;
	if (NBCGameState) {
		NBCGameState->OnGameOver();
	}
}

void ANBCCharacter::UpdateOverheadHP()
{
	if (!OverheadWidget) return;

	UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
	if (!OverheadWidgetInstance) return;

	if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("Txt_OverheadHP")))) {
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));
	}
}

void ANBCCharacter::UpdateMoveSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed * SlowSpeedMultiplier * SprintSpeedMultiplier;
}

void ANBCCharacter::EndSlow()
{
	SlowSpeedMultiplier = 1;
	UpdateMoveSpeed();

	if (ANBCPlayerController* NBCPlayerController = Cast<ANBCPlayerController>(GetController())) {
		NBCPlayerController->HideDebuffUI(TEXT("Img_Slow"), TEXT("Slow 시간이 종료되었습니다."));
	}
}

void ANBCCharacter::EndReverse()
{

	bIsReverseController = false;

	if (ANBCPlayerController* NBCPlayerController = Cast<ANBCPlayerController>(GetController())) {
		NBCPlayerController->HideDebuffUI(TEXT("Img_Reverse"), TEXT("Reverse 시간이 종료되었습니다."));
	}
}
