#ifndef SYSTEM_OBJECT_H
#define SYSTEM_OBJECT_H

#include "PropertyFunctionManager.h"
#include <map>
#include <string>

class SystemObject
{
private:
    std::map<std::string, SystemObject> subsystems; // Graphic / Something 包府
    std::map<std::string, SystemObject> objects;    // Object 包府
    PropertyFunctionManager propertyFunction;       // 加己 棺 窃荐 包府
    std::string name;

public:
    SystemObject();
    SystemObject(const std::string& objName);

    SystemObject& getSubsystem(const std::string& systemType);
    SystemObject& getObject(const std::string& objectName);
    PropertyFunctionManager& PropertyFunction();
};

#endif
