// Fill out your copyright notice in the Description page of Project Settings.


#include "BlindItem.h"

ABlindItem::ABlindItem()
{
	ItemType = "Debuff";
}

void ABlindItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
}
