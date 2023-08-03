// Fill out your copyright notice in the Description page of Project Settings.


#include "DataInteractionGameInstance.h"

void UDataInteractionGameInstance::Init() {
    Super::Init();

    // Spawn and initialize actors here, which are responsible for performing a wide range of tasks. They are initialized in a specific order to avoid any errors.
    DataManager = GetWorld()->SpawnActor<ADataManager>(ADataManager::StaticClass());
    UIManager = GetWorld()->SpawnActor<AUIManager>(AUIManager::StaticClass());
    ActorSpawner = GetWorld()->SpawnActor<AActorSpawner>(AActorSpawner::StaticClass());

    // Store the DataManager instance somewhere (like a UPROPERTY) where AActorSpawner can access it
}
