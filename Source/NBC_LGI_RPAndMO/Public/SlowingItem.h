// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "SlowingItem.generated.h"

/**
 * 
 */
UCLASS()
class NBC_LGI_RPANDMO_API ASlowingItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float SlowTime;

	ASlowingItem();
	virtual void ActivateItem(AActor* Activator) override;
};
