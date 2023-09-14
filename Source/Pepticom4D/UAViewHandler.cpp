#include "UAViewHandler.h"

void UAViewHandler::Initialize(FString AName, TArray<FString> ADataTypeNames)
{
	this->Name = AName;
	this->DataTypeNames = ADataTypeNames;
}

void UAViewHandler::SetAnimationHandlers(TArray<UAAnimationHandler*> AAnimationHandlers)
{
	this->AnimationHandlers = AAnimationHandlers;
}

void UAViewHandler::SetBoundaryPointsString(FString ABoundaryPointsString)
{
	this->BoundaryPointsString = ABoundaryPointsString;
}

void UAViewHandler::SetColorMap(TMap<FString, TMap<FString, FColor>> AColorMap)
{
	this->ColorMap = AColorMap;
}

TMap<FString, TMap<FString, FColor>>* UAViewHandler::GetColorMap()
{
	return &this->ColorMap;
}

FString UAViewHandler::GetBoundaryPointsString()
{
	return this->BoundaryPointsString;
}

TArray<FString> UAViewHandler::GetDataTypes()
{
	return this->DataTypeNames;
}
