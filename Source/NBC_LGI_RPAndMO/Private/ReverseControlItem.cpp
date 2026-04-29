// Fill out your copyright notice in the Description page of Project Settings.


#include "ReverseControlItem.h"

AReverseControlItem::AReverseControlItem()
{
	ItemType = "Debuff";
}

void AReverseControlItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
}
