// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataPointActor.h"
#include "Engine/DataTable.h"
#include "ADataManager.h"
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

	UFUNCTION()
	void OnViewChange(FString ViewName);

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
	TQueue<TTuple<FTableRowBase*, FSpatialDataStruct*, FVector, FString, FString, FString>> ActorDataTupleQueue = TQueue<TTuple<FTableRowBase*, FSpatialDataStruct*, FVector, FString, FString, FString>>();
	
	int NumActorsSpawned = 0;
	// Store a handler to the UI manager
	UPROPERTY()
	AUIManager* UIManager = nullptr;
	// Store a handler to the data manager
	UPROPERTY()
	ADataManager* DataManager = nullptr;
};



