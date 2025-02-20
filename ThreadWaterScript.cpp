#include "pch.h"
#include "Basic.h"
#include "ThreadWaterScript.h"

IMPLEMENT_DYNCREATE(CThreadWaterScript, CWinThread)

CThreadWaterScript::CThreadWaterScript()
{
	m_bAutoDelete = FALSE;
	m_bThreadEnd = TRUE;
	m_hOwner = NULL;
	m_nInterval = 0;
	m_bRoop = false;
	m_bCompile = false;
	m_bOk = NULL;
	m_bPause = false;
}

CThreadWaterScript::~CThreadWaterScript()
{
	//m_Basic.Reset();
}


BEGIN_MESSAGE_MAP(CThreadWaterScript, CWinThread)
END_MESSAGE_MAP()


BOOL CThreadWaterScript::InitInstance()
{

	return TRUE;
}

int CThreadWaterScript::Run()
{

	int nInterval = m_nInterval < 10 ? 10 : m_nInterval;
	DWORD64 dwOld = 0;
	m_bThreadEnd = FALSE;
	do
	{
		Sleep(1);
		if (m_bPause)
			continue;
		if (GetTickCount64() - dwOld < nInterval)
			continue;
		if (WorkThread())
			dwOld = GetTickCount64();
		if (!m_bRoop)
			break;
	} while (!m_bThreadEnd);
	m_bThreadEnd = TRUE;
	PostThreadMessage(WM_QUIT, 0, 0);

	return CWinThread::Run();
}


int CThreadWaterScript::ExitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 정리를 수행합니다.
	return CWinThread::ExitInstance();
}



void CThreadWaterScript::SetScriptText(CString txt)
{
	m_ScriptText = txt;
}

int CThreadWaterScript::SetInterval(int nInterval)
{
	if (nInterval<10)
	{
		nInterval = 1000;
	}
	m_nInterval = nInterval;
	return 1;
}

int CThreadWaterScript::SetRoop(bool bRoop)
{
	m_bRoop = bRoop;
	return 1;
}

bool CThreadWaterScript::WorkThread()
{
	try
	{
		if (m_Basic.isSetSource())
		{
			if (FAILED(m_Basic.Run()))
			{
				return 0;
			}
		}


	}
	catch (...)
	{
		return 0;
	}
	return 1;
}



BOOL CThreadWaterScript::IsRunning()
{
	return !m_bThreadEnd;
}

int CThreadWaterScript::CompileScript(const char* szScriptText, const char* szErroMsg, int nErrMsgSize)
{
	int nRet = 1;
	c_string strScriptText = "";
	CString strBuf = "";
	strBuf.Format("%s", szScriptText);
	strBuf.Replace(";", "");
	strBuf.Replace("\\n", "\n");
	strScriptText = strBuf;

	try
	{
		if (strScriptText.get_length() != 0)
		{
			if (FAILED(m_Basic.SetSource(strScriptText)))
			{
				BSTR bstrMsg;
				c_string msg = "";
				CString ErrMsg = "";
				m_Basic.get_Error(&bstrMsg);
				ErrMsg = bstrMsg;
				int nStrLen = ErrMsg.GetLength();
				if (nStrLen <= nErrMsgSize)
				{
					sprintf_s((char*)szErroMsg, nErrMsgSize, "%s", (LPSTR)(LPCTSTR)ErrMsg);
				}
				else
				{
					CString temp = ErrMsg.Left(nErrMsgSize - 1);
					sprintf_s((char*)szErroMsg, nErrMsgSize, "%s", (LPSTR)(LPCTSTR)temp);
				}
				//AfxMessageBox(ErrMsg);
				nRet = 0;

			}
		}
	}
	catch (...)
	{
		nRet = 0;
	}
	return nRet;
}

int CThreadWaterScript::SaveScript(const char* szScriptText)
{
	int nRet = 1;
	c_string strScriptText = "";
	CString strBuf = "";
	strBuf.Format("%s", szScriptText);
	strBuf.Replace(";", "");
	strBuf.Replace("\\n", "\n");
	strScriptText = strBuf;

	try
	{
		if (strScriptText.get_length() != 0)
		{
			if (FAILED(m_Basic.SetSource(strScriptText)))
			{
				nRet = 0;
			}
		}
	}
	catch (...)
	{
		nRet = 0;
	}
	return nRet;
}



