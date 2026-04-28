// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "NBCGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class NBC_LGI_RPANDMO_API UNBCGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameData")
	int32 TotalScore;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameData")
	int32 CurrentLevelIndex;

	UNBCGameInstance();

	UFUNCTION(BlueprintCallable, Category = "GameData")
	void AddToScore(int32 Amount);
	
};
