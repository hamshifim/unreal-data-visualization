// Fill out your copyright notice in the Description page of Project Settings.


#include "DataInteractionHUD.h"

void ADataInteractionHUD::BeginPlay()
{
	Super::BeginPlay();

	// Store the player controller
    DataInteractionPlayerController = Cast<ADataInteractionPlayerController>(GetWorld()->GetFirstPlayerController());
}

void ADataInteractionHUD::DrawHUD()
{
    // This function gets called every frame, so we don't want to do anything too expensive.

    Super::DrawHUD();

    if (ActorToDrawLineTo) {
        // Get the screen location
        FVector2D ScreenLocation;
        DataInteractionPlayerController->ProjectWorldLocationToScreen(ActorToDrawLineTo->GetActorLocation(), ScreenLocation);
        // Set the end point of the line to be the screen location of the actor
        FVector2D LineEnd = ScreenLocation;
        // Draw the line
        DrawLine(LineStart.X, LineStart.Y, LineEnd.X, LineEnd.Y, FLinearColor::Red, 1.5f);
    }
}
