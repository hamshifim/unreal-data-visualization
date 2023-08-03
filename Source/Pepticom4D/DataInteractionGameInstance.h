// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ActorSpawner.h"
#include "DataManager.h"
#include "UIManager.h"
#include "DataInteractionGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PEPTICOM4D_API UDataInteractionGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;

private:
	ADataManager* DataManager;
	AUIManager* UIManager;
	AActorSpawner* ActorSpawner;
};
