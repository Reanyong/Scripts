#ifndef PROPERTY_FUNCTION_MANAGER_H
#define PROPERTY_FUNCTION_MANAGER_H

#include <map>
#include <string>

class PropertyFunctionManager
{
private:
    std::map<std::string, std::string> properties; // 속성 저장 (예: "Visible" → "true")

public:
    void setProperty(const std::string& propertyName, const std::string& value);
    std::string getProperty(const std::string& propertyName) const;

    void executeFunction(const std::string& functionName, const std::string& arg);
};

#endif
