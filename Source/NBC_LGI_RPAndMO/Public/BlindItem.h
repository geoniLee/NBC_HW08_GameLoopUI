// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "BlindItem.generated.h"

/**
 * 
 */
UCLASS()
class NBC_LGI_RPANDMO_API ABlindItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
	float BlindTime;

	ABlindItem();
	virtual void ActivateItem(AActor* Activator) override;
};
