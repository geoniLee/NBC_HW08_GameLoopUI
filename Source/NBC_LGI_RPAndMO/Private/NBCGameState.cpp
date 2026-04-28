// Fill out your copyright notice in the Description page of Project Settings.


#include "NBCGameState.h"
#include "Kismet/GameplayStatics.h"
#include "SpawmVolume.h"
#include "CoinItem.h"
#include "NBCGameInstance.h"
#include "NBCPlayerController.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

ANBCGameState::ANBCGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	LevelDuration = 30;
	CurrentLevelIndex = 0;
	MaxLevel = 3;
}

void ANBCGameState::BeginPlay()
{
	Super::BeginPlay();

	StartLevel();

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ANBCGameState::UpdateHUD,
		0.1f,
		true
	);
}

int32 ANBCGameState::GetScore() const
{
	return Score;
}

void ANBCGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance()) {
		UNBCGameInstance* NBCGameInstance = Cast<UNBCGameInstance>(GameInstance);
		if (NBCGameInstance) {
			NBCGameInstance->AddToScore(Amount);
		}
	}
}
void ANBCGameState::StartLevel()
{
	if(APlayerController* PlayerController = GetWorld()->GetFirstPlayerController()){
		if (ANBCPlayerController* NBCPlayerController = Cast<ANBCPlayerController>(PlayerController)) {
			NBCPlayerController->ShowGameHUD();
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance()) {
		UNBCGameInstance* NBCGameInstance = Cast<UNBCGameInstance>(GameInstance);
		if (NBCGameInstance) {
			CurrentLevelIndex = NBCGameInstance->CurrentLevelIndex;
		}
	}

	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawmVolume::StaticClass(), FoundVolumes);

	const int32 ItemToSpawn = 40;

	if (FoundVolumes.Num() == 0) return;

	for (int32 i = 0; i < ItemToSpawn; ++i) {
		ASpawmVolume* SpawnVolume = Cast<ASpawmVolume>(FoundVolumes[0]);
		if (SpawnVolume) {
			AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
			// isA: 이 객체가 특정 클래스 타입이거나, 그 클래스를 상속한 타입인지 검사하는 함수
			if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass())) {
				SpawnedCoinCount++;
			}
		}
	}

	GetWorldTimerManager().SetTimer(
		LevelTimerHandle,
		this,
		&ANBCGameState::OnLevelTimeUp,
		LevelDuration,
		false
	);

}

void ANBCGameState::OnCoinCollected()
{
	CollectedCoinCount++;
	UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"), CollectedCoinCount, SpawnedCoinCount);

	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount) {
		EndLevel();
	}
}

void ANBCGameState::OnLevelTimeUp()
{
	EndLevel();
}

void ANBCGameState::EndLevel()
{
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);

	if (UGameInstance* GameInstance = GetGameInstance()) {
		UNBCGameInstance* NBCGameInstance = Cast<UNBCGameInstance>(GameInstance);
		if (NBCGameInstance) {
			AddScore(Score);
			CurrentLevelIndex++;
			NBCGameInstance->CurrentLevelIndex = CurrentLevelIndex;
		}
	}

	if (CurrentLevelIndex >= MaxLevel) {
		OnGameOver();
		return;
	}

	if (LevelMapNames.IsValidIndex(CurrentLevelIndex)) {
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else {
		OnGameOver();
	}
}

void ANBCGameState::UpdateHUD()
{
	if (APlayerController* Playercontroller = GetWorld()->GetFirstPlayerController()) {
		if (ANBCPlayerController* NBCPlayerController = Cast<ANBCPlayerController>(Playercontroller)) {
			if (UUserWidget* HUDWidget = NBCPlayerController->GetHUDWidget()) {
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Txt_Time")))) {
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
				}

				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Txt_Score")))) {
					if(UGameInstance* GameInstance = GetGameInstance()){
						UNBCGameInstance* NBCGameInstance = Cast<UNBCGameInstance>(GameInstance);
						if (NBCGameInstance) {
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), NBCGameInstance->TotalScore)));
						}	
					}
				}

				if (UTextBlock* LevelText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Txt_Level")))) {
					LevelText->SetText(FText::FromString(FString::Printf(TEXT("Level %d"), CurrentLevelIndex+1)));
				}
			}
		}
	}
}

void ANBCGameState::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController()) {
		if (ANBCPlayerController* NBCPlayerController = Cast<ANBCPlayerController>(PlayerController)) {
			NBCPlayerController->SetPause(true);
			NBCPlayerController->ShowMainMenu(true);
		}
	}
}
