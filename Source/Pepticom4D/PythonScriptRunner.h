// Fill out your copyright notice in the Description page of Project Settings.
// Set PythonPath to the path of your Python executable. Make sure that all dependencies/packages are installed.

#pragma once

#include "CoreMinimal.h"
#include "PythonScriptRunner.generated.h"

/**
 * 
 */

UCLASS()
class PEPTICOM4D_API UPythonScriptRunner : public UObject
{
	GENERATED_BODY()

public:
	UPythonScriptRunner();

	UFUNCTION(BlueprintCallable, Category="Python")
	static void RunPythonScript(FString PythonPath, FString ScriptPath, FString Arguments);
	
	
};
