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

void AActorSpawner::EnqueueSpawningActorsFromDataTable()
{
	// Store a map of actor type name to the number of actors of that type that have been spawned
	TMap<FString, int32> ActorTypeToNumSpawnedMap;

	TArray<FString> CurrentDataTypes = DataManager->GetCurrentDataTypes();

	for (const FString DataType : CurrentDataTypes)
	{
		UADataTypeHandler* DataTypeHandler = DataManager->DataTypeHandlerMap.FindRef(*DataType);
		TMap<FString, UATableHandler*> TableHandlerMap = DataTypeHandler->GetTableHandlerMap();
		UATableHandler* MainTableHandler = DataTypeHandler->GetDefaultTableHandler();

		UDataTable* SpatialDataTable = MainTableHandler->GetDataTable();
		
		// Get all of the row names
		TArray<FName> SpatialDataRowNames = SpatialDataTable->GetRowNames();
		// Get the total number of rows
		int32 NumSpatialDataRows = SpatialDataRowNames.Num();

		//iterate over TableHandlerMap
		for (const auto& TableHandlerPair : TableHandlerMap)
		{
			FString TableName = TableHandlerPair.Key;
			UATableHandler* TableHandler = TableHandlerPair.Value;
			FString FullTableName = TableHandler->GetFullTableName();
			FString MetaDataTableName = TableHandler->GetTableName();

			UDataTable* SpatialMetadataTable = TableHandler->GetDataTable();

			TArray<FName> SpatialMetadataRowNames = SpatialMetadataTable->GetRowNames();

			int NumSpatialMetadataRows = SpatialMetadataRowNames.Num();
			// Make sure that there are the same number of rows in both tables
			if (NumSpatialDataRows != NumSpatialMetadataRows)
			{
				UE_LOG(LogTemp, Error,
				       TEXT(
					       "Shivkook The number of rows in the spatial data table (%d) does not match the number of rows in the spatial metadata table (%d). DataType: %s"
				       ), NumSpatialDataRows, NumSpatialMetadataRows, *FullTableName);
				// return;
				continue;
			}

			// Iterate through the rows while keeping track of the row index
			for (int32 i = 0; i < NumSpatialDataRows; ++i)
			{
				// Get the spatial data row name
				FName SpatialDataRowName = SpatialDataRowNames[i];
				FSpatialDataStruct* SpatialDataRow = SpatialDataTable->FindRow<FSpatialDataStruct>(
					SpatialDataRowName, TEXT(""));
				// Get the spatial metadata table given the type name of the current row
				FName TypePropertyName = FName(TEXT("type"));
				FProperty* TypeProperty = FSpatialDataStruct::StaticStruct()->FindPropertyByName(TypePropertyName);
				FString TypeName = TEXT("");
				if (TypeProperty)
				{
					FStrProperty* StrProp = CastField<FStrProperty>(TypeProperty);
					TypeName = StrProp->GetPropertyValue_InContainer(SpatialDataRow);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Could not find type property in spatial data struct"));
					return;
				}

				// Add the type name to the map if it doesn't already exist, or increment the count if it does
				if (ActorTypeToNumSpawnedMap.Contains(TypeName))
				{
					ActorTypeToNumSpawnedMap[TypeName] += 1;
				}
				else
				{
					ActorTypeToNumSpawnedMap.Add(TypeName, 0);
				}
				// Get the corresponding metadata row
				int32 MetadataRowIndex = ActorTypeToNumSpawnedMap[TypeName];
				FName SpatialMetadataRowName = SpatialMetadataRowNames[MetadataRowIndex];
				FTableRowBase* SpatialMetadataRow = SpatialMetadataTable->FindRow<FTableRowBase>(
					SpatialMetadataRowName, TEXT(""));

				//Get the FString version of SpatialDataRowName
				FString Index = SpatialDataRowName.ToString();
				
				// Make sure that we found the rows
				if (SpatialDataRow && SpatialMetadataRow)
				{
					// Spawn the actor
					FVector SpawnLocation = FVector(SpatialDataRow->x, SpatialDataRow->y, SpatialDataRow->z);
					auto ActorDataTuple = TTuple<FTableRowBase*, FSpatialDataStruct*, FVector, FString, FString, FString>(SpatialMetadataRow, SpatialDataRow, SpawnLocation, DataType, MetaDataTableName, Index);
					ActorDataTupleQueue.Enqueue(ActorDataTuple);
				}
				else
				{
					UE_LOG(LogTemp, Error,
					       TEXT("Could not find one or more rows in the spatial data table or spatial metadata table"));
				}
			}
		}
	}
}

void AActorSpawner::SpawnActorsFromQueue()
{
	//while ActorDataTupleQueue is not empty
	while (!ActorDataTupleQueue.IsEmpty())
	{
					// Make sure that there are actors to spawn
		TTuple<FTableRowBase*, FSpatialDataStruct*, FVector, FString, FString, FString> ActorDataTuple;
		// Get the next spawn location
		ActorDataTupleQueue.Dequeue(ActorDataTuple);
		FTableRowBase* Metadata = ActorDataTuple.Get<0>();
		FSpatialDataStruct* SpatialData = ActorDataTuple.Get<1>();
		FVector SpawnLocation = ActorDataTuple.Get<2>();

		FString DataType = ActorDataTuple.Get<3>();
		FString MetaDataTableName = ActorDataTuple.Get<4>();
		FString Index = ActorDataTuple.Get<5>();
		
		// Set actor rotation to be the same as the rotation of the spawner
		FRotator SpawnRotation = GetActorRotation();
		// Spawn the actor
		ADataPointActor* DataPointActor = GetWorld()->SpawnActor<ADataPointActor>(SpawnLocation, SpawnRotation);

		// Get the spatial data type in order to be able to apply rendering properties (color, size, etc.)
		UStruct* SpatialDataStruct = FSpatialDataStruct::StaticStruct();
		if (DataPointActor)
		{
			// Check if the spatial data has a color property and set the actor's color to that color
			FName ColorPropertyName = FName(TEXT("color"));
			FProperty* ColorProperty = SpatialDataStruct->FindPropertyByName(ColorPropertyName);
			if (ColorProperty)
			{
				FStrProperty* StrProp = CastField<FStrProperty>(ColorProperty);
				FString ColorHex = StrProp->GetPropertyValue_InContainer(SpatialData);
				DataPointActor->ChangeColor(ColorHex);
			}
			// Check if the spatial data has a radius property and set the actor's size to that radius
			FName RadiusPropertyName = FName(TEXT("size"));
			FProperty* RadiusProperty = SpatialDataStruct->FindPropertyByName(RadiusPropertyName);
			if (RadiusProperty)
			{
				FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(RadiusProperty);
				float Radius = DoubleProp->GetPropertyValue_InContainer(SpatialData);
				DataPointActor->ChangeScale(Radius);
			}

			//add actor to data type handler
			DataManager->DataTypeHandlerMap.FindRef(DataType)->AddDataPointActor(DataPointActor);

			DataPointActor->Initialize(DataType, MetaDataTableName, SpatialData, Metadata, Index);
			NumActorsSpawned++;

			//log the number of actors spawned
			UE_LOG(LogTemp, Display, TEXT("Number of actors of data type: %s spawned: %d"), *DataType, NumActorsSpawned);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Actor not ready yet"));
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Spawned all actors"));
}

void AActorSpawner::DestroySpawnedActors()
{
	/* Destroys all actors which have been spawned by this actor spawner */
	
	for (const FString DataType : DataManager->GetCurrentDataTypes())
	{
		// Iterate through all actors DataPointActors in of the data type and destroy them
		TArray<ADataPointActor*> DataPointActors = DataManager->DataTypeHandlerMap.FindRef(*DataType)->GetDataPointActors();
		for (auto& Actor : DataPointActors)
		{
			// Destroy the actor
			Actor->Destroy();
		}

		DataPointActors.Empty();
	}
}


void AActorSpawner::OnViewChange(FString ViewName)
{
	DataManager->SetCurrentView(ViewName);
	ForceRefresh();
}


void AActorSpawner::ForceRefresh()
{
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
	if (!UIManager)
	{
		UE_LOG(LogTemp, Error, TEXT("UI Manager not found in actor spawner"));
		return;
	}
	// Get and store a reference to the data manager
	DataManager = Cast<ADataManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ADataManager::StaticClass()));
	if (!DataManager)
	{
		UE_LOG(LogTemp, Error, TEXT("Data Manager not found in actor spawner"));
		return;
	}

	// Initial dataset setup and UI configuration based on dataset configuration
	DataManager->ProcessConfig(FString(TEXT("DataConfigFilePath")));
	
	UIManager->ConfigureDataFilteringWidget();
	UE_LOG(LogTemp, Display, TEXT("shiflif: 0"));
	UIManager->ConfigureUserControlWidget();
	UE_LOG(LogTemp, Display, TEXT("shiflif: 1"));

	ForceRefresh();

	// Spawn actors from the queue if the queue is not empty
	SpawnActorsFromQueue();
	UIManager->RefreshUI();
}

// Called every frame
void AActorSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
