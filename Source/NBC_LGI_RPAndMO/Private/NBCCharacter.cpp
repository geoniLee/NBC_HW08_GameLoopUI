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

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300;
	// 컨트롤러를 회전할 때 스프링 암도 같이 움직임 여부
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	//USpringArmComponent::SockerName -> SpringArm의 끝지점
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidger"));
	OverheadWidget->SetupAttachment(GetMesh());
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);

	NormalSpeed = 600;
	SprintSpeedMultiplier = 1.5f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	MaxHealth = 100;
	Health = MaxHealth;
}


float ANBCCharacter::GetHealth() const
{
	return Health;
}

void ANBCCharacter::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0, MaxHealth);
	UpdateOverheadHP();
}

void ANBCCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateOverheadHP();
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
	UpdateOverheadHP();

	if (Health <= 0) {
		OnDeath();
	}

	return 0.0f;
}

void ANBCCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller) return;

	const FVector2D moveInput = Value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(moveInput.X)) {
		// AddMovementInput(방향, 이동값)
		// GetActorForwardVector(): 캐릭터가 바라보는 방향/ 정면
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
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	
}

void ANBCCharacter::StopSprint(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
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
