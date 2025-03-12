// ScriptSystem.h
#pragma once
#include "pch.h"
#include "c_variable.h"
#include "c_engine.h"
#include <map>
#include <string>

// 각 객체 클래스 선언
class CScriptGraphicObject;
class CScriptGraphic;
class CScriptSystem;

// 객체 매니저 - 메모리 관리와 객체 참조 처리
class CScriptObjectManager
{
private:
    static CScriptObjectManager* s_instance;
    std::map<std::string, CScriptGraphic*> m_graphics;
    std::map<std::string, CScriptGraphicObject*> m_objects;

public:
    static CScriptObjectManager* GetInstance();

    CScriptGraphic* GetGraphic(const char* name);
    CScriptGraphicObject* GetObject(const char* graphicName, const char* objectName);

    void Clear();
    ~CScriptObjectManager();

    static void DestroyInstance();
};

// 그래픽 객체 (도면 내 개별 요소)
class CScriptGraphicObject
{
private:
    c_string m_graphicName;
    c_string m_objectName;

public:
    CScriptGraphicObject(const char* graphicName, const char* objectName);

    const char* GetGraphicName() { return m_graphicName.get_buffer(); }
    const char* GetObjectName() { return m_objectName.get_buffer(); }

    bool GetVisible(c_variable& result);
    bool SetVisible(c_variable& value);

    // 추가 속성
    bool GetText(c_variable& result);
    bool SetText(c_variable& value);

    bool GetForegroundColor(c_variable& result);
    bool SetForegroundColor(c_variable& value);
};

// 그래픽 (도면)
class CScriptGraphic
{
private:
    c_string m_name;

public:
    CScriptGraphic(const char* name);
    const char* GetName() { return m_name.get_buffer(); }

    CScriptGraphicObject* GetObject(const char* objectName);
};

// 시스템 객체 (최상위)
class CScriptSystem
{
public:
    static CScriptGraphic* GetGraphic(const char* graphicName);
};

// 스크립트 엔진 확장 함수
void __stdcall System_GetProperty(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall System_SetProperty(int nargs, c_variable** pargs, c_engine* p_engine);
bool _check_System_GetProperty(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_System_SetProperty(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);

// 디스패치 함수 - 객체 속성 접근
void __stdcall System_Dot(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
bool _check_System_Dot(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);

// 디스패치 함수 - Graphic 객체 접근
void __stdcall System_Graphic(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
bool _check_System_Graphic(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);

// 디스패치 함수 - Object 객체 접근
void __stdcall Graphic_Object(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
bool _check_Graphic_Object(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);

// 속성 접근 함수
void __stdcall Object_GetVisible(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall Object_SetVisible(int nargs, c_variable** pargs, c_engine* p_engine);

bool _check_Object_GetVisible(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_Object_SetVisible(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);