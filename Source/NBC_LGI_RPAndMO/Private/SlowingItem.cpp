// Fill out your copyright notice in the Description page of Project Settings.


#include "SlowingItem.h"
#include "NBCCharacter.h"

ASlowingItem::ASlowingItem()
{
	SlowTime = 1;
	ItemType = "Debuff";
}

void ASlowingItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player")) {
		if (ANBCCharacter* PlayerCharacter = Cast<ANBCCharacter>(Activator)) {
			PlayerCharacter->AddSlowTime(SlowTime);
		}
		DestroyItem();
	}
}
