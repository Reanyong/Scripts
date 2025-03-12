#include "pch.h"
#include "ThreadWaterScript.h"

WaterScript::WaterScript()
{
	m_ThreadScript = NULL;
	m_pTagCol = NULL;
	m_hOwner = NULL;
	m_nInterval = 0;
	m_bRoop = false;
	m_strScriptText = "";

}
WaterScript::~WaterScript()
{
	ScriptEnd();
}
int WaterScript::ScriptInit(HWND hOwner, CTagCollector* pTagCol,BOOL bRoop, int nInterval)
{
	m_ThreadScript = NULL;
	m_hOwner = hOwner;
	g_pTagCol = pTagCol;
	m_bRoop = bRoop;
	m_nInterval = nInterval;
	char g_szDirPathiSchdule[512] = { 0 };
	char szBuf[512] = { 0, };
	char szProject[512] = { 0, };
	char m_szDBPath[512] = { 0, };
	char szLocalNode[512] = { 0, };
	char szSetUpPath[512] = { 0 };
	GetCurrentDirectory(sizeof(g_szDirPathiSchdule), g_szDirPathiSchdule);

	sprintf_s(szBuf, "%s\\iwater.ini", g_szDirPathiSchdule);
	GetPrivateProfileString("iwater", "Project", "", szProject, sizeof(szProject), szBuf);
	GetPrivateProfileString("iwater", "SetUp_Path", "", szSetUpPath, sizeof(szSetUpPath), szBuf);
	CStringArray ArrXmlNode;
	//C
	try
	{
		if (pTagCol != NULL)
		{
			pTagCol->GetNodeListFromXml(ArrXmlNode, szProject, szSetUpPath);
			pTagCol->GetLocalGrpNodeName(szLocalNode, sizeof(szLocalNode));
			ArrXmlNode.Add(szLocalNode);
			int nXmlNodeCount = (int)ArrXmlNode.GetCount();
			int nNodeCount = (int)pTagCol->GetNodeCnt();
			bool bSame = false;
			for (int i = 0; i < nNodeCount; i++)
			{
				char szTempNode[128] = { 0 };
				pTagCol->GetNodeName(i, szTempNode, sizeof(szTempNode));
				for (int j = 0; j < nXmlNodeCount; j++)
				{
					bSame = false;
					if (_stricmp(szTempNode, ArrXmlNode[j]) == 0)
					{
						bSame = true;
						break;
					}
					if (!bSame)
					{
						sprintf_s(g_szLocalModNode, "%s", szTempNode);
					}
				}
			}
		}
	}
	catch (...)
	{
		return 0;
	}
	if (m_ThreadScript != NULL)
		ScriptEnd();
	m_ThreadScript = (CThreadWaterScript*)AfxBeginThread(RUNTIME_CLASS(CThreadWaterScript), THREAD_PRIORITY_HIGHEST, 0, CREATE_SUSPENDED);
	m_ThreadScript->SetOwner(m_hOwner);
	m_ThreadScript->SetRoop(m_bRoop);
	m_ThreadScript->SetInterval(m_nInterval);

	return 1;
}

int WaterScript::CompileScript(const char* szScriptText, const char* szErroMsg, int nErrMsgSize)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//HINSTANCE hins = AfxGetInstanceHandle();
	//_Module = hins;

	/*if (g_pTagCol==NULL)
	{
		AfxMessageBox("TagCollector 연결에 실패했습니다.");
		return 0;
	}

	m_ThreadScript->SetCompile(true);
	m_ThreadScript->SetScriptSource(szScriptText);
	m_ThreadScript->ResumeThread();
	ScriptEnd();*/
	//return 1;
	if (m_ThreadScript!=NULL)
	{
		return m_ThreadScript->CompileScript(szScriptText, szErroMsg, nErrMsgSize);
	}
	return 0;
}



int WaterScript::SaveScript(const char* szScriptText)
{
	if (m_ThreadScript != NULL)
	{
		return m_ThreadScript->SaveScript(szScriptText);
	}
	return 0;
}

int WaterScript::RunScript()
{
	if (m_ThreadScript != NULL)
	{
		if (m_ThreadScript->IsPause())
			m_ThreadScript->ReStart();
		else
			m_ThreadScript->ResumeThread();
		return 1;
	}
	else
		return 0;
}

int WaterScript::StopScript()
{
	if (m_ThreadScript != NULL)
	{
		m_ThreadScript->Pause();
	}
	return 1;
}

int WaterScript::ScriptEnd()
{
	if (m_ThreadScript==NULL)
	{
		return 1;
	}
	if (!m_ThreadScript->IsRunning())
	{
		if (m_ThreadScript != NULL)
		{
			delete m_ThreadScript;
			m_ThreadScript = NULL;
		}
		return 1;
	}
	m_ThreadScript->Stop();
	DWORD dwExitCode = 0;
	DWORD64 dw = GetTickCount64();
	try
	{
		while (true)
		{
			if (GetExitCodeThread(m_ThreadScript->m_hThread, &dwExitCode))
			{
				if (STILL_ACTIVE == dwExitCode)
				{
					Sleep(10);
					if (GetTickCount64() > dw + 1000 * 60)
					{
						break;
					}

					continue;
				}
				else
					break;
			}
			else
				break;
		}
		if (m_ThreadScript != NULL)
		{
			delete m_ThreadScript;
			m_ThreadScript = NULL;
		}
	}
	catch (...)
	{

	}
	return 1;
}

int WaterScript::IsRunning()
{
	if (m_ThreadScript != NULL)
	{
		return m_ThreadScript->IsRunning();
	}
	return 0;
}

void WaterScript::Wait(DWORD64 dwMillisecond)
{
	MSG msg;
	DWORD64 dwStart;
	dwStart = GetTickCount64();
	while (GetTickCount64() - dwStart < dwMillisecond)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}
