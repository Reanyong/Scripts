#include "pch.h"
#include "PropertyFunctionManager.h"
#include "Windows.h"

void PropertyFunctionManager::setProperty(const std::string& propertyName, const std::string& value)
{
    properties[propertyName] = value;

    // Visible 속성 변경 시 Viewer에 메시지 전송
    if (propertyName == "Visible")
    {
        HWND hViewer = FindWindow(NULL, "Viewer.exe");
        if (hViewer != NULL)
        {
            struct GRAPHIC_VISIBILITY_MSG
            {
                char szGraphic[256];
                char szObject[256];
                bool bVisible;
            } msgData;

            strcpy_s(msgData.szGraphic, "MainScreen");
            strcpy_s(msgData.szObject, "Button1");
            msgData.bVisible = (value == "true");

            COPYDATASTRUCT cds;
            cds.dwData = 10;  // GM_GV_OBJVISBLE
            cds.cbData = sizeof(msgData);
            cds.lpData = &msgData;

            SendMessage(hViewer, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&cds);
        }
    }
}

std::string PropertyFunctionManager::getProperty(const std::string& propertyName) const
{
    auto it = properties.find(propertyName);
    return (it != properties.end()) ? it->second : "";
}

void PropertyFunctionManager::executeFunction(const std::string& functionName, const std::string& arg)
{
    if (functionName == "SetCurStr")
    {
        properties["CurStr"] = arg;
    }
}
