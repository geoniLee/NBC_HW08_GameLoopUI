// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawmVolume.h"
#include "Components/BoxComponent.h"

// Sets default values
ASpawmVolume::ASpawmVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetupAttachment(Scene);

	ItemDataTable = nullptr;
}

AActor* ASpawmVolume::SpawnRandomItem()
{
	if (FItemSpawnRow* SelectedRow = GetRandomItem()) {
		if (UClass* ActualClass = SelectedRow->ItemClass.Get()) {
			return SpawnItem(ActualClass);
		}
	}	

	return nullptr;
}

FItemSpawnRow* ASpawmVolume::GetRandomItem() const
{
	if (!ItemDataTable) return nullptr;
	
	TArray<FItemSpawnRow*> AllRows;
	static const FString ConstextString(TEXT("ItemSpawnContext"));
	ItemDataTable->GetAllRows(ConstextString, AllRows);

	if (AllRows.IsEmpty()) return nullptr;
	
	float TotalChance = 0;
	for (const FItemSpawnRow* Row : AllRows) {
		if (Row) {
			TotalChance += Row->SpawnChance;
		}
	}

	const float RandValue = FMath::FRandRange(0, TotalChance);
	float AccumulateChance = 0;

	for (FItemSpawnRow* Row : AllRows) {
		AccumulateChance += Row->SpawnChance;
		if (RandValue <= AccumulateChance) {
			return Row;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Can't Find"));
	return nullptr;
}

AActor* ASpawmVolume::SpawnItem(TSubclassOf<AActor> ItemClass)
{ 
	if (!ItemClass) return nullptr;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
		ItemClass,
		GetRandomPointInVolume(),
		FRotator::ZeroRotator
	);

	return SpawnedActor;
}

FVector ASpawmVolume::GetRandomPointInVolume() const
{
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
	FVector BoxOrigin = SpawningBox->GetComponentLocation();

	return BoxOrigin + FVector(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z)
	);
}

