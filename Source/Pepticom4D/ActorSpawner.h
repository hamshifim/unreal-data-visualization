// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorToSpawn.h"
#include "Engine/DataTable.h"
#include "SpatialMetadataStruct.h"
#include "ActorSpawner.generated.h"

UCLASS()
class PEPTICOM4D_API AActorSpawner : public AActor
{
	GENERATED_BODY()

public:
	AActorSpawner();

	UFUNCTION()
	void RefreshDataTable(FString DataTablePath, FString SourceFileName);

	UFUNCTION()
	void EnqueueSpawningActorsFromDataTable();

	UFUNCTION()
	void SpawnActorsFromQueue();

	UFUNCTION()
	void DestroySpawnedActors();

	UFUNCTION()
	void ForceRefresh();

	UFUNCTION()
	FSpatialMetadataStruct& GetMetadataFromActor(AActor* Actor);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* SpawnVolume;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// Map of actors that have been spawned; key is metadata, value is the actor
	TMap<AActor*, FSpatialMetadataStruct*> SpawnedActorsMap = TMap<AActor*, FSpatialMetadataStruct*>();
	// Queue of locations at which to spawn actors; stores pairs of metadata to location
	TQueue<TPair<FSpatialMetadataStruct*, FVector>> ActorSpawnMetadataLocationPairQueue = TQueue<TPair<FSpatialMetadataStruct*, FVector>>();
	// Number of actors to spawn per tick
	int32 SpawnActorsPerTick = 500; // adjust this value as needed to prevent lag
};



