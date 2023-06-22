// Fill out your copyright notice in the Description page of Project Settings.
// This GameModeBase should be configured in Unreal: Edit -> Project Settings -> Maps & Modes -> Default Modes -> Override GameMode with: DataInteractionGameModeBase
#include "DataInteractionGameModeBase.h"
#include "DataInteractionPlayerController.h"

ADataInteractionGameModeBase::ADataInteractionGameModeBase()
{
    // Set default PlayerController to the required one
    PlayerControllerClass = ADataInteractionPlayerController::StaticClass();
}

