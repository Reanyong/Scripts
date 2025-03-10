// ScriptSystem.cpp
#include "pch.h"
#include "ScriptSystem.h"
#include "iWaterScriptFunc.h"

int GetReturnVal(int nTimeout, ST_GLOBAL& global, char* pszBuffer)
{
    GUID guid;
    CoCreateGuid(&guid);

    sprintf(global.szParms7, "\\\\.\\mailslot\\%x%x%x%x%x%x%x%x%x%x%x", guid.Data1, guid.Data2, guid.Data3,
        guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
        guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

    HANDLE hMailslot = CreateMailslot(global.szParms7,
        1024,
        MAILSLOT_WAIT_FOREVER,
        NULL);

    if (hMailslot == INVALID_HANDLE_VALUE)
    {
        return -1;
    }

    COPYDATASTRUCT cds;
    ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
    cds.dwData = GM_COPYDATA_SCRIPT_CODE;
    cds.cbData = sizeof(ST_GLOBAL);
    cds.lpData = &global;

    if (!KWSendMsgToViewer(cds))
    {
        CloseHandle(hMailslot);
        return -2;
    }

    BOOL bReadSucc = FALSE;
    ULONGLONG ull = GetTickCount64();

    try
    {
        for (;;)
        {
            DWORD msgSize;
            BOOL err = GetMailslotInfo(hMailslot, 0, &msgSize, 0, 0);

            if (!err)
            {
                goto err_exit;
            }

            if (msgSize != (DWORD)MAILSLOT_NO_MESSAGE)
            {
                void* buffer = GlobalAlloc(GMEM_FIXED, msgSize);
                if (!buffer)
                {
                    // �޸� �Ҵ� ����
                }
                else
                {
                    DWORD numRead;
                    err = ReadFile(hMailslot, buffer, msgSize, &numRead, 0);

                    if (!err)
                    {
                        // �б� ����
                    }
                    else if (msgSize != numRead)
                    {
                        // ����Ʈ �� ����ġ
                    }
                    else
                    {
                        bReadSucc = TRUE;
                        sprintf(pszBuffer, (char*)buffer);
                        pszBuffer[msgSize] = 0;
                    }

                    GlobalFree(buffer);
                }
            }

            if (bReadSucc == TRUE)
            {
                break;
            }

            if ((DWORD)nTimeout < GetTickCount64() - ull)
            {
                break;
            }

            Sleep(5);
        }
    }
    catch (...)
    {
        // ���� ó��
    }

err_exit:
    CloseHandle(hMailslot);

    if (bReadSucc == TRUE)
        return 1;

    return -1;
}

// �̱��� �ʱ�ȭ
CScriptObjectManager* CScriptObjectManager::s_instance = nullptr;

CScriptObjectManager* CScriptObjectManager::GetInstance()
{
    if (s_instance == nullptr) {
        s_instance = new CScriptObjectManager();
    }
    return s_instance;
}

CScriptObjectManager::~CScriptObjectManager()
{
    Clear();
}

void CScriptObjectManager::Clear()
{
    // �׷��� ��ü�� ����
    for (auto& pair : m_graphics) {
        delete pair.second;
    }
    m_graphics.clear();

    // ���� ��ü�� ����
    for (auto& pair : m_objects) {
        delete pair.second;
    }
    m_objects.clear();
}

CScriptGraphic* CScriptObjectManager::GetGraphic(const char* name)
{
    std::string key = name;

    // ���� ��ü�� ������ ��ȯ
    auto it = m_graphics.find(key);
    if (it != m_graphics.end()) {
        return it->second;
    }

    // ������ ���� ����
    CScriptGraphic* pGraphic = new CScriptGraphic(name);
    m_graphics[key] = pGraphic;
    return pGraphic;
}

CScriptGraphicObject* CScriptObjectManager::GetObject(const char* graphicName, const char* objectName)
{
    std::string key = std::string(graphicName) + "::" + objectName;

    // ���� ��ü�� ������ ��ȯ
    auto it = m_objects.find(key);
    if (it != m_objects.end()) {
        return it->second;
    }

    // ������ ���� ����
    CScriptGraphicObject* pObject = new CScriptGraphicObject(graphicName, objectName);
    m_objects[key] = pObject;
    return pObject;
}

// �׷��� ��ü ����
CScriptGraphicObject::CScriptGraphicObject(const char* graphicName, const char* objectName)
    : m_graphicName(graphicName), m_objectName(objectName)
{
}

bool CScriptGraphicObject::GetVisible(c_variable& result)
{
    // Viewer�� ��ü ���ü� ���� ��û
    ST_GLOBAL global;
    ZeroMemory(&global, sizeof(ST_GLOBAL));
    global.nMode = GM_GV_Get_OBJVISBLE;

    strcpy_s(global.szParms1, m_graphicName.get_buffer());
    strcpy_s(global.szParms2, m_objectName.get_buffer());

    COPYDATASTRUCT cds;
    ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
    cds.dwData = GM_COPYDATA_SCRIPT_CODE;
    cds.cbData = sizeof(ST_GLOBAL);
    cds.lpData = &global;

    char buf[1024] = { 0 };
    int nRet = GetReturnVal(3000, global, buf);
    if (nRet == 1)
    {
        result = (atoi(buf) == 1);
        return true;
    }

    result = false;
    return false;
}

bool CScriptGraphicObject::SetVisible(c_variable& value)
{
    // Viewer�� ��ü ���ü� ���� ��û
    ST_GLOBAL global;
    ZeroMemory(&global, sizeof(ST_GLOBAL));
    global.nMode = GM_GV_OBJVISBLE;

    strcpy_s(global.szParms1, m_graphicName.get_buffer());
    strcpy_s(global.szParms2, m_objectName.get_buffer());
    strcpy_s(global.szParms3, value.as_integer() ? "1" : "0");

    COPYDATASTRUCT cds;
    ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
    cds.dwData = GM_COPYDATA_SCRIPT_CODE;
    cds.cbData = sizeof(ST_GLOBAL);
    cds.lpData = &global;

    return KWSendMsgToViewer(cds);
}

bool CScriptGraphicObject::GetText(c_variable& result)
{
    // Viewer�� ��ü �ؽ�Ʈ ���� ��û
    ST_GLOBAL global;
    ZeroMemory(&global, sizeof(ST_GLOBAL));
    global.nMode = GM_GV_Get_OBJTEXT;

    strcpy_s(global.szParms1, m_graphicName.get_buffer());
    strcpy_s(global.szParms2, m_objectName.get_buffer());

    COPYDATASTRUCT cds;
    ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
    cds.dwData = GM_COPYDATA_SCRIPT_CODE;
    cds.cbData = sizeof(ST_GLOBAL);
    cds.lpData = &global;

    char buf[1024] = { 0 };
    int nRet = GetReturnVal(3000, global, buf);
    if (nRet == 1)
    {
        result = buf;
        return true;
    }

    result = "";
    return false;
}

bool CScriptGraphicObject::SetText(c_variable& value)
{
    // Viewer�� ��ü �ؽ�Ʈ ���� ��û
    ST_GLOBAL global;
    ZeroMemory(&global, sizeof(ST_GLOBAL));
    global.nMode = GM_GV_OBJTEXT;

    c_string strValue;
    value.as_string(strValue);

    strcpy_s(global.szParms1, m_graphicName.get_buffer());
    strcpy_s(global.szParms2, m_objectName.get_buffer());
    strcpy_s(global.szParms3, strValue.get_buffer());

    COPYDATASTRUCT cds;
    ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
    cds.dwData = GM_COPYDATA_SCRIPT_CODE;
    cds.cbData = sizeof(ST_GLOBAL);
    cds.lpData = &global;

    return KWSendMsgToViewer(cds);
}

// �׷��� Ŭ���� ����
CScriptGraphic::CScriptGraphic(const char* name)
    : m_name(name)
{
}

CScriptGraphicObject* CScriptGraphic::GetObject(const char* objectName)
{
    return CScriptObjectManager::GetInstance()->GetObject(m_name.get_buffer(), objectName);
}

// �ý��� Ŭ���� ����
CScriptGraphic* CScriptSystem::GetGraphic(const char* graphicName)
{
    return CScriptObjectManager::GetInstance()->GetGraphic(graphicName);
}

// Ȯ�� �Լ� ����
void __stdcall System_Graphic(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
    if (nargs != 1 || pargs[0]->vt != VT_BSTR) {
        result = INT_MIN;
        return;
    }

    c_string graphicName;
    pargs[0]->as_string(graphicName);

    // �׷��� ��ü ���� ID ���� (�޸� �ּҸ� ���ڿ��� ��ȯ)
    CScriptGraphic* pGraphic = CScriptSystem::GetGraphic(graphicName.get_buffer());
    char buffer[32];
    sprintf_s(buffer, "_GRAPHIC_%p", pGraphic);

    // ���ڿ��� ��ȯ
    result = buffer;
}

void __stdcall Graphic_Object(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
    if (nargs != 2 || pargs[0]->vt != VT_BSTR || pargs[1]->vt != VT_BSTR) {
        result = INT_MIN;
        return;
    }

    c_string graphicRef, objectName;
    pargs[0]->as_string(graphicRef);
    pargs[1]->as_string(objectName);

    // �׷��� ��ü �������� �޸� �ּ� ����
    CScriptGraphic* pGraphic = nullptr;
    sscanf_s(graphicRef.get_buffer(), "_GRAPHIC_%p", &pGraphic);

    if (!pGraphic) {
        result = INT_MIN;
        return;
    }

    // ��ü ���� ID ����
    CScriptGraphicObject* pObject = pGraphic->GetObject(objectName.get_buffer());
    char buffer[64];
    sprintf_s(buffer, "_OBJECT_%p", pObject);

    // ���ڿ��� ��ȯ
    result = buffer;
}

void __stdcall Object_GetVisible(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
    if (nargs != 1 || pargs[0]->vt != VT_BSTR) {
        result = INT_MIN;
        return;
    }

    c_string objectRef;
    pargs[0]->as_string(objectRef);

    // ��ü �������� �޸� �ּ� ����
    CScriptGraphicObject* pObject = nullptr;
    sscanf_s(objectRef.get_buffer(), "_OBJECT_%p", &pObject);

    if (!pObject) {
        result = INT_MIN;
        return;
    }

    // ��ü�� ���ü� ���� ��������
    pObject->GetVisible(result);
}

void __stdcall Object_SetVisible(int nargs, c_variable** pargs, c_engine* p_engine)
{
    if (nargs != 2 || pargs[0]->vt != VT_BSTR) {
        return;
    }

    c_string objectRef;
    pargs[0]->as_string(objectRef);

    // ��ü �������� �޸� �ּ� ����
    CScriptGraphicObject* pObject = nullptr;
    sscanf_s(objectRef.get_buffer(), "_OBJECT_%p", &pObject);

    if (!pObject) {
        return;
    }

    // ��ü�� ���ü� ����
    pObject->SetVisible(*pargs[1]);
}

// �ܺο��� ȣ���� �� �ִ� System_SetProperty �Լ� ����
bool __cdecl System_SetProperty(const char* path, VARIANT* value)
{
    // ��� ���� Ȯ�� �� �Ľ�
    if (!path || !value)
        return false;

    // "$System."���� �����ϴ��� Ȯ��
    if (strncmp(path, "$System.", 8) != 0)
        return false;

    // ��θ� �Ľ��ϱ� ���� �ӽ� ����
    char buffer[1024] = { 0 };
    strcpy_s(buffer, path);

    // "$System." ������ ���ڿ��� �м�
    char* token = buffer + 8;  // "$System." ���� �κк���
    char* nextToken = NULL;

    // ù ��° ��ū �м� (��: Graphic)
    char* component = strtok_s(token, ".()", &nextToken);
    if (!component)
        return false;

    // Graphic ó��
    if (_stricmp(component, "Graphic") == 0)
    {
        // ����� ����
        char drawingName[MAX_PATH] = { 0 };
        component = strtok_s(NULL, "\"", &nextToken);
        if (!component)
            return false;
        strcpy_s(drawingName, component);

        // ���� �κ� ���� (��: Object)
        component = strtok_s(NULL, ".()", &nextToken);
        if (!component || _stricmp(component, "Object") != 0)
            return false;

        // ��ü�� ����
        char objectName[MAX_PATH] = { 0 };
        component = strtok_s(NULL, "\"", &nextToken);
        if (!component)
            return false;
        strcpy_s(objectName, component);

        // �Ӽ��� ����
        component = strtok_s(NULL, ".()", &nextToken);
        if (!component)
            return false;

        // Visible �Ӽ� ó��
        if (_stricmp(component, "Visible") == 0)
        {
            // CScriptObjectManager�� ���� ��ü ����
            CScriptGraphicObject* pObject = CScriptObjectManager::GetInstance()->GetObject(drawingName, objectName);
            if (!pObject)
                return false;

            // VARIANT�� c_variable�� ��ȯ
            c_variable val;
            if (value->vt == VT_BOOL)
                val = (value->boolVal != VARIANT_FALSE);
            else if (value->vt == VT_I4)
                val = (value->lVal != 0);
            else
                val = 1; // �⺻��

            // ���ü� ����
            return pObject->SetVisible(val);
        }
        // �ٸ� �Ӽ� ó�� (�ʿ��ϸ� �߰�)
        // ...
    }

    return false;
}

// ��ũ��Ʈ �������� ȣ���ϴ� Ȯ�� �Լ� (4�� �Ķ���� ����)
void __stdcall System_SetProperty(int nargs, c_variable** pargs, c_engine* p_engine)
{
    // ���� ����
    if (nargs != 2 || pargs[0]->vt != VT_BSTR)
        return;

    c_string path;
    pargs[0]->as_string(path);

    // VARIANT ����
    VARIANT value;
    VariantInit(&value);

    // c_variable�� VARIANT�� ��ȯ
    switch (pargs[1]->vt)
    {
    case VT_I4:
        value.vt = VT_I4;
        value.lVal = pargs[1]->lVal;
        break;
    case VT_R8:
        value.vt = VT_R8;
        value.dblVal = pargs[1]->dblVal;
        break;
    case VT_BOOL:
        value.vt = VT_BOOL;
        value.boolVal = pargs[1]->boolVal;
        break;
    case VT_BSTR:
        value.vt = VT_BSTR;
        value.bstrVal = SysAllocString(pargs[1]->bstrVal);
        break;
    default:
        value.vt = VT_I4;
        value.lVal = pargs[1]->as_integer();
        break;
    }

    // ���� ���� �Լ� ȣ��
    System_SetProperty(path.get_buffer(), &value);

    // VARIANT ����
    VariantClear(&value);
}

// ���� ���� �Լ�
bool _check_System_SetProperty(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
    if (n != 2)
    {
        *p_msg = "SetProperty �Լ��� ��ο� �� 2���� ���ڰ� �ʿ��մϴ�.";
        return false;
    }
    return true;
}

// üũ �Լ� ����
bool _check_System_Graphic(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
    if (n != 1) {
        *p_msg = "Graphic �Լ��� ���� �̸� ���ڰ� �ʿ��մϴ�.";
        return false;
    }
    return true;
}

bool _check_Graphic_Object(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
    if (n != 2) {
        *p_msg = "Object �Լ��� �׷��� ������ ��ü �̸� ���ڰ� �ʿ��մϴ�.";
        return false;
    }
    return true;
}

bool _check_Object_GetVisible(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
    if (n != 1) {
        *p_msg = "GetVisible �Լ��� ��ü ���� ���ڰ� �ʿ��մϴ�.";
        return false;
    }
    return true;
}

bool _check_Object_SetVisible(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
    if (n != 2) {
        *p_msg = "SetVisible �Լ��� ��ü ������ �� ���ڰ� �ʿ��մϴ�.";
        return false;
    }
    return true;
}