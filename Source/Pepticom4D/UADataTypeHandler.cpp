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

void UADataTypeHandler::AddToManyToOneTableHandlerMap(FString TableName, UATableHandler* TableHandler)
{
	this->ManyToOneTableHandlerMap.Add(TableName, TableHandler);
}

UATableHandler* UADataTypeHandler::GetManyToOneTableHandler(FString TableName)
{
	return ManyToOneTableHandlerMap.FindRef(TableName);
}

UATableHandler* UADataTypeHandler::GetTableHandler(FString TableName)
{
	return TableHandlerMap.FindRef(TableName);
}

void UADataTypeHandler::AddTableHandler(FString TableName, UATableHandler* TableHandler)
{
	this->TableHandlerMap.Add(TableName, TableHandler);
}

void UADataTypeHandler::SetDefaultTableHandler(UATableHandler* TableHandler)
{
	this->DefaultTableHandler = TableHandler;
}

UATableHandler* UADataTypeHandler::GetDefaultTableHandler()
{
	return this->DefaultTableHandler;
}

TMap<FString, UATableHandler*> UADataTypeHandler::GetTableHandlerMap()
{
	return this->TableHandlerMap;
}

void UADataTypeHandler::AddDataPointActor(ADataPointActor* ADataPointActor)
{
	this->DataPointActors.Add(ADataPointActor);
}

TArray<ADataPointActor*> UADataTypeHandler::GetDataPointActors()
{
	return this->DataPointActors;
}

void UADataTypeHandler::PurgeActors()
{
	//check if DataPointActors is null or empty
	UE_LOG(LogTemp, Display, TEXT("Kinjal 0 DataType:  %s"), *Name);
		
	for (auto& Actor : DataPointActors)
	{
		// Destroy the actor
		if(IsValid(Actor))
		{
			Actor->Destroy();
		}
	}

	UE_LOG(LogTemp, Display, TEXT("Kinjal 1 DataType:  %s"), *Name);

	DataPointActors.Empty();

	UE_LOG(LogTemp, Display, TEXT("Kinjal 2 DataType:  %s"), *Name);
}
