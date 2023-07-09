// Fill out your copyright notice in the Description page of Project Settings.

#include "PythonScriptRunner.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"

UPythonScriptRunner::UPythonScriptRunner() 
{
	// Do nothing
}

struct FAutoProcHandle {
	FProcHandle ProcHandle;
	FAutoProcHandle(const FProcHandle& ProcHandle) : ProcHandle(ProcHandle) {}
	~FAutoProcHandle() {
		if (ProcHandle.IsValid()) {
			FPlatformProcess::CloseProc(ProcHandle);
		}
	}
};

struct FAutoPipeHandle {
	void* ReadPipe;
	void* WritePipe;
	FAutoPipeHandle(void* ReadPipe, void* WritePipe) : ReadPipe(ReadPipe), WritePipe(WritePipe) {}
	~FAutoPipeHandle() {
		if (ReadPipe != nullptr && WritePipe != nullptr) {
			FPlatformProcess::ClosePipe(ReadPipe, WritePipe);
		}
	}
};

void UPythonScriptRunner::RunPythonScript(FString PythonPath, FString ScriptPath, FString Arguments)
{
	FString Command = TEXT("\"") + PythonPath + TEXT("\" \"") + ScriptPath + TEXT("\" ") + Arguments;
	UE_LOG(LogTemp, Warning, TEXT("Executing Python script: %s"), *Command);
	int result = system(TCHAR_TO_ANSI(*Command));
	if (result == 0) {
		UE_LOG(LogTemp, Warning, TEXT("Python script executed successfully"));
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Python script failed with exit code: %d"), result);
	}
	/*
	void* ReadPipe = nullptr;
	void* WritePipe = nullptr;
	if (FPlatformProcess::CreatePipe(ReadPipe, WritePipe)) {
		FAutoPipeHandle AutoPipeHandle(ReadPipe, WritePipe);
		FProcHandle ProcHandle = FPlatformProcess::CreateProc(*Command, nullptr, true, false, false, nullptr, 0, nullptr, WritePipe);
		if (ProcHandle.IsValid()) {
			FAutoProcHandle AutoProcHandle(ProcHandle);
			FString Output;
			while (FPlatformProcess::IsProcRunning(ProcHandle)) {
				Output += FPlatformProcess::ReadPipe(ReadPipe);
				FPlatformProcess::Sleep(0.01);
			}
			Output += FPlatformProcess::ReadPipe(ReadPipe);
			UE_LOG(LogTemp, Warning, TEXT("Python script output: %s"), *Output);
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Python: failed to create process."));
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Python: failed to create pipe."));
	}
	*/
}
