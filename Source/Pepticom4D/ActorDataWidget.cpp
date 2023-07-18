// Fill out your copyright notice in the Description page of Project Settings.
#include "ActorDataWidget.h"

UActorDataWidget::UActorDataWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // your widget initialization code
}

void UActorDataWidget::AnimateIn(float AnimationDuration)
{
    // Set the widget's opacity to 0
    SetRenderOpacity(0.f);

    // Make the widget visible
    SetVisibility(ESlateVisibility::Visible);

    if (AnimationDuration <= 0.f) return;

    const float Step = 1.f / AnimationDuration;

    FTimerHandle AnimationTimer;
    GetWorld()->GetTimerManager().SetTimer(AnimationTimer, [this, Step]()
        {
            const float CurrentOpacity = GetRenderOpacity();
            if (CurrentOpacity >= 1.f) return;

            SetRenderOpacity(FMath::Min(1.f, CurrentOpacity + Step));
        }, AnimationDuration / 30.f, true);
}

