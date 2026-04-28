// Fill out your copyright notice in the Description page of Project Settings.


#include "NBCGameInstance.h"

UNBCGameInstance::UNBCGameInstance()
{
	TotalScore = 0;
	CurrentLevelIndex = 0;
}

void UNBCGameInstance::AddToScore(int32 Amount)
{
	TotalScore += Amount;
	UE_LOG(LogTemp, Warning, TEXT("Total Score Updated: %d"), TotalScore);
}
