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

void UADataTypeHandler::SetManyToOneTableHandlerMap(TMap<FString, UATableHandler*> Map)
{
	this->ManyToOneTableHandlerMap = Map;
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

UATableHandler* UADataTypeHandler::GetDefaultTableHandler() const
{
	return this->DefaultTableHandler;
}

TMap<FString, UATableHandler*> UADataTypeHandler::GetTableHandlerMap()
{
	return this->TableHandlerMap;
}
