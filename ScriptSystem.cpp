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
                    // 메모리 할당 실패
                }
                else
                {
                    DWORD numRead;
                    err = ReadFile(hMailslot, buffer, msgSize, &numRead, 0);

                    if (!err)
                    {
                        // 읽기 실패
                    }
                    else if (msgSize != numRead)
                    {
                        // 바이트 수 불일치
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
        // 예외 처리
    }

err_exit:
    CloseHandle(hMailslot);

    if (bReadSucc == TRUE)
        return 1;

    return -1;
}

// 싱글톤 초기화
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
    // 그래픽 객체들 정리
    for (auto& pair : m_graphics) {
        delete pair.second;
    }
    m_graphics.clear();

    // 개별 객체들 정리
    for (auto& pair : m_objects) {
        delete pair.second;
    }
    m_objects.clear();
}

CScriptGraphic* CScriptObjectManager::GetGraphic(const char* name)
{
    std::string key = name;

    // 기존 객체가 있으면 반환
    auto it = m_graphics.find(key);
    if (it != m_graphics.end()) {
        return it->second;
    }

    // 없으면 새로 생성
    CScriptGraphic* pGraphic = new CScriptGraphic(name);
    m_graphics[key] = pGraphic;
    return pGraphic;
}

CScriptGraphicObject* CScriptObjectManager::GetObject(const char* graphicName, const char* objectName)
{
    std::string key = std::string(graphicName) + "::" + objectName;

    // 기존 객체가 있으면 반환
    auto it = m_objects.find(key);
    if (it != m_objects.end()) {
        return it->second;
    }

    // 없으면 새로 생성
    CScriptGraphicObject* pObject = new CScriptGraphicObject(graphicName, objectName);
    m_objects[key] = pObject;
    return pObject;
}

// 그래픽 객체 구현
CScriptGraphicObject::CScriptGraphicObject(const char* graphicName, const char* objectName)
    : m_graphicName(graphicName), m_objectName(objectName)
{
}

