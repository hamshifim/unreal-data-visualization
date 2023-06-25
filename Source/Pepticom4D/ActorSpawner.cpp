// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorSpawner.h"
#include "Components/BoxComponent.h"
#include "SpatialDataStruct.h"
#include "Engine/DataTable.h"
#include "Engine/AssetManager.h"


// Sets default values
AActorSpawner::AActorSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	SpawnVolume->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

}

void AActorSpawner::SpawnActor(FVector& SpawnLocation)
{
	// Set actor rotation to be the same as the rotation of the spawner
	FRotator SpawnRotation = GetActorRotation();
	// Spawn the actor
	AActorToSpawn* Actor = GetWorld()->SpawnActor<AActorToSpawn>(SpawnLocation, SpawnRotation);
	// Add the spawned actor to the array of spawned actors
	SpawnedActors.Add(Actor);
}

void AActorSpawner::RefreshDataTable() {
	/* Imports data from the source file into the data table */
	// Get the data table
	UDataTable* DataTable = LoadObject<UDataTable>(NULL, TEXT("/Game/SpatialDataTable.SpatialDataTable"), NULL, LOAD_None, NULL);
	// Make sure that we found the data table
	if (DataTable) {
		FString FileName = TEXT("C:/Users/Josh's PC/Documents/Pepticom/Internship/Pepticom 4D/star_spatial_data.csv");
		// Make sure that the file exists
		if (FPaths::FileExists(FileName)) {
			FString FileContent;
			// Read the file into a string
			FFileHelper::LoadFileToString(FileContent, *FileName);
			TArray<FString> problems = DataTable->CreateTableFromCSVString(FileContent);
			// Make sure that there were no problems
			if (problems.Num() > 0) {
				UE_LOG(LogTemp, Warning, TEXT("There were problems importing the data table"));
				for (auto& problem : problems) {
					UE_LOG(LogTemp, Warning, TEXT("%s"), *problem);
				}
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Data table imported successfully"));
			}
		}
	}
}

void AActorSpawner::EnqueueSpawningActorsFromDataTable() {
	/* Using data from the appropriate data table, actors are spawned in the world */
	// Get the data table
	UDataTable* DataTable = LoadObject<UDataTable>(NULL, TEXT("/Game/SpatialDataTable.SpatialDataTable"), NULL, LOAD_None, NULL);
	// Make sure that we found the data table
	if (DataTable) {
		// Get all of the row names
		TArray<FName> RowNames = DataTable->GetRowNames();
		for (auto& Name : RowNames) {
			FSpatialDataStruct* Row = DataTable->FindRow<FSpatialDataStruct>(Name, TEXT(""));
			// Make sure that we found the row
			if (Row) {
				// Spawn the actor
				FVector SpawnLocation = FVector(Row->X, Row->Y, Row->Z);
				ActorSpawnLocations.Enqueue(SpawnLocation);
			}
		}
	}
}

void AActorSpawner::SpawnActorsFromQueue() {
	/* Spawns actors from the queue */
	for (int32 i = 0; i < SpawnActorsPerTick; ++i) {
		// Make sure that there are actors to spawn
		if (!ActorSpawnLocations.IsEmpty()) {
			FVector SpawnLocation;
			// Get the next spawn location
			ActorSpawnLocations.Dequeue(SpawnLocation);
			SpawnActor(SpawnLocation);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Spawned all actors"));
			break;
		}
	}
}

void AActorSpawner::DestroySpawnedActors() {
	/* Destroys all actors which have been spawned by this actor spawner */
	for (auto& Actor : SpawnedActors) {
		Actor->Destroy();
	}
	SpawnedActors.Empty();
}

void AActorSpawner::ForceRefresh() {
	/* Forces a refresh of the data table, destroys spawned actors, and enqueues new actors to spawn from the new data */
	RefreshDataTable();
	DestroySpawnedActors();
	EnqueueSpawningActorsFromDataTable();
	// TODO: This code will also need to generate configuration structs/view-perspective stucts, etc. and reload those data tables IF data has changed since last loaded.
	// Best way to check for changes would be to store some kind of hash of parts of the data, so that the hash is not too large, and compare that to the hash of the current data.
}

// Called when the game starts or when spawned
void AActorSpawner::BeginPlay()
{
	Super::BeginPlay();

	ForceRefresh();
}

// Called every frame
void AActorSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Spawn actors from the queue if the queue is not empty
	SpawnActorsFromQueue();
}

