// Fill out your copyright notice in the Description page of Project Settings.
// This GameModeBase should be configured in Unreal: Edit -> Project Settings -> Maps & Modes -> Default Modes -> Override GameMode with: DataInteractionGameModeBase
#include "DataInteractionGameModeBase.h"
#include "DataInteractionPlayerController.h"
#include "PlayerCameraPawn.h"
#include "DataInteractionHUD.h"


ADataInteractionGameModeBase::ADataInteractionGameModeBase()
{
    // Set default PlayerController to the required one
    PlayerControllerClass = ADataInteractionPlayerController::StaticClass();
    DefaultPawnClass = APlayerCameraPawn::StaticClass();
    // Set the default HUD class to the one we created
    HUDClass = ADataInteractionHUD::StaticClass();
}

