// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimationControlWidget.h"

#include "Components/Button.h"

UAnimationControlWidget::UAnimationControlWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}


void UAnimationControlWidget::OnAnimationButtonClick()
{
	UE_LOG(LogTemp, Log, TEXT("Hello World!"));
}