#include "UADataTypeHandler.h"


// An Initialization of the necessary variables
void UADataTypeHandler::Initialize(FString DataTypeName, FString ADefaultTableName)
{
	this->Name = DataTypeName;
	this->DefaultTableName = ADefaultTableName;
}

void UADataTypeHandler::Sanity()
{
	UE_LOG(LogTemp, Display, TEXT("Kadlaomer you bastard: %s."), *this->Name);
}

FString UADataTypeHandler::GetDefaultTableName()
{
	return DefaultTableName;
}
