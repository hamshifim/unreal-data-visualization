// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorSpawner.h"
#include "Components/BoxComponent.h"
#include "SpatialDataStruct.h"
#include "Engine/AssetManager.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Misc/FileHelper.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AActorSpawner::AActorSpawner(): AActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	SpawnVolume->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void AActorSpawner::EnqueueSpawningActorsFromDataTable() {
	/* Using data from the appropriate data table, actors are spawned in the world */
	// Get the spatial data table
	FString SpatialDataTablePath = FString(TEXT("/Game/SpatialDataTable.SpatialDataTable"));
	UDataTable* SpatialDataTable = LoadObject<UDataTable>(NULL, *SpatialDataTablePath, NULL, LOAD_None, NULL);
	// Make sure that we found the data table
	if (SpatialDataTable) {

		// Get all of the row names
		TArray<FName> SpatialDataRowNames = SpatialDataTable->GetRowNames();

		// TArray<FString> CurrentDataTypes = DataManager->ViewHandlerMap.FindRef(DataManager->CurrentViewName)->GetDataTypes();
		//
		// for (const FString DataType : CurrentDataTypes)
		// {
		// 	UADataTypeHandler* DataTypeHandler = DataManager->DataTypeHandlerMap.FindRef(*DataType);
		// 	TMap<FString, UATableHandler*> TableHandlerMap = DataTypeHandler->GetTableHandlerMap();
		//
		// 	//iterate over TableHandlerMap
		// 	for (const auto& TableHandlerPair : TableHandlerMap)
		// 	{
		// 		FString TableName = TableHandlerPair.Key;
		// 		UATableHandler* TableHandler = TableHandlerPair.Value;
		// 	}
		// }

		

		// Get the total number of rows
		int32 NumSpatialDataRows = SpatialDataRowNames.Num();

		// Get the total number of rows across all combined metadata tables
		int32 NumSpatialMetadataRows = 0;
		for (const auto& Pair : DataManager->FullTableNameToSpatialMetadataTableMap) {
			UDataTable* MetadataTable = Pair.Value;
			// Get all rows
			TArray<FName> MetadataRowNames = MetadataTable->GetRowNames();
			// Add the number of rows to the total
			NumSpatialMetadataRows += MetadataRowNames.Num();
		}

		// Store a map of actor type name to the number of actors of that type that have been spawned
		TMap<FString, int32> ActorTypeToNumSpawnedMap;

		// Iterate through the rows while keeping track of the row index
		for (int32 i = 0; i < NumSpatialDataRows; ++i) {
			// Get the spatial data row name
			FName SpatialDataRowName = SpatialDataRowNames[i];
			FSpatialDataStruct* SpatialDataRow = SpatialDataTable->FindRow<FSpatialDataStruct>(SpatialDataRowName, TEXT(""));
			// Get the spatial metadata table given the type name of the current row
			FName TypePropertyName = FName(TEXT("type"));
			FProperty* TypeProperty = FSpatialDataStruct::StaticStruct()->FindPropertyByName(TypePropertyName);
			FString TypeName = TEXT("");
			if (TypeProperty) {
				FStrProperty* StrProp = CastField<FStrProperty>(TypeProperty);
				TypeName = StrProp->GetPropertyValue_InContainer(SpatialDataRow);
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("Could not find type property in spatial data struct"));
				return;
			}
			// Get the full dataset name from the type name
			FString FullDatasetName = DataManager->GetFullDatasetNameFromDataType(TypeName);
			// Get the metadata table from the full dataset name
			UDataTable* SpatialMetadataTable = DataManager->GetMetadataTableFromFullDatasetName(FullDatasetName);
			// Make sure that we found the metadata table
			if (SpatialMetadataTable) {
				// Get all of the row names
				TArray<FName> SpatialMetadataRowNames = SpatialMetadataTable->GetRowNames();

				// Make sure that there are the same number of rows in both tables
				if (NumSpatialDataRows != NumSpatialMetadataRows) {
					
					UE_LOG(LogTemp, Error, TEXT("Shivkook The number of rows in the spatial data table (%d) does not match the number of rows in the spatial metadata table (%d). FullDatasetName: %s"), NumSpatialDataRows, NumSpatialMetadataRows, *FullDatasetName);
					// return;
					continue;
				}

				// Add the type name to the map if it doesn't already exist, or increment the count if it does
				if (ActorTypeToNumSpawnedMap.Contains(TypeName)) {
					ActorTypeToNumSpawnedMap[TypeName] += 1;
				}
				else {
					ActorTypeToNumSpawnedMap.Add(TypeName, 0);
				}
				// Get the corresponding metadata row
				int32 MetadataRowIndex = ActorTypeToNumSpawnedMap[TypeName];
				FName SpatialMetadataRowName = SpatialMetadataRowNames[MetadataRowIndex];
				FTableRowBase* SpatialMetadataRow = SpatialMetadataTable->FindRow<FTableRowBase>(SpatialMetadataRowName, TEXT(""));
				// Make sure that we found the rows
				if (SpatialDataRow && SpatialMetadataRow) {
					// Spawn the actor
					FVector SpawnLocation = FVector(SpatialDataRow->x, SpatialDataRow->y, SpatialDataRow->z);
					auto ActorDataTuple = TTuple<FTableRowBase*, FSpatialDataStruct*, FVector>(SpatialMetadataRow, SpatialDataRow, SpawnLocation);
					ActorDataTupleQueue.Enqueue(ActorDataTuple);
				}
				else {
					UE_LOG(LogTemp, Error, TEXT("Could not find one or more rows in the spatial data table or spatial metadata table"));
				}
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("Could not find spatial metadata table for dataset name %s"), *FullDatasetName);
				return;
			}
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Could not find spatial data table: %s"), *SpatialDataTablePath);
		return;
	}
}

