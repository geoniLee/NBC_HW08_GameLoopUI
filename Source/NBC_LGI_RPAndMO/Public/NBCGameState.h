// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "NBCGameState.generated.h"

/**
 * 
 */
UCLASS()
class NBC_LGI_RPANDMO_API ANBCGameState : public AGameState
{
	GENERATED_BODY()
	
public:
#pragma region 변수
	FTimerHandle WaveTimerHandle;			// 레벨 타이머
	FTimerHandle HUDUpdateTimerHandle;		// HUD 갱신 타이머
	TArray<AActor*> FoundVolumes;			// SpawmVolume을 저장할 배열

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score;							// 점수

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 SpawnedCoinCount;					// 생성된 코인

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 CollectedCoinCount;				// 전체 생성된 코인
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level");
	float LevelDuration;					// 레벨 제한 시간

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevelIndex;				// 현재 레벨 idx

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 MaxLevel;							// 최대 레벨

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	TArray<FName> LevelMapNames;			// 레벨 별 Map 이름

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level|Wave")
	int32 currentWave;						// 현재 Wave

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level|Wave")
	int32 MaxWave;							// 최대 Wave

#pragma endregion

#pragma region 함수
	// 생성자
	ANBCGameState();
	
	// 시작시 실행
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;
	 
	UFUNCTION(BlueprintCallable, Category = "Score")
	// instance에 현재 점수 반영
	void AddScore(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Level")
	// 게임 오버 처리
	void OnGameOver();

	// 레벨 시작
	void StartLevel();
	// 코인 획득
	void OnCoinCollected();
	// 웨이브 제한 시간 초과
	void OnWaveTimeUp();
	// 레벨 종료
	void EndLevel();
	// HUD 갱신
	void UpdateHUD();

private:
	// 웨이브 시작
	void StartWave();
	// 웨이브 종료
	void EndWave();

#pragma endregion

};
