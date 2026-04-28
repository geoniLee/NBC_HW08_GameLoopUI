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
	LevelDuration = 5;
	CurrentLevelIndex = 0;
	MaxLevel = 3;
	currentWave = 1;
	MaxWave = 3;
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

// instance에 현재 점수 반영
void ANBCGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance()) {
		UNBCGameInstance* NBCGameInstance = Cast<UNBCGameInstance>(GameInstance);
		if (NBCGameInstance) {
			NBCGameInstance->AddToScore(Amount);
		}
	}
}

// 레벨 시작
void ANBCGameState::StartLevel()
{
	// PlayerController에 HUD 부착
	if(APlayerController* PlayerController = GetWorld()->GetFirstPlayerController()){
		if (ANBCPlayerController* NBCPlayerController = Cast<ANBCPlayerController>(PlayerController)) {
			NBCPlayerController->ShowGameHUD();
		}
	}

	// 인스턴스 에서 현재 레벨 받아와 동기화
	if (UGameInstance* GameInstance = GetGameInstance()) {
		UNBCGameInstance* NBCGameInstance = Cast<UNBCGameInstance>(GameInstance);
		if (NBCGameInstance) {
			CurrentLevelIndex = NBCGameInstance->CurrentLevelIndex;
		}
	}

	// 현재 레벨에서 SpawmVolume 탐색
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawmVolume::StaticClass(), FoundVolumes);

	if (FoundVolumes.Num() == 0) return;
	
	StartWave();
}

void ANBCGameState::OnCoinCollected()
{
	CollectedCoinCount++;
	UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"), CollectedCoinCount, SpawnedCoinCount);

	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount) {
		EndWave();
	}
}

void ANBCGameState::OnWaveTimeUp()
{
	EndWave();
}

void ANBCGameState::EndLevel()
{
	GetWorldTimerManager().ClearTimer(HUDUpdateTimerHandle);

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
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
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

void ANBCGameState::StartWave()
{
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	const int32 ItemToSpawn = 15 * currentWave;

	// ItemToSpawn의 개수만큼 Item 생성
	for (int32 i = 0; i < ItemToSpawn; ++i) {
		ASpawmVolume* SpawnVolume = Cast<ASpawmVolume>(FoundVolumes[0]);
		if (SpawnVolume) {
			AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
			if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass())) {
				SpawnedCoinCount++;
			}
		}
	}

	// Wave 종료 시점 설정
	GetWorldTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&ANBCGameState::OnWaveTimeUp,
		LevelDuration * currentWave,
		false
	);
}

// 웨이브 종료 및 다음 웨이브/레벨로 연결
void ANBCGameState::EndWave()
{
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);

	currentWave++;

	if (currentWave > MaxWave) {
		EndLevel();
		return;
	}

	StartWave();
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
