// Fill out your copyright notice in the Description page of Project Settings.


#include "BlindItem.h"
#include "NBCCharacter.h"

ABlindItem::ABlindItem()
{
	BlindTime = 2;
	ItemType = "Blind";
}

void ABlindItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player")) {
		if (ANBCCharacter* PlayerCharacter = Cast<ANBCCharacter>(Activator)) {
			PlayerCharacter->AddBlindTime(BlindTime);
		}
		DestroyItem();
	}
}
