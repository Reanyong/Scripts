#ifndef SYSTEM_OBJECT_H
#define SYSTEM_OBJECT_H

#include "PropertyFunctionManager.h"
#include <map>
#include <string>

class SystemObject
{
private:
    std::map<std::string, SystemObject> subsystems; // Graphic / Something ����
    std::map<std::string, SystemObject> objects;    // Object ����
    PropertyFunctionManager propertyFunction;       // �Ӽ� �� �Լ� ����
    std::string name;

public:
    SystemObject();
    SystemObject(const std::string& objName);

    SystemObject& getSubsystem(const std::string& systemType);
    SystemObject& getObject(const std::string& objectName);
    PropertyFunctionManager& PropertyFunction();
};

#endif