void AActorSpawner::SpawnActorsFromQueue() {
	/* Spawns actors from the queue */
	if (!ActorDataTupleQueue.IsEmpty()) {
		for (int32 i = 0; i < SpawnActorsPerTick; ++i) {
			// Make sure that there are actors to spawn
			if (!ActorDataTupleQueue.IsEmpty()) {
				TTuple<FTableRowBase*, FSpatialDataStruct*, FVector> ActorDataTuple;
				// Get the next spawn location
				ActorDataTupleQueue.Dequeue(ActorDataTuple);
				FTableRowBase* Metadata = ActorDataTuple.Get<0>();
				FSpatialDataStruct* SpatialData = ActorDataTuple.Get<1>();
				FVector SpawnLocation = ActorDataTuple.Get<2>();
				// Set actor rotation to be the same as the rotation of the spawner
				FRotator SpawnRotation = GetActorRotation();
				// Spawn the actor
				ADataPointActor* DataPointActor = GetWorld()->SpawnActor<ADataPointActor>(SpawnLocation, SpawnRotation);

				// Get the spatial data type in order to be able to apply rendering properties (color, size, etc.)
				UStruct* SpatialDataStruct = FSpatialDataStruct::StaticStruct();
				if (DataPointActor) {
					// Check if the spatial data has a color property and set the actor's color to that color
					FName ColorPropertyName = FName(TEXT("color"));
					FProperty* ColorProperty = SpatialDataStruct->FindPropertyByName(ColorPropertyName);
					if (ColorProperty) {
						FStrProperty* StrProp = CastField<FStrProperty>(ColorProperty);
						FString ColorHex = StrProp->GetPropertyValue_InContainer(SpatialData);
						DataPointActor->ChangeColor(ColorHex);
					}
					// Check if the spatial data has a radius property and set the actor's size to that radius
					FName RadiusPropertyName = FName(TEXT("size"));
					FProperty* RadiusProperty = SpatialDataStruct->FindPropertyByName(RadiusPropertyName);
					if (RadiusProperty) {
						FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(RadiusProperty);
						float Radius = DoubleProp->GetPropertyValue_InContainer(SpatialData);
						DataPointActor->ChangeScale(Radius);
					}
					
					// Get the actor's type
					FString ActorDataType = TEXT("");
					FName TypePropertyName = FName(TEXT("type"));
					FProperty* TypeProperty = SpatialDataStruct->FindPropertyByName(TypePropertyName);
					if (TypeProperty) {
						FStrProperty* StrProp = CastField<FStrProperty>(TypeProperty);
						ActorDataType = StrProp->GetPropertyValue_InContainer(SpatialData);
					}
					else {
						UE_LOG(LogTemp, Error, TEXT("Could not find type property for at least one actor"));
						continue;
					}

					DataManager->DataPointActors.Add(DataPointActor);
					
					// Map the actor to its data type
					DataManager->ActorToDataTypeMap.Add(TPair<ADataPointActor*, FString>(DataPointActor, ActorDataType));

					DataPointActor->Initialize(ActorDataType, SpatialData, Metadata);
				}
				else {
					UE_LOG(LogTemp, Warning, TEXT("Actor not ready yet"));
					continue;
				}
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Spawned all actors"));
				break;
			}
		}
	}
}

void AActorSpawner::DestroySpawnedActors() {
	/* Destroys all actors which have been spawned by this actor spawner */

	// Iterate through all actors DataPointActors and destroy them
	for (auto& Actor : DataManager->DataPointActors) {
		// Destroy the actor
		Actor->Destroy();
	}
	
	DataManager->ActorToDataTypeMap.Empty();
}

void AActorSpawner::ForceRefresh() {
	DataManager->ForceRefresh();
	// Destroy all spawned actors and create new ones based on the new data
	DestroySpawnedActors();
	EnqueueSpawningActorsFromDataTable();
}

// Called when the game starts or when spawned
void AActorSpawner::BeginPlay()
{
	Super::BeginPlay();

	// Get and store a reference to the UI manager
	UIManager = Cast<AUIManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AUIManager::StaticClass()));
	if (!UIManager) {
		UE_LOG(LogTemp, Error, TEXT("UI Manager not found in actor spawner"));
		return;
	}
	// Get and store a reference to the data manager
	DataManager = Cast<ADataManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ADataManager::StaticClass()));
	if (!DataManager) {
		UE_LOG(LogTemp, Error, TEXT("Data Manager not found in actor spawner"));
		return;
	}

	// Initial dataset setup and UI configuration based on dataset configuration
	DataManager->ProcessConfig(FString(TEXT("DataConfigFilePath")));
	UIManager->ConfigureDataSelectorWidget();
	UIManager->ConfigureDataFilteringWidget();

	ForceRefresh();
}

// Called every frame
void AActorSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Spawn actors from the queue if the queue is not empty
	SpawnActorsFromQueue();
}

