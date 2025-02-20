#pragma once
#include <afxwin.h>
#include "c_string.h"
#include "pch.h"
#include "basic.h"


class   CThreadWaterScript :
    public CWinThread
{
	DECLARE_DYNCREATE(CThreadWaterScript)
public:
	CThreadWaterScript();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CThreadWaterScript();


public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();

public:
	VARIANT_BOOL* m_bOk;
	void SetOwner(HWND hOwner) { 	m_hOwner = hOwner; }
	void Stop() {
		m_Basic.Stop();
		m_bThreadEnd = TRUE;
		m_bPause = false;
	}
	void SetScriptText(CString txt);
	int SetInterval(int nInterval);
	int SetRoop(bool bRoop);
	bool WorkThread();
	BOOL IsRunning();
	int CompileScript(const char* szScriptText, const char* szErroMsg, int nErrMsgSize);
	int SaveScript(const char* szScriptText);
	bool IsPause()
	{
		return m_bPause;
	}
	void Pause()
	{
		m_bPause = true;
	}
	void ReStart()
	{
		m_bPause = false;
	}
private:
	bool m_bPause;
	Basic m_Basic;
	c_string m_ScriptText;
	HWND	m_hOwner;
	BOOL	m_bThreadEnd;
	int		m_nInterval;
	bool	m_bRoop;
	bool	m_bCompile;
protected:
	DECLARE_MESSAGE_MAP()
};

