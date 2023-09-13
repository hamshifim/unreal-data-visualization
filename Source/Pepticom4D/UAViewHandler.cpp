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

FString UAViewHandler::GetBoundaryPointsString()
{
	return this->BoundaryPointsString;
}
