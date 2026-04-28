// Fill out your copyright notice in the Description page of Project Settings.


#include "NBCPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "NBCGameState.h"
#include "Components/TextBlock.h"
#include "NBCGameInstance.h"
#include "Kismet/GameplayStatics.h"

ANBCPlayerController::ANBCPlayerController(): 
	InputMapptingContext(nullptr),
	MoveAction(nullptr),
	JumpAction(nullptr),
	SprintAction(nullptr),
	LookAction(nullptr),
	HUDWigetClass(nullptr),
	HUDWidgetInstance(nullptr),
	MainMenuWigetClass(nullptr),
	MainMenuWidgetInstance(nullptr)
{

}

UUserWidget* ANBCPlayerController::GetHUDWidget() const
{
	return HUDWidgetInstance;
}

void ANBCPlayerController::ShowGameHUD()
{
	if (HUDWidgetInstance) {
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance) {
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (HUDWigetClass) {
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWigetClass);
		if (HUDWidgetInstance) {
			HUDWidgetInstance->AddToViewport();

			// 마우스 커서 보이도록
			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());
		}

		ANBCGameState* NBCGameState = GetWorld() ? GetWorld()->GetGameState<ANBCGameState>() : nullptr;
		if (NBCGameState) {
			NBCGameState->UpdateHUD();
		}
	}
}

void ANBCPlayerController::ShowMainMenu(bool BIsRestart)
{
	if (HUDWidgetInstance) {
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance) {
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (MainMenuWigetClass) {
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWigetClass);
		if (MainMenuWidgetInstance) {
			MainMenuWidgetInstance->AddToViewport();

			// 마우스 커서 보이도록
			bShowMouseCursor = true;
			// 마우스만 사용
			SetInputMode(FInputModeUIOnly());
		}

		if (UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("Btn_Start")))) {
			if (BIsRestart) {
				ButtonText->SetText(FText::FromString(TEXT("Restart")));
			}
			else {
				ButtonText->SetText(FText::FromString(TEXT("Start")));
			}
		}

		if (BIsRestart) {
			UFunction* PlayAnimFunc = MainMenuWidgetInstance->FindFunction(FName("PlayGameOverAnim"));
			if (PlayAnimFunc) {
				MainMenuWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
			}

			if (UTextBlock* TotalScoreText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName("Txt_TotalScore"))) {
				if (UNBCGameInstance* NBCGameInstance = Cast<UNBCGameInstance>(UGameplayStatics::GetGameInstance(this))) {
					TotalScoreText->SetText(FText::FromString(FString::Printf(TEXT("Total Score: %d"), NBCGameInstance->TotalScore)));
				}
			}
		}
	}
}

void ANBCPlayerController::StartGame()
{
	if (UNBCGameInstance* NBCGameInstance = Cast<UNBCGameInstance>(UGameplayStatics::GetGameInstance(this))) {
		NBCGameInstance->CurrentLevelIndex = 0;
		NBCGameInstance->TotalScore = 0;
	}

	SetPause(false);
	UGameplayStatics::OpenLevel(GetWorld(), FName("BasicLevel"));
}

void ANBCPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// LocalPlayer = Player의 입력이나 화면 뷰 등을 관리하는 객체
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer()) {

		// UEnhancedInputLocalPlayerSubsystem = IMC을 관리하는 서브시스템
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem =

			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()) {

			if (InputMapptingContext) {
				// 주어진 IMC을 SubSystem에 추가하여 입력 매핑을 활성화
				// 0 -> 우선 순위
				SubSystem->AddMappingContext(InputMapptingContext, 0);
			}
		}
	}

	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("Menulevel")) {
		ShowMainMenu(false);
	}
}
