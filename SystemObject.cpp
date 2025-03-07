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
    return objects[objectName]; // Object("��ü��") ����
}

PropertyFunctionManager& SystemObject::PropertyFunction()
{
    return propertyFunction; // �Ӽ� �� �Լ� ���� ��ȯ
}
