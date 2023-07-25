// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorToSpawn.h"
#include "Engine/DataTable.h"
#include "ActorSpawner.generated.h"

UCLASS()
class PEPTICOM4D_API AActorSpawner : public AActor
{
	GENERATED_BODY()

public:
	AActorSpawner();

	UFUNCTION()
	UDataTable* CreateMetadataTableFromStruct(const FString& TableName, UScriptStruct* RowStruct);

	UFUNCTION()
	void RefreshDataTable(UDataTable* DataTable, FString SourceFileName);

	UFUNCTION()
	void EnqueueSpawningActorsFromDataTable();

	UFUNCTION()
	void SpawnActorsFromQueue();

	UFUNCTION()
	void DestroySpawnedActors();

	UFUNCTION()
	void ForceRefresh();

	UFUNCTION()
	FTableRowBase& GetMetadataFromActor(AActor* Actor);

	UFUNCTION()
	FString GetCurrentFullDatasetName();

	UFUNCTION()
	FString GetFullDatasetNameFromMainAndSubDatasetNames(FString MainDatasetName, FString SubDatasetName);

	UFUNCTION()
	FString GetStructNameFromFullDatasetName(FString FullDatasetName);

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
	// TODO: Change map to be from unique ID within metadata to actor, or create an additional map
	TMap<AActor*, FTableRowBase*> SpawnedActorsMap = TMap<AActor*, FTableRowBase*>();
	// Queue of locations at which to spawn actors; stores pairs of metadata to location
	TQueue<TPair<FTableRowBase*, FVector>> ActorSpawnMetadataLocationPairQueue = TQueue<TPair<FTableRowBase*, FVector>>();
	// Number of actors to spawn per tick
	int32 SpawnActorsPerTick = 500; // adjust this value as needed to prevent lag
	
	UDataTable* SpatialMetadataTable = nullptr;

	// Track all available datasets and the current dataset
	void ProcessConfig(FString ConfigVarName);
	FString CurrentMainDatasetName;
	FString CurrentSubDatasetName;
	// Combination of main and sub dataset names
	FString CurrentFullDatasetName; 
	// Current valid keys for each TPair: "SpatialDataFilePath", "SpatialMetadataFilePath", "POIFilePath". All entries in the map should have pairs with these keys.
	TMap<FString, TMap<FString, FString>> FullDatasetNameToFilePathsMap = TMap<FString, TMap<FString, FString>>();
};



