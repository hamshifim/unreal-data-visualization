// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorToSpawn.h"
#include "Engine/DataTable.h"
#include "DataManager.h"
#include "UIManager.h"
#include "ActorSpawner.generated.h"

UCLASS()
class PEPTICOM4D_API AActorSpawner : public AActor
{
	GENERATED_BODY()

public:
	AActorSpawner();

	UFUNCTION()
	void EnqueueSpawningActorsFromDataTable();

	UFUNCTION()
	void SpawnActorsFromQueue();

	UFUNCTION()
	void DestroySpawnedActors();

	UFUNCTION()
	void ForceRefresh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* SpawnVolume;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// Queue of locations at which to spawn actors; stores pairs of metadata to location
	TQueue<TPair<FTableRowBase*, FVector>> ActorSpawnMetadataLocationPairQueue = TQueue<TPair<FTableRowBase*, FVector>>();
	// Number of actors to spawn per tick
	int32 SpawnActorsPerTick = 500; // adjust this value as needed to prevent lag
	
	UDataTable* SpatialMetadataTable = nullptr;

	// Store a handler to the UI manager
	AUIManager* UIManager = nullptr;
	// Store a handler to the data manager
	ADataManager* DataManager = nullptr;
};



