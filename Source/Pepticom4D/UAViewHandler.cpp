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
