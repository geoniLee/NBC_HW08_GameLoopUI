// Fill out your copyright notice in the Description page of Project Settings.


#include "ReverseControlItem.h"
#include "NBCCharacter.h"

AReverseControlItem::AReverseControlItem()
{
	ReverseTime = 1;
	ItemType = "Debuff";
}

void AReverseControlItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player")) {
		if (ANBCCharacter* PlayerCharacter = Cast<ANBCCharacter>(Activator)) {
			PlayerCharacter->AddReverseControllerTime(ReverseTime);
		}
		DestroyItem();
	}
}