bool CScriptGraphicObject::GetVisible(c_variable& result)
{
    // Viewer에 객체 가시성 상태 요청
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
    // Viewer에 객체 가시성 변경 요청
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
    // Viewer에 객체 텍스트 상태 요청
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
    // Viewer에 객체 텍스트 변경 요청
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

// 그래픽 클래스 구현
CScriptGraphic::CScriptGraphic(const char* name)
    : m_name(name)
{
}

CScriptGraphicObject* CScriptGraphic::GetObject(const char* objectName)
{
    return CScriptObjectManager::GetInstance()->GetObject(m_name.get_buffer(), objectName);
}

// 시스템 클래스 구현
CScriptGraphic* CScriptSystem::GetGraphic(const char* graphicName)
{
    return CScriptObjectManager::GetInstance()->GetGraphic(graphicName);
}

// 확장 함수 구현
void __stdcall System_Graphic(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
    if (nargs != 1 || pargs[0]->vt != VT_BSTR) {
        result = INT_MIN;
        return;
    }

    c_string graphicName;
    pargs[0]->as_string(graphicName);

    // 그래픽 객체 고유 ID 생성 (메모리 주소를 문자열로 변환)
    CScriptGraphic* pGraphic = CScriptSystem::GetGraphic(graphicName.get_buffer());
    char buffer[32];
    sprintf_s(buffer, "_GRAPHIC_%p", pGraphic);

    // 문자열로 반환
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

    // 그래픽 객체 참조에서 메모리 주소 추출
    CScriptGraphic* pGraphic = nullptr;
    sscanf_s(graphicRef.get_buffer(), "_GRAPHIC_%p", &pGraphic);

    if (!pGraphic) {
        result = INT_MIN;
        return;
    }

    // 객체 고유 ID 생성
    CScriptGraphicObject* pObject = pGraphic->GetObject(objectName.get_buffer());
    char buffer[64];
    sprintf_s(buffer, "_OBJECT_%p", pObject);

    // 문자열로 반환
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

    // 객체 참조에서 메모리 주소 추출
    CScriptGraphicObject* pObject = nullptr;
    sscanf_s(objectRef.get_buffer(), "_OBJECT_%p", &pObject);

    if (!pObject) {
        result = INT_MIN;
        return;
    }

    // 객체의 가시성 상태 가져오기
    pObject->GetVisible(result);
}

void __stdcall Object_SetVisible(int nargs, c_variable** pargs, c_engine* p_engine)
{
    if (nargs != 2 || pargs[0]->vt != VT_BSTR) {
        return;
    }

    c_string objectRef;
    pargs[0]->as_string(objectRef);

    // 객체 참조에서 메모리 주소 추출
    CScriptGraphicObject* pObject = nullptr;
    sscanf_s(objectRef.get_buffer(), "_OBJECT_%p", &pObject);

    if (!pObject) {
        return;
    }

    // 객체의 가시성 설정
    pObject->SetVisible(*pargs[1]);
}

// 외부에서 호출할 수 있는 System_SetProperty 함수 구현
bool __cdecl System_SetProperty(const char* path, VARIANT* value)
{
    // 경로 형식 확인 및 파싱
    if (!path || !value)
        return false;

    // "$System."으로 시작하는지 확인
    if (strncmp(path, "$System.", 8) != 0)
        return false;

    // 경로를 파싱하기 위한 임시 버퍼
    char buffer[1024] = { 0 };
    strcpy_s(buffer, path);

    // "$System." 이후의 문자열을 분석
    char* token = buffer + 8;  // "$System." 다음 부분부터
    char* nextToken = NULL;

    // 첫 번째 토큰 분석 (예: Graphic)
    char* component = strtok_s(token, ".()", &nextToken);
    if (!component)
        return false;

    // Graphic 처리
    if (_stricmp(component, "Graphic") == 0)
    {
        // 도면명 추출
        char drawingName[MAX_PATH] = { 0 };
        component = strtok_s(NULL, "\"", &nextToken);
        if (!component)
            return false;
        strcpy_s(drawingName, component);

        // 다음 부분 추출 (예: Object)
        component = strtok_s(NULL, ".()", &nextToken);
        if (!component || _stricmp(component, "Object") != 0)
            return false;

        // 객체명 추출
        char objectName[MAX_PATH] = { 0 };
        component = strtok_s(NULL, "\"", &nextToken);
        if (!component)
            return false;
        strcpy_s(objectName, component);

        // 속성명 추출
        component = strtok_s(NULL, ".()", &nextToken);
        if (!component)
            return false;

        // Visible 속성 처리
        if (_stricmp(component, "Visible") == 0)
        {
            // CScriptObjectManager를 통해 객체 접근
            CScriptGraphicObject* pObject = CScriptObjectManager::GetInstance()->GetObject(drawingName, objectName);
            if (!pObject)
                return false;

            // VARIANT를 c_variable로 변환
            c_variable val;
            if (value->vt == VT_BOOL)
                val = (value->boolVal != VARIANT_FALSE);
            else if (value->vt == VT_I4)
                val = (value->lVal != 0);
            else
                val = 1; // 기본값

            // 가시성 설정
            return pObject->SetVisible(val);
        }
        // 다른 속성 처리 (필요하면 추가)
        // ...
    }

    return false;
}

// 스크립트 엔진에서 호출하는 확장 함수 (4개 파라미터 버전)
void __stdcall System_SetProperty(int nargs, c_variable** pargs, c_engine* p_engine)
{
    // 인자 검증
    if (nargs != 2 || pargs[0]->vt != VT_BSTR)
        return;

    c_string path;
    pargs[0]->as_string(path);

    // VARIANT 생성
    VARIANT value;
    VariantInit(&value);

    // c_variable을 VARIANT로 변환
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

    // 실제 구현 함수 호출
    System_SetProperty(path.get_buffer(), &value);

    // VARIANT 정리
    VariantClear(&value);
}

// 인자 검증 함수
bool _check_System_SetProperty(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
    if (n != 2)
    {
        *p_msg = "SetProperty 함수는 경로와 값 2개의 인자가 필요합니다.";
        return false;
    }
    return true;
}

// 체크 함수 구현
bool _check_System_Graphic(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
    if (n != 1) {
        *p_msg = "Graphic 함수는 도면 이름 인자가 필요합니다.";
        return false;
    }
    return true;
}

bool _check_Graphic_Object(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
    if (n != 2) {
        *p_msg = "Object 함수는 그래픽 참조와 객체 이름 인자가 필요합니다.";
        return false;
    }
    return true;
}

bool _check_Object_GetVisible(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
    if (n != 1) {
        *p_msg = "GetVisible 함수는 객체 참조 인자가 필요합니다.";
        return false;
    }
    return true;
}

bool _check_Object_SetVisible(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
    if (n != 2) {
        *p_msg = "SetVisible 함수는 객체 참조와 값 인자가 필요합니다.";
        return false;
    }
    return true;
}