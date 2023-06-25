// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorToSpawn.h"
#include "ActorSpawner.generated.h"

UCLASS()
class PEPTICOM4D_API AActorSpawner : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AActorSpawner();

	UFUNCTION()
	void SpawnActor(FVector& SpawnLocation);

	UFUNCTION()
	void RefreshDataTable();

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
	TArray<TWeakObjectPtr<AActorToSpawn>> SpawnedActors = TArray<TWeakObjectPtr<AActorToSpawn>>();
	TQueue<FVector> ActorSpawnLocations = TQueue<FVector>();
	int32 SpawnActorsPerTick = 5000; // adjust this value as needed to prevent lag

};
