#include "pch.h"
#include "SystemObject.h"

SystemObject::SystemObject() : name("System") {}

SystemObject::SystemObject(const std::string& objName) : name(objName) {}

SystemObject& SystemObject::getSubsystem(const std::string& systemType)
{
    return subsystems[systemType];
}

SystemObject& SystemObject::getObject(const std::string& objectName)
{
    return objects[objectName]; // Object("按眉疙") 包府
}

PropertyFunctionManager& SystemObject::PropertyFunction()
{
    return propertyFunction; // 加己 棺 窃荐 包府 馆券
}
