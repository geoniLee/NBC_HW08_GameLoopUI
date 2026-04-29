// Fill out your copyright notice in the Description page of Project Settings.


#include "NBCGameState.h"
#include "Kismet/GameplayStatics.h"
#include "SpawmVolume.h"
#include "CoinItem.h"
#include "NBCGameInstance.h"
#include "NBCCharacter.h"
#include "NBCPlayerController.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

ANBCGameState::ANBCGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	LevelDuration = 5;
	WaveDuration = 3;
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
				// Timer 갱신
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Txt_Time")))) {
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
					
					// 색상 변경 
					FSlateColor TimerColor;
					if (RemainingTime < 5) {
						TimerColor = FSlateColor(FLinearColor::Red);
					}
					else {
						TimerColor = FSlateColor(FLinearColor::White);
					}
					TimeText->SetColorAndOpacity(TimerColor);
				}

				// Score Text 갱신
				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Txt_Score")))) {
					if(UGameInstance* GameInstance = GetGameInstance()){
						UNBCGameInstance* NBCGameInstance = Cast<UNBCGameInstance>(GameInstance);
						if (NBCGameInstance) {
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), NBCGameInstance->TotalScore)));
						}	
					}
				}

				// Level Text 갱신
				if (UTextBlock* LevelText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Txt_Level")))) {
					LevelText->SetText(FText::FromString(FString::Printf(TEXT("Level %d"), CurrentLevelIndex+1)));
				}

				// Wave Text 갱신
				if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Txt_Wave")))) {
					WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave %d"), currentWave)));
				}

				// HP Text 갱신
				if (UTextBlock* HPText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Txt_HP")))) {
					if (ANBCCharacter* NBCChracter = Cast<ANBCCharacter>(Playercontroller->GetPawn())) {
						HPText->SetText(FText::FromString(FString::Printf(
							TEXT("%.f / %.f"), NBCChracter->GetHealth(), NBCChracter->GetMaxHealth())));
					}
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
	SpawnItems(ItemToSpawn, false);

	// Wave 종료 시점 설정
	GetWorldTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&ANBCGameState::OnWaveTimeUp,
		LevelDuration * currentWave,
		false
	);

	// Level/Wave 애니메이션 실행
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController()) {
		if (ANBCPlayerController* NBCPlayerController = Cast<ANBCPlayerController>(PlayerController)) {
			if (UUserWidget* HUDWidgetInstance = NBCPlayerController->HUDWidgetInstance) {
				if(UFunction* PlayAnimFunc = HUDWidgetInstance->FindFunction(FName("StartWaveAnim"))){
					HUDWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
				}
			}
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Emerald, FString::Printf(TEXT("Stage %d, Wave %d 시작!!"), CurrentLevelIndex + 1, currentWave));
	
	if (currentWave == 2) {
		GetWorldTimerManager().SetTimer(
			WaveEvnetTimerHandle,
			FTimerDelegate::CreateLambda([this]() {
				SpawnItems(5, true);
			}),
			WaveDuration,
			false
		);
	}
	else if (currentWave == 3) {
		GetWorldTimerManager().SetTimer(
			WaveEvnetTimerHandle,
			this,
			&ANBCGameState::SpawnAndUseItem,
			WaveDuration,
			false
		);
	}
}

void ANBCGameState::SpawnItems(int SpawnCnt, bool bIsWaveEvent)
{
	for (int32 i = 0; i < SpawnCnt; ++i) {
		ASpawmVolume* SpawnVolume = Cast<ASpawmVolume>(FoundVolumes[0]);
		if (SpawnVolume) {
			AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
			if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass())) {
				if (bIsWaveEvent) continue;
				SpawnedCoinCount++;
			}
		}
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Emerald, FString::Printf(TEXT("랜덤 아이템 5개 추가 생성")));

}

void ANBCGameState::SpawnAndUseItem()
{
	ASpawmVolume* SpawnVolume = Cast<ASpawmVolume>(FoundVolumes[0]);
	if (!SpawnVolume) return;
	
	AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
	if (!SpawnedActor) return;

	if (ANBCCharacter* Player = Cast<ANBCCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0))) {
		if (ABaseItem* Item = Cast<ABaseItem>(SpawnedActor)) {
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Emerald,
				FString::Printf(TEXT("랜덤 아이템 사용 (%s)"), *Item->GetItemType().ToString()));
			Item->ActivateItem(Player);
		}
	}
}

// 웨이브 종료 및 다음 웨이브/레벨로 연결
void ANBCGameState::EndWave()
{
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	GetWorldTimerManager().ClearTimer(WaveEvnetTimerHandle);

	currentWave++;

	if (currentWave > MaxWave) {
		EndLevel();
		return;
	}
	ClearItems();
	StartWave();
}

void ANBCGameState::ClearItems()
{
	TArray<AActor*> FoundItems;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseItem::StaticClass(), FoundItems);

	for (AActor* Item : FoundItems) {
		if (Item) {
			Item->Destroy();
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
