#include "pch.h"
#include "other.h"
#include "com.h"
#include "c_engine.h"
#include <string>
#include <cstring>

//-----------------------------------------------------------------------------
// CreateObject ---------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall CreateObject(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	c_string classname;
	pargs[0]->as_string(classname);

	CLSID clsid = {0};
	GUID  libid = {0};

	if (!get_clsid_libid(classname.get_buffer(), &clsid, &libid, 0))
	{
		p_engine->runtime_error(CUR_ERR_LINE, "class '%s' is not registered in the system.",  classname.get_buffer());
		return;
	}

	IDispatch* pd = 0;
	HRESULT hr = co_create_instance(clsid, &pd);

	if (FAILED(hr) || !pd)
	{
		p_engine->runtime_error(CUR_ERR_LINE, "cannot create object of class '%s'.",  classname.get_buffer());
		return;
	}

	result = pd;
}

bool _check_CreateObject (int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'CreateObject' function gets 1 argument.";
		return false;
	}

	if (p_types[0] != VT_EMPTY && p_types[0] != VT_BSTR)
	{
		*p_msg = "argument of the 'CreateObject' function must be string.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// MsgBox ---------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall MsgBox(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	//_ASSERT(nargs >= 1 && nargs <= 3);
	if (!(nargs >= 1 && nargs <= 3))
	{
		result = INT_MIN;
		return;
	}

	c_string str_text;
	c_string str_title="";

	str_title = "";
	int n_flags = MB_OK;
	switch (pargs[0]->vt)
	{
	case VT_EMPTY:
	case VT_NULL:
	case VT_CY:
	case VT_DISPATCH:
	case VT_ERROR:
	case VT_VARIANT:
	case VT_UNKNOWN:
	case VT_VOID:
	case VT_HRESULT:
	case VT_PTR:
	case VT_SAFEARRAY:
	case VT_CARRAY:
	case VT_USERDEFINED:
	case VT_RECORD:
	case VT_INT_PTR:
	case VT_UINT_PTR:
	case VT_FILETIME:
	case VT_BLOB:
	case VT_STREAM:
	case VT_STORAGE:
	case VT_STREAMED_OBJECT:
	case VT_STORED_OBJECT:
	case VT_BLOB_OBJECT:
	case VT_CF:
	case VT_CLSID:
	case VT_VERSIONED_STREAM:
	case VT_BSTR_BLOB:
	case VT_VECTOR:
	case VT_ARRAY:
	case VT_BYREF:
	case VT_RESERVED:
	case VT_ILLEGAL:
		result = INT_MIN;
		return;
	default:
		break;
	}
	pargs[0]->as_string(str_text);
	if (nargs > 1)
	{
		switch (pargs[1]->vt)
		{
		case VT_EMPTY:
		case VT_NULL:
		case VT_CY:
		case VT_DATE:
		case VT_BSTR:
		case VT_DISPATCH:
		case VT_ERROR:
		case VT_VARIANT:
		case VT_UNKNOWN:
		case VT_DECIMAL:
		case VT_VOID:
		case VT_HRESULT:
		case VT_PTR:
		case VT_SAFEARRAY:
		case VT_CARRAY:
		case VT_USERDEFINED:
		case VT_LPSTR:
		case VT_LPWSTR:
		case VT_RECORD:
		case VT_INT_PTR:
		case VT_UINT_PTR:
		case VT_FILETIME:
		case VT_BLOB:
		case VT_STREAM:
		case VT_STORAGE:
		case VT_STREAMED_OBJECT:
		case VT_STORED_OBJECT:
		case VT_BLOB_OBJECT:
		case VT_CF:
		case VT_CLSID:
		case VT_VERSIONED_STREAM:
		case VT_BSTR_BLOB:
		case VT_VECTOR:
		case VT_ARRAY:
		case VT_BYREF:
		case VT_RESERVED:
		case VT_ILLEGAL:
			result = INT_MIN;
			return;
		default:
			break;
		}
		n_flags = pargs[1]->as_integer();
	}
	if (nargs > 2)
	{
		if (pargs[2]->vt != VT_BSTR)
		{
			result = INT_MIN;
			return;
		}
		pargs[2]->as_string(str_title);
	}
	if (str_title == "")
		str_title = "MsgBox";
	CString strMsg = str_text;
	//CString strTest = "aaaa\nssss";
	result = MessageBox(0, strMsg, str_title.get_buffer(), n_flags | MB_SYSTEMMODAL);
}

bool _check_MsgBox(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n < 1 || n > 3)
	{
		*p_msg = "'MsgBox' function gets 1 to 3 arguments.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// DbgPrint -------------------------------------------------------------------
//-----------------------------------------------------------------------------
void __stdcall DbgPrint(int nargs, c_variable** pargs, c_engine* p_engine)
{
	c_string line;
	c_string str;
	int i;
	for (i = 0; i < nargs; i++)
	{
		pargs[i]->as_string(str);
		line += str;
	}

	p_engine->dbg_print(line.get_buffer());
}


bool _check_DbgPrint(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return true;
}

//20210402 YYYY-MM-DD HH:MM:SS.MIL   DbgPrintMil 을 찍어보자
void __stdcall DbgPrintMil(int nargs, c_variable** pargs, c_engine* p_engine)
{
	c_string line;
	c_string str;
	int i;
	for (i = 0; i < nargs; i++)
	{

	 	time_t tCur;
	 	time(&tCur);
	 	struct tm tmC; localtime_s(&tmC, &tCur);



		c_string curtime = "YYYY-MM-DD HH:MM:SS.MIL  ";
		curtime.format("%d-%d-%d %d:%d:%d   > ",
			tmC.tm_year, tmC.tm_mday, tmC.tm_hour, tmC.tm_hour, tmC.tm_min, tmC.tm_sec);

		pargs[i]->as_string(str);
		line += curtime ;
		line +=  str;
	}

	p_engine->dbg_print(line.get_buffer());
}

bool _check_DbgPrintMil(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return true;
}

//-----------------------------------------------------------------------------
// DbgLog ---------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall DbgLog(int nargs, c_variable** pargs, c_engine* p_engine)
{
	_ASSERT(nargs == 2);

	c_string fname;
	c_string str;

	pargs[0]->as_string(fname);
	pargs[1]->as_string(str);

	FILE* f = fopen(fname.get_buffer(), "a");
	if (!f) return;
	fprintf(f, "%s\n", str.get_buffer());
	fflush(f);
	fclose(f);
}

bool _check_DbgLog(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'DbgLog' function gets 2 arguments.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// DbgBreak -------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall DbgBreak(int nargs, c_variable** pargs, c_engine* p_engine)
{
	_ASSERT(nargs == 0);
	p_engine->dbg_break();
}

bool _check_DbgBreak(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 0)
	{
		*p_msg = "'DbgBreak' function gets 0 arguments.";
		return false;
	}

	return true;
}


//-----------------------------------------------------------------------------
// iif ------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//result = IIF(condition, TrueArg, FalseArg)

void __stdcall iif(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 3);

	int n_cond = pargs[0]->as_integer();

	if (n_cond) result = *pargs[1];
	else result = *pargs[2];
}

bool _check_iif(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 3)
	{
		*p_msg = "'IIF' function gets 3 arguments.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// eval -----------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall eval(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);

	c_string str;
	pargs[0]->as_string(str);
	p_engine->eval_watch(str.get_buffer(), &result);
}

bool _check_eval(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'Eval' function gets 1 argument.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Stop -----------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall Stop(int nargs, c_variable** pargs, c_engine* p_engine)
{
	_ASSERT(nargs == 0);
	p_engine->stop();
}

bool _check_Stop(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 0)
	{
		*p_msg = "'Stop' function gets no arguments.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Assert ---------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall Assert(int nargs, c_variable** pargs, c_engine* p_engine)
{
	_ASSERT(nargs == 1);

	if (pargs[0]->as_integer() == 0)
		p_engine->runtime_error(CUR_ERR_LINE, "Assertion failed");
}

bool _check_Assert(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'Assert' function gets 1 argument.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// GetTagValue --------------------------------------------------------------
//-----------------------------------------------------------------------------
void __stdcall GetTagValue(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs >= 1 && nargs <= 3);

	c_string str_TagName;
	c_string str_title;
	str_title = "";
	int n_flags = 0;
	//double a;
	pargs[0]->as_string(str_TagName);
	if (!KWGetTagValue(str_TagName.get_buffer(), result))
	{
		result = INT_MIN;
	}
	//if (nargs > 1) n_flags = pargs[1]->as_integer();
	//if (nargs > 2) pargs[2]->as_string(str_title);

	//result = a;
}

bool _check_GetTagValue(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'GetTagValue' function gets 1 arguments.";
		return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
// SetTagValue --------------------------------------------------------------
//-----------------------------------------------------------------------------
void __stdcall SetTagValue(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs >= 1 && nargs <= 3);

	c_string str_TagName = "";
	double TagValue = 0;
	int n_flags = 0;

	pargs[0]->as_string(str_TagName);
	TagValue = pargs[1]->as_double();
	//if (nargs > 1) n_flags = pargs[1]->as_integer();
	//if (nargs > 2) pargs[2]->as_string(str_title);
	result = 1;
	if (!KWPutTagValue(str_TagName.get_buffer(), TagValue))
		result = INT_MIN;
	//result = 2;
}

bool _check_SetTagValue(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'SetAiTagValue' function gets 2 arguments.";
		return false;
	}

	return true;
}

void __stdcall Sleep(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 1)
	{
		result = INT_MIN;
		return;
	}
	switch (pargs[0]->vt)
	{
	case VT_EMPTY:
	case VT_NULL:
	case VT_CY:
	case VT_DATE:
	case VT_DISPATCH:
	case VT_ERROR:
	case VT_VARIANT:
	case VT_UNKNOWN:
	case VT_VOID:
	case VT_HRESULT:
	case VT_PTR:
	case VT_SAFEARRAY:
	case VT_CARRAY:
	case VT_USERDEFINED:
	case VT_RECORD:
	case VT_INT_PTR:
	case VT_UINT_PTR:
	case VT_FILETIME:
	case VT_BLOB:
	case VT_STREAM:
	case VT_STORAGE:
	case VT_STREAMED_OBJECT:
	case VT_STORED_OBJECT:
	case VT_BLOB_OBJECT:
	case VT_CF:
	case VT_CLSID:
	case VT_VERSIONED_STREAM:
	case VT_BSTR_BLOB:
	case VT_VECTOR:
	case VT_ARRAY:
	case VT_BYREF:
	case VT_RESERVED:
	case VT_ILLEGAL:
		result = INT_MIN;
		return;
	default:
		break;
	}
	int nVal = 0;
	nVal = pargs[0]->as_integer();
	Sleep(nVal);
	result = 1;
}

bool _check_Sleep(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'Sleep' function gets 1 arguments.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// ToggleDigitalPoint --------------------------------------------------------------
//-----------------------------------------------------------------------------
void __stdcall ToggleDigitalPoint(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 1 || pargs[0]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}
	if (g_pTagCol == NULL)
	{
		result = INT_MIN;
		return;
	}

	c_string str_TagName = "";
	double TagValue = 0;
	int n_flags = 0;
	CString s = str_TagName;
	time_t t=0;
	pargs[0]->as_string(str_TagName);
	result = g_pTagCol->GetTagValue((char*)str_TagName.get_buffer(), TagValue, &t);
	if (result.as_integer() < 0)
	{
		result = INT_MIN;
		return;
	}

	if( TagValue == 1)
		g_pTagCol->PutTagValue((char*)str_TagName.get_buffer(), 0);
	else
		g_pTagCol->PutTagValue((char*)str_TagName.get_buffer(), 1);
	result = 1;
}

void __stdcall AcknowledgeAllAlarms(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	c_string str_GrpName = "";

	if (nargs != 0)
	{
		result = INT_MIN;
		return;
	}
	if (g_pTagCol == NULL)
	{
		result = INT_MIN;
		return;
	}
	CArray<ALARM_PROC, ALARM_PROC> arr;
	CArray<ALARM_PROC, ALARM_PROC> RemoteAckArr;
	if (g_pTagCol == NULL)
	{
		result = INT_MIN;
		return;
	}
	if (g_pTagCol->GetCurAlarms(arr) != 1)
	{
		result = INT_MIN;
		return;
	}
	g_pTagCol->SetAlarmAck(arr.GetData(), (int)arr.GetCount());
	result = 1;
}

void __stdcall AcknowledgeAnAlarm(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	c_string str_TagName = "";
	//int flag = 0;
	try
	{
		if (nargs != 1 || pargs[0]->vt != VT_BSTR)
		{
			result = INT_MIN;
			return;
		}

		pargs[0]->as_string(str_TagName);
		//flag = pargs[1]->as_integer();
		CArray<ALARM_PROC, ALARM_PROC> arr;
		CArray<ALARM_PROC, ALARM_PROC> RemoteAckArr;
		if (g_pTagCol == NULL)
		{
			result = INT_MIN;
			return;
		}
		if (g_pTagCol->GetCurAlarms(arr) != 1)
		{
			result = INT_MIN;
			return;
		}
		CString LocalNodeName = g_pTagCol->GetLocalGrpNodeName();
		char TagNodeName[64] = { 0 };
		if (g_pTagCol->GetTagNodeName((char*)str_TagName.get_buffer(), TagNodeName) != 1)
		{
			result = INT_MIN;
			return;
		}
		int nAlmCount = (int)arr.GetCount();
		LPTAG_ANALOG pAi = NULL;
		LPTAG_ANALOG_OUTPUT pAo = NULL;
		LPTAG_DIGITAL pDi = NULL;
		LPTAG_DIGITAL_OUTPUT pDo = NULL;
		for (int i = 0; i < nAlmCount; ++i)
		{
			if (g_pTagCol == NULL)
			{
				result = INT_MIN;
				return;
			}
			if (_stricmp(arr[i].szName, str_TagName.get_buffer()) == 0)
			{
				g_pTagCol->SetAlarmAck(&arr[i], 1);
				break;
			}
		}
		result = 1;
	}
	catch (...)
	{
		result = INT_MIN;
	}


}

void __stdcall DisableAlarm(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 1 || pargs[0]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}
	c_string str_TagName = "";

	pargs[0]->as_string(str_TagName);
	result = 1;
	if (!KWSetTagField(str_TagName.get_buffer(), TAG_FIELD::ALARM, 0))
		result = INT_MIN;

}

void __stdcall EnableAlarm(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 1 || pargs[0]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}
	c_string str_TagName = "";

	pargs[0]->as_string(str_TagName);
	result = 1;
	if (!KWSetTagField(str_TagName.get_buffer(), TAG_FIELD::ALARM, 1))
		result = INT_MIN;

}

void __stdcall CloseDigitalPoint(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 1 || pargs[0]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	c_string str_TagName = "";
	double TagValue = 0;
	int n_flags = 0;

	pargs[0]->as_string(str_TagName);
	result = 1;
	if (!KWPutTagValue(str_TagName,1))
		result = INT_MIN;

}

void __stdcall OpenDigitalPoint(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 1 || pargs[0]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}
	c_string str_TagName = "";
	double TagValue = 0;
	int n_flags = 0;

	pargs[0]->as_string(str_TagName);
	result = 1;
	if (!KWPutTagValue(str_TagName, 0))
		result = INT_MIN;
}

/*
	ClosePicture("석성생활(정)_활성탄")
*/
void __stdcall ClosePicture(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 1 || pargs[0]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_CLOSEPAGE;


	c_string a, b, c;
	pargs[0]->as_string(a);
	//pargs[1]->as_string(b);
	//pargs[2]->as_string(c);
	strcpy_s(global.szParms1, a);
	//strcpy_s(global.szParms2, b);
	//strcpy_s(global.szParms3, c);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;
	result = 1;
	if (!KWSendMsgToViewer(cds))
		result = INT_MIN;

}

/*
	OpenPicture("SAMPLE")
*/
void __stdcall OpenPicture(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 1 || pargs[0]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}
	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_POPUPPAGE;


	c_string a, b, c;
	pargs[0]->as_string(a);
	//pargs[1]->as_string(b);
	//pargs[2]->as_string(c);
	strcpy_s(global.szParms1, a);
	//strcpy_s(global.szParms2, b);
	//strcpy_s(global.szParms3, c);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;
	result = 1;
	if (!KWSendMsgToViewer(cds))
		result = INT_MIN;

}

void __stdcall ReplacePicture(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 2 || pargs[0]->vt != VT_BSTR || pargs[1]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	c_string str_pName = "";
	c_string str_pNameNew = "";
	//c_string str_tName = "";
	pargs[0]->as_string(str_pName);
	pargs[1]->as_string(str_pNameNew);

	pargs[0]->as_string(str_pName);

	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_REPLACEPAGE;


	c_string a, b, c;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	//pargs[2]->as_string(c);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);
	//strcpy_s(global.szParms3, c);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;
	result = 1;
	if (!KWSendMsgToViewer(cds))
		result = INT_MIN;

}

//표준수운형 엔진 소프트웨어국산화
// Script
//Tag Scan 관련 함수 추가
//2023-01-26 phk

void __stdcall OffScan(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 1 || pargs[0]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}
	c_string str_TagName = "";

	pargs[0]->as_string(str_TagName);

	result = 1;
	if (!KWSetTagField(str_TagName, TAG_FIELD::SCAN, 0))
		result = INT_MIN;

}

void __stdcall OnScan(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 1 || pargs[0]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}
	c_string str_TagName = "";

	pargs[0]->as_string(str_TagName);
	result = 1;
	if (!KWSetTagField(str_TagName, TAG_FIELD::SCAN, 1))
		result = INT_MIN;
}

void __stdcall ToggleScan(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 1 || pargs[0]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	c_string str_TagName = "";
	pargs[0]->as_string(str_TagName);
	c_variable FieldValue=0;
	if (!KWGetTagField(str_TagName, TAG_FIELD::SCAN, FieldValue))
	{
		result = INT_MIN;
		return;
	}
	int nScan = FieldValue.as_integer();
	if (nScan == INT_MIN)
	{
		result = INT_MIN;
		return;
	}
	result = 1;
	if (nScan == 1)
	{
		if(!KWSetTagField(str_TagName, TAG_FIELD::SCAN, 1))
			result = INT_MIN;
	}
	else
	{
		if (!KWSetTagField(str_TagName, TAG_FIELD::SCAN, 0))
			result = INT_MIN;
	}
}

void __stdcall ReadValue(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 1 || pargs[0]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	c_string str_TagName;
	c_string str_title;
	str_title = "";
	int n_flags = 0;
	//double a;
	pargs[0]->as_string(str_TagName);
	if (g_pTagCol == NULL)
	{
		result = INT_MIN;
		return;
	}
	double dValue = INT_MIN;
	time_t tLast = 0;
	int nErrCode = 1;
	TAGTYPEID tagtype = g_pTagCol->GetTagType((char*)str_TagName.get_buffer(), &nErrCode);

	if (tagtype == TAGTYPEID::ENUMEND || nErrCode < 0)
	{
		result = INT_MIN;
		return;
	}
	char szNode[64] = { 0 };
	char szValue[TEXTDATALEN] = { 0 };
	LPOLESTR temp = NULL;
	if (g_pTagCol->GetTagNodeName((char*)str_TagName.get_buffer(), szNode) != 1)
	{
		result = INT_MIN;
		return;
	}
	switch (tagtype)
	{
	case TAGTYPEID::ANALOGINPUT:
	case TAGTYPEID::ANALOGOUTPUT:
	case TAGTYPEID::DIGITALINPUT:
	case TAGTYPEID::DIGITALOUTPUT:
		if (g_pTagCol->GetTagValue((char*)str_TagName.get_buffer(), dValue, &tLast) != 1)
		{
			result = INT_MIN;
			return;
		}
		else
		{
			result = dValue;
		}
		break;
	case TAGTYPEID::TEXT:
		if (g_pTagCol->GetStrTagValue((char*)str_TagName.get_buffer(), szValue, &tLast) != 1)
		{
			result = INT_MIN;
			return;
		}
		else
		{
			CString strValue = "";
			strValue.Format("%s", szValue);
			result.vt = VT_BSTR;
			result.bstrVal = strValue.AllocSysString();
		}
		break;
	}
}

void __stdcall WriteValue(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	try
	{
		if (nargs != 2 || pargs[0]->vt != VT_BSTR)
		{
			result = INT_MIN;
			return;
		}
		bool bString = false;
		double TagValue = 0;
		c_string strTagValue = "";
		switch (pargs[1]->vt)
		{
		case VT_BSTR:
			pargs[1]->as_string(strTagValue);
			bString = true;
			break;
		case VT_I2:
		case VT_I4:
		case VT_INT:
		case VT_UINT:
		case VT_BOOL:
		case VT_R4:
		case VT_DATE:
		case VT_R8:
			TagValue = pargs[1]->as_double();
			break;
		case VT_VARARRAY:
		{
			c_variable* p_element = 0;
			get_element(pargs[1]->parray, 0, &p_element);
			if (p_element) TagValue= p_element->as_double();
		}
		break;
		default:
			result = INT_MIN;
			return;
		}


		c_string str_TagName = "";


		pargs[0]->as_string(str_TagName);
		TagValue = pargs[1]->as_double();
		if (g_pTagCol == NULL)
		{
			result = INT_MIN;
			return;
		}
		int nErrCode = 1;
		TAGTYPEID tagtype = g_pTagCol->GetTagType((char*)str_TagName.get_buffer(), &nErrCode);

		if (tagtype == TAGTYPEID::ENUMEND || nErrCode < 0)
		{
			result = INT_MIN;
			return;
		}
		char szNode[64] = { 0 };
		if (g_pTagCol->GetTagNodeName((char*)str_TagName.get_buffer(), szNode) != 1)
		{
			result = INT_MIN;
			return;
		}
		switch (tagtype)
		{
		case TAGTYPEID::ANALOGINPUT:
		case TAGTYPEID::DIGITALINPUT:
			if (bString)
			{
				result = INT_MIN;
				return;
			}
			else
			{
				if (g_pTagCol->PutTagValue(szNode, (char*)str_TagName.get_buffer(), TagValue)!=1)
				{
					result = INT_MIN;
					return;
				}
				result = 1;
			}

			break;
		case TAGTYPEID::ANALOGOUTPUT:
		case TAGTYPEID::DIGITALOUTPUT:
			if (bString)
			{
				result = INT_MIN;
				return;
			}
			else
			{
				if(g_pTagCol->PutTagOutput(szNode, (char*)str_TagName.get_buffer(), TagValue)!=1)
				{
					result = INT_MIN;
					return;
				}
				result = 1;
			}
			break;
		case TAGTYPEID::TEXT:
			if (!bString)
			{
				result = INT_MIN;
				return;
			}
			else
			{
				if(g_pTagCol->PutStrTagValue(szNode, (char*)str_TagName.get_buffer(), (char*)strTagValue.get_buffer(), strTagValue.get_length())!=1)
				{
					result = INT_MIN;
					return;
				}
				result = 1;
			}

			break;
		default:
			result = INT_MIN;
			return;
		}
	}
	catch (...)
	{
		result = INT_MIN;
		return;
	}
}
void __stdcall RampValue(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 2 || pargs[0]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}
	int nRamp = pargs[1]->as_integer();
	if (nRamp == INT_MIN)
	{
		result = INT_MIN;
		return;
	}
	c_string str_TagName = "";
	c_variable TagValue = double(0);
	pargs[0]->as_string(str_TagName);
	if (!KWGetTagValue(str_TagName.get_buffer(), TagValue)) {
		result = INT_MIN;
		return;
	}
	double temp = TagValue.as_double();
	if(temp==INT_MIN)
	{
		result = INT_MIN;
		return;
	}
	if (nRamp<0)
	{
		temp -= 1;
	}
	else
	{
		temp += 1;
	}
	TagValue = temp;
	if(!KWPutTagValue(str_TagName.get_buffer(), TagValue))
	{
		result = INT_MIN;
		return;
	}
	result = 1;
}
/*void __stdcall LocateObject(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs >= 1 && nargs <= 3);
}*/
void __stdcall Login(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if(nargs!=0)
	{
		result = INT_MIN;
		return;
	}
	//char str_ProgramName[256];
	c_string str_Name = "";
	if (g_pTagCol == NULL)
	{
		result = INT_MIN;
		return;
	}
	result = 1;
	if (g_pTagCol->GetIsLogin())
	{
		g_pTagCol->LogOut();
	}
	else
	{
		g_pTagCol->Login();
	}
}
void __stdcall PictureAlias(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs >= 1 && nargs <= 3);

}


void __stdcall Execute(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs >= 1 && nargs <= 3);

	char szBuf[256] = { 0, };

	c_string str_pName = "";
	//CString str_folderPath = "";

	pargs[0]->as_string(str_pName);

	//GetModuleFileName(NULL, szBuf, 256);
	//str_folderPath = szBuf;
	//str_folderPath = str_folderPath.Left(str_folderPath.ReverseFind('\\'));
	//str_folderPath = str_folderPath + "\\" + str_pName;

	//ShellExecute(NULL, "open", str_folderPath, NULL, NULL, SW_SHOW);
	ShellExecute(NULL, "open", str_pName, NULL, NULL, SW_SHOW);
}

//표준수운형 엔진 소프트웨어국산화
//Script
//엔진관련 Interface 추가
//2023-03-08 phk
void __stdcall HmiDuplexTransfer(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs >= 1 && nargs <= 3);

	int ModeType = 0;
	ModeType = pargs[0]->as_integer();

	c_string msg;
	msg.format("ModeType = %d", ModeType);
	MessageBox(0, "HmiDuplexTransfer", msg, MB_ICONINFORMATION | MB_OK);
}

void __stdcall HmiNetworkTransfer(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs >= 1 && nargs <= 3);

	//char szBuf[256] = { 0, };

	c_string str_NodeName = "";
	//CString str_folderPath = "";

	int HMIType = 0;
	int NetworkType = 0;

	HMIType = pargs[0]->as_integer();
	NetworkType = pargs[1]->as_integer();
	pargs[2]->as_string(str_NodeName);

	c_string msg;
	msg.format("HMIType = %d, NetworkType = %d, node:%s", HMIType, NetworkType, str_NodeName.get_buffer());
	MessageBox(0, "HmiNetworkTransfer", msg, MB_ICONINFORMATION | MB_OK);
}

void __stdcall sprintf(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs >= 1);
	try
	{
		c_string buffer = "";
		c_string FormatString = "";
		pargs[0]->as_string(FormatString);
		CString strOrg = FormatString;
		CString strTemp = "";
		if (nargs > 1)
		{
			int nFormatStrSize = strOrg.GetLength();
			if (nFormatStrSize == 0)
				return;
			CString strReturn;
			int nArgPos = 1;

			for (int nPos = 0; nPos < nFormatStrSize; ++nPos)
			{
				if (FormatString[nPos] != '%')
				{
					strReturn += FormatString[nPos];
					continue;
				}

				CString strFormat;
				int nFormatEnd = -1;

				// Find the end position of the format specifier
				for (int i = nPos + 1; i < nFormatStrSize; ++i)
				{
					if (FormatString[i] == 'd' || FormatString[i] == 'f' || FormatString[i] == 's')
					{
						nFormatEnd = i;
						break;
					}
				}

				if (nFormatEnd == -1)
					break;

				strFormat = strOrg.Mid(nPos, nFormatEnd - nPos + 1);
				nPos = nFormatEnd;

				if (nArgPos >= nargs)
					break;

				CString strBuffer;
				switch (pargs[nArgPos]->vt)
				{
				case VT_BSTR:
					pargs[nArgPos]->as_string(buffer);
					strTemp = buffer;
					strBuffer.Format(strFormat, (LPSTR)(LPCTSTR)strTemp);
					break;
				case VT_I4:
					strBuffer.Format(strFormat, pargs[nArgPos]->as_integer());
					break;
				case VT_R4:
				case VT_R8:
					strBuffer.Format(strFormat, pargs[nArgPos]->as_double());
					break;
				default:
					break;
				}

				++nArgPos;
				strReturn += strBuffer;
			}



			result = strReturn;

		}
		else
		{
			result = FormatString;
		}
	}
	catch (...)
	{

	}


}
void __stdcall ChgEngActiveStandby(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);
	try
	{
		if (pargs[0]->vt != VT_BSTR)
			return;
		if (g_pTagCol == NULL)
			return;
		c_string temp = "";
		pargs[0]->as_string(temp);
		CString strTemp = temp;
		char szNode[64] = { 0 };
		sprintf_s(szNode, "%s", (LPSTR)(LPCTSTR)strTemp);
		int nRet=g_pTagCol->ChgEngActiveStandby(szNode);
		result = nRet;
	}
	catch (...)
	{

	}

}

/*
	SetObjectVisible("하단메뉴_Script", "이미지_활성탄", 1)
*/
void __stdcall SetObjectVisible(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	//1 : 도면명
	//2 : 객체명
	//3 : 보이기 설정 1 보이기 0 안보이기
	if (nargs != 3 ||
		pargs[0]->vt != VT_BSTR ||
		pargs[1]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}
	if (!(pargs[2]->as_integer()==1 || pargs[2]->as_integer() == 0))
	{
		result = INT_MIN;
		return;
	}
	if (g_pTagCol == NULL)
		return;
	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_OBJVISBLE;


	c_string a, b, c;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	pargs[2]->as_string(c);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);
	strcpy_s(global.szParms3, c);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;
	result = 1;
	if (!KWSendMsgToViewer(cds))
		result = INT_MIN;
}

/*
	Dim nRet as Integer
	nRet = SetObjectText("SCRIPTTEST", "텍스트", "1번입니다.")
	Msgbox nRet
*/
void __stdcall SetObjectText(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	//1 : 도면명
	//2 : 객체명
	//3 : 텍스트(라벨)
	if (nargs != 3 ||
		pargs[0]->vt != VT_BSTR ||
		pargs[1]->vt != VT_BSTR ||
		pargs[2]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
		return;
	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_OBJTEXT;


	c_string a, b, c;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	pargs[2]->as_string(c);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);
	strcpy_s(global.szParms3, c);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	result = 1;
	if (!KWSendMsgToViewer(cds))
		result = INT_MIN;
}

void __stdcall SetNodePrimaryType(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 2);
	if (g_pTagCol == NULL)
		return;
	c_string strNode;
	pargs[0]->as_string(strNode);
	int nType = pargs[1]->as_integer();
	char szNode[64] = { 0 };
	sprintf_s(szNode, "%s", strNode.get_buffer());
	result=g_pTagCol->SetNodePrimaryType(szNode, (PRIMARY_TYPE)nType);
}

bool _check_ToggleDigitalPoint(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'ToggleDigitalPoint' function gets 1 arguments.";
		return false;
	}

	return true;
}

bool _check_AcknowledgeAllAlarms(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 0)
	{
		*p_msg = "'AcknowledgeAllAlarms' function gets 0 arguments.";
		return false;
	}

	return true;
}

bool _check_AcknowledgeAnAlarm(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'AcknowledgeAnAlarm' function gets 1 arguments.";
		return false;
	}

	return true;
}

bool _check_DisableAlarm(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'DisableAlarm' function gets 1 arguments.";
		return false;
	}

	return true;
}

bool _check_EnableAlarm(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'EnableAlarm' function gets 1 arguments.";
		return false;
	}

	return true;
}

bool _check_CloseDigitalPoint(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'CloseDigitalPoint' function gets 1 arguments.";
		return false;
	}

	return true;
}

bool _check_OpenDigitalPoint(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'OpenDigitalPoint' function gets 1 arguments.";
		return false;
	}

	return true;
}

bool _check_ClosePicture(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'ClosePicture' function gets 1 arguments.";
		return false;
	}

	return true;
}

bool _check_OpenPicture(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'OpenPicture' function gets 1 arguments.";
		return false;
	}

	return true;
}

bool _check_ReplacePicture(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'ReplacePicture' function gets 2 arguments.";
		return false;
	}

	return true;
}

bool _check_OffScan(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'OffScan' function gets 1 arguments.";
		return false;
	}

	return true;
}

bool _check_OnScan(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'OnScan' function gets 1 arguments.";
		return false;
	}

	return true;
}

bool _check_ToggleScan(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'ToggleScan' function gets 1 arguments.";
		return false;
	}

	return true;
}

bool _check_ReadValue(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'ReadValue' function gets 1 arguments.";
		return false;
	}

	return true;
}

bool _check_WriteValue(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'WriteValue' function gets 2 arguments.";
		return false;
	}
	//p_engine->reset();
	return true;
}

bool _check_RampValue(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'RampValue' function gets 2 arguments.";
		return false;
	}
	return true;
}

/*bool _check_LocateObject(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'LocateObject' function gets 1 arguments.";
		return false;
	}

	return true;
}*/

bool _check_Login(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 0)
	{
		*p_msg = "'Login' function gets 0 arguments.";
		return false;
	}

	return true;
}
/*bool _check_PictureAlias(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'PictureAlias' function gets 1 arguments.";
		return false;
	}

	return true;
}*/


bool _check_Execute(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine) {
	if (n != 1)
	{
		*p_msg = "'Execute' function gets 1 arguments.";
		return false;
	}

	return true;
}

bool _check_HmiDuplexTransfer(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine) {
	if (n != 1)
	{
		*p_msg = "'HmiDuplexTransfer' function gets 1 arguments.";
		return false;
	}

	return true;
}


bool _check_HmiNetworkTransfer(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine) {
	if (n != 3)
	{
		*p_msg = "'HmiNetworkTransfer' function gets 3 arguments.";
		return false;
	}

	return true;
}

bool _check_sprintf(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n>0)
	{
		if (p_types[0] != VARENUM::VT_BSTR)
		{
			*p_msg = "The first parameter of sprintf function is not a string.";
			return false;
		}
		for (int i = 1; i < n; i++)
		{
			if (p_types[i] == VARENUM::VT_EMPTY)
			{
				//The first parameter of sprintf is not a string.
				*p_msg = "The parameter of the sprintf function has an invalid type.";
				return false;
			}
		}
		return true;
	}
	*p_msg = "There are no parameters for the sprintf function.";
	return false;
}

bool _check_ChgEngActiveStandby(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n!=1)
	{
		*p_msg = "ChgEngActiveStandby function requires one argument.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of ChgEngActiveStandby function is not a string.";
		return false;
	}
	return true;
}

bool _check_SetObjectVisible(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 3)
	{
		*p_msg = "'SetObjectVisible' function gets 3 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of SetObjectVisible function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of SetObjectVisible function is not a string.";
		return false;
	}
	if (p_types[2] != VARENUM::VT_I4)
	{
		*p_msg = "The third parameter of SetObjectVisible function is not a integer.";
		return false;
	}
	return true;
}

bool _check_SetObjectText(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 3)
	{
		*p_msg = "'SetObjectText' function gets 3 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of SetObjectText function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of SetObjectText function is not a string.";
		return false;
	}
	if (p_types[2] != VARENUM::VT_BSTR)
	{
		*p_msg = "The third parameter of SetObjectText function is not a string.";
		return false;
	}
	return true;
}

bool _check_SetNodePrimaryType(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'SetNodePrimaryType' function gets 2 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of SetNodePrimaryType function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_I4)
	{
		*p_msg = "The second parameter of SetNodePrimaryType function is not a integer.";
		return false;
	}
	return true;
}

//#define SendMsgToTester
#define BUFFER_SIZE 1024 //1k

bool KWSendMsgToTester(COPYDATASTRUCT cds)
{
	HWND hViewer = NULL;
	try
	{
		if ((hViewer = ::FindWindow(NULL, "Script Tester")))
		{
			if (hViewer != NULL)
			{
				::SendMessageTimeout(hViewer, WM_COPYDATA, NULL, (LPARAM)&cds, SMTO_NORMAL, 1000, NULL);
				return true;
			}
		}
	}
	catch (...)
	{

	}
	return false;
}

int GetReturnVal(int nTimeout, ST_GLOBAL global, char* pszBuffer)
{
	GUID guid;
	CoCreateGuid(&guid);

	sprintf(global.szParms7, "\\\\.\\mailslot\\%x%x%x%x%x%x%x%x%x%x%x", guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

	HANDLE hMailslot = CreateMailslot(global.szParms7,					// mailslot name
		BUFFER_SIZE,				// input buffer size
		MAILSLOT_WAIT_FOREVER,		// no timeout
		NULL);						// default security attribute

	if (hMailslot == INVALID_HANDLE_VALUE)
	{
	err_exit:

		// Close any mailslot we opened
		if (hMailslot != INVALID_HANDLE_VALUE)
			CloseHandle(hMailslot);

		return -1;
	}

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;
#ifdef SendMsgToTester
	if (KWSendMsgToTester(cds))
	{
	}
#else
	if (KWSendMsgToViewer(cds))
	{
	}
#endif
	else
	{
		if (hMailslot != INVALID_HANDLE_VALUE)
			CloseHandle(hMailslot);
		return -2;
	}

	BOOL bReadSucc = FALSE;
	ULONGLONG ull = GetTickCount64();

	try
	{
		for (;;)
		{
			DWORD	msgSize;
			BOOL	err;

			// Get the size of the next record
			err = GetMailslotInfo(hMailslot, 0, &msgSize, 0, 0);

			// Check for an error
			if (!err)
			{
				// "GetMailslotInfo failed: %s";
				goto err_exit;
			}

			// Check if there was a record. If so, the size is not -1
			if (msgSize != (DWORD)MAILSLOT_NO_MESSAGE)
			{
				void* buffer;

				// Allocate memory to read in the record
				buffer = GlobalAlloc(GMEM_FIXED, msgSize);
				if (!buffer)
				{
					//TRACE("An error getting a memory block!");
				}
				else
				{
					DWORD	numRead;

					// Read the record
					err = ReadFile(hMailslot, buffer, msgSize, &numRead, 0);

					// See if an error
					if (!err)
					{
						TRACE("ReadFile error: %d", GetLastError());
					}
					// Make sure all the bytes were read
					else if (msgSize != numRead)
					{
						//TRACE("ReadFile did not read the correct number of bytes!");
					}
					else
					{
						bReadSucc = TRUE;
						sprintf(pszBuffer, (char*)buffer);
						pszBuffer[msgSize] = 0;
					}

					// Free the buffer
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
		ASSERT(FALSE);
	}

	CloseHandle(hMailslot);

	if (bReadSucc == TRUE)
		return 1;			// Success

	return -1;			// Fail
}

bool _check_GetHmiRedundancyStatusCur(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'GetHmiRedundancyStatusCur' function gets 1 arguments.";
		return false;
	}

	return true;
}

/*
dim nRet as integer
nRet = GetHmiRedundancyStatusCur(NodeName)
*/
/*
0 : Active
1 : Standby
2 : Client
그외 확인 필요
*/
void __stdcall GetHmiRedundancyStatusCur(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 1 || pargs[0]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	c_string cstrName;
	pargs[0]->as_string(cstrName);

	if (g_pTagCol == NULL)
	{
		result = INT_MIN;
		return;
	}

#if 1
	char szNodeName[256];
	strcpy_s(szNodeName, cstrName);
	ST_ENGINE_STATUS stEngineStat;

	__int64 i64 = g_pTagCol->GetLocalEngStat(stEngineStat);
	if(i64 == 1)
	{
		result = stEngineStat.nHmiRedundancy_Status_Cur;
		return;
	}
	else
	{
		result = INT_MIN;
		return;
	}
#else
	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_GetHmiRedundancyStatusCur;

	c_string a;
	pargs[0]->as_string(a);
	strcpy_s(global.szParms1, a);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	char buf[BUFFER_SIZE];
	buf[0] = 0x0;
	int nRet = GetReturnVal(3000, global, buf);
	if(nRet == 1)
	{
		nRet = atoi(buf);

		/*
		// string 타입 반환
		CString sValue = "abc";
		result.vt = VT_BSTR;
		result.bstrVal = sValue.AllocSysString();
		return;
		*/
	}

	result = nRet;
#endif
}

bool _check_SetObjectFont(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 3)
	{
		*p_msg = "'SetObjectFont' function gets 3 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of SetObjectFont function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of SetObjectFont function is not a string.";
		return false;
	}
	if (p_types[2] != VARENUM::VT_BSTR)
	{
		*p_msg = "The third parameter of SetObjectFont function is not a string.";
		return false;
	}
	return true;
}

/*
	SetObjectFont("하단메뉴_Script", "txMovePage", "맑은 고딕")
*/
void __stdcall SetObjectFont(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 3 ||
		pargs[0]->vt != VT_BSTR ||
		pargs[1]->vt != VT_BSTR ||
		pargs[2]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
		return;
	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_OBJFONT;

	c_string a, b, c;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	pargs[2]->as_string(c);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);
	strcpy_s(global.szParms3, c);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	result = 1;
	if (!KWSendMsgToViewer(cds))
		result = INT_MIN;
}

bool _check_SetObjectFontSize(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 3)
	{
		*p_msg = "'SetObjectFontSize' function gets 3 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of SetObjectFontSize function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of SetObjectFontSize function is not a string.";
		return false;
	}
	if (p_types[2] != VARENUM::VT_I4)
	{
		*p_msg = "The third parameter of SetObjectFontSize function is not a string.";
		return false;
	}
	return true;
}

/*
	SetObjectFontSize("하단메뉴_Script", "txMovePage", 9)
*/
void __stdcall SetObjectFontSize(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 3 ||
		pargs[0]->vt != VT_BSTR ||
		pargs[1]->vt != VT_BSTR ||
		pargs[2]->vt != VT_I4)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
		return;
	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_OBJFONT_SIZE;

	c_string a, b, c;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	pargs[2]->as_string(c);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);
	strcpy_s(global.szParms3, c);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	result = 1;
	if (!KWSendMsgToViewer(cds))
		result = INT_MIN;
}

bool _check_SetObjectFontType(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 3)
	{
		*p_msg = "'SetObjectFontType' function gets 3 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of SetObjectFontType function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of SetObjectFontType function is not a string.";
		return false;
	}
	if (p_types[2] != VARENUM::VT_I4)
	{
		*p_msg = "The third parameter of SetObjectFontType function is not a string.";
		return false;
	}
	return true;
}

/*
	SetObjectFontType("하단메뉴_Script", "txMovePage", (0:일반, 1:굵게 , 2:기울기, 3:굵게+기울기))
*/
void __stdcall SetObjectFontType(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 3 ||
		pargs[0]->vt != VT_BSTR ||
		pargs[1]->vt != VT_BSTR ||
		pargs[2]->vt != VT_I4)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
		return;
	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_OBJFONT_TYPE;

	c_string a, b, c;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	pargs[2]->as_string(c);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);
	strcpy_s(global.szParms3, c);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	result = 1;
	if (!KWSendMsgToViewer(cds))
		result = INT_MIN;
}

bool _check_SetObjectForegroundColor(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 3)
	{
		*p_msg = "'SetObjectForegroundColor' function gets 3 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of SetObjectForegroundColor function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of SetObjectForegroundColor function is not a string.";
		return false;
	}
	if (p_types[2] != VARENUM::VT_BSTR)
	{
		*p_msg = "The third parameter of SetObjectForegroundColor function is not a string.";
		return false;
	}
	return true;
}

/*
	SetObjectForegroundColor("하단메뉴_Script", "txMovePage", "(255,255,255)")
*/
void __stdcall SetObjectForegroundColor(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 3 ||
		pargs[0]->vt != VT_BSTR ||
		pargs[1]->vt != VT_BSTR ||
		pargs[2]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
		return;
	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_OBJCOLOR_FORE;

	c_string a, b, c;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	pargs[2]->as_string(c);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);
	strcpy_s(global.szParms3, c);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	result = 1;
	if (!KWSendMsgToViewer(cds))
		result = INT_MIN;
}

bool _check_SetObjectBackgroundColor(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 3)
	{
		*p_msg = "'SetObjectBackroundColor' function gets 3 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of SetObjectBackgroundColor function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of SetObjectBackgroundColor function is not a string.";
		return false;
	}
	if (p_types[2] != VARENUM::VT_BSTR)
	{
		*p_msg = "The third parameter of SetObjectBackgroundColor function is not a string.";
		return false;
	}
	return true;
}

/*
	SetObjectBackgroundColor("하단메뉴_Script", "txMovePage", "(255,255,255)")
*/
void __stdcall SetObjectBackgroundColor(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 3 ||
		pargs[0]->vt != VT_BSTR ||
		pargs[1]->vt != VT_BSTR ||
		pargs[2]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
		return;
	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_OBJCOLOR_BACK;

	c_string a, b, c;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	pargs[2]->as_string(c);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);
	strcpy_s(global.szParms3, c);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	result = 1;
	if (!KWSendMsgToViewer(cds))
		result = INT_MIN;
}

bool _check_SetObjectOutlineColor(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 3)
	{
		*p_msg = "'SetObjectOutlineColor' function gets 3 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of SetObjectOutlineColor function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of SetObjectOutlineColor function is not a string.";
		return false;
	}
	if (p_types[2] != VARENUM::VT_BSTR)
	{
		*p_msg = "The third parameter of SetObjectOutlineColor function is not a string.";
		return false;
	}
	return true;
}

/*
	SetObjectOutlineColor("하단메뉴_Script", "txMovePage", "(255,255,255)")
*/
void __stdcall SetObjectOutlineColor(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 3 ||
		pargs[0]->vt != VT_BSTR ||
		pargs[1]->vt != VT_BSTR ||
		pargs[2]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
		return;
	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_OBJCOLOR_LINE;

	c_string a, b, c;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	pargs[2]->as_string(c);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);
	strcpy_s(global.szParms3, c);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	result = 1;
	if (!KWSendMsgToViewer(cds))
		result = INT_MIN;
}

bool _check_SetObjectFillValueX(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 3)
	{
		*p_msg = "'SetObjectFillValueX' function gets 3 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of SetObjectFillValueX function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of SetObjectFillValueX function is not a string.";
		return false;
	}
	if (p_types[2] != VARENUM::VT_I4)
	{
		*p_msg = "The third parameter of SetObjectFillValueX function is not a string.";
		return false;
	}
	return true;
}

/*
	SetObjectFillValueX("하단메뉴_Script", "txMovePage", 채우기값(0~100))
*/
void __stdcall SetObjectFillValueX(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 3 ||
		pargs[0]->vt != VT_BSTR ||
		pargs[1]->vt != VT_BSTR ||
		pargs[2]->vt != VT_I4)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
		return;
	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_OBJCOLOR_FILL_X;

	c_string a, b, c;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	pargs[2]->as_string(c);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);
	strcpy_s(global.szParms3, c);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	result = 1;
	if (!KWSendMsgToViewer(cds))
		result = INT_MIN;
}

bool _check_SetObjectFillValueY(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 3)
	{
		*p_msg = "'SetObjectFillValueY' function gets 3 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of SetObjectFillValueY function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of SetObjectFillValueY function is not a string.";
		return false;
	}
	if (p_types[2] != VARENUM::VT_I4)
	{
		*p_msg = "The third parameter of SetObjectFillValueY function is not a string.";
		return false;
	}
	return true;
}

/*
	SetObjectFillValueY("하단메뉴_Script", "txMovePage", 채우기값(0~100))
*/
void __stdcall SetObjectFillValueY(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 3 ||
		pargs[0]->vt != VT_BSTR ||
		pargs[1]->vt != VT_BSTR ||
		pargs[2]->vt != VT_I4)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
		return;
	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_OBJCOLOR_FILL_Y;

	c_string a, b, c;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	pargs[2]->as_string(c);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);
	strcpy_s(global.szParms3, c);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	result = 1;
	if (!KWSendMsgToViewer(cds))
		result = INT_MIN;
}

bool _check_SetObjectFillDirectionX(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 3)
	{
		*p_msg = "'SetObjectFillDirectionX' function gets 3 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of SetObjectFillDirectionX function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of SetObjectFillDirectionX function is not a string.";
		return false;
	}
	if (p_types[2] != VARENUM::VT_I4)
	{
		*p_msg = "The third parameter of SetObjectFillDirectionX function is not a string.";
		return false;
	}
	return true;
}

/*
	SetObjectFillDirectionX("하단메뉴_Script", "txMovePage", 채우기타입(0:위->아래, 1:가운데->바깥, 2:아래->위))
*/
void __stdcall SetObjectFillDirectionX(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 3 ||
		pargs[0]->vt != VT_BSTR ||
		pargs[1]->vt != VT_BSTR ||
		pargs[2]->vt != VT_I4)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
		return;
	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_OBJCOLOR_FILLDIR_X;

	c_string a, b, c;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	pargs[2]->as_string(c);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);
	strcpy_s(global.szParms3, c);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	result = 1;
	if (!KWSendMsgToViewer(cds))
		result = INT_MIN;
}

bool _check_SetObjectFillDirectionY(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 3)
	{
		*p_msg = "'SetObjectFillDirectionY' function gets 3 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of SetObjectFillDirectionY function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of SetObjectFillDirectionY function is not a string.";
		return false;
	}
	if (p_types[2] != VARENUM::VT_I4)
	{
		*p_msg = "The third parameter of SetObjectFillDirectionY function is not a string.";
		return false;
	}
	return true;
}

/*
	SetObjectFillDirectionY("하단메뉴_Script", "txMovePage", 채우기타입(0:위->아래, 1:가운데->바깥, 2:아래->위))
*/
void __stdcall SetObjectFillDirectionY(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 3 ||
		pargs[0]->vt != VT_BSTR ||
		pargs[1]->vt != VT_BSTR ||
		pargs[2]->vt != VT_I4)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
		return;
	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_OBJCOLOR_FILLDIR_Y;

	c_string a, b, c;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	pargs[2]->as_string(c);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);
	strcpy_s(global.szParms3, c);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	result = 1;
	if (!KWSendMsgToViewer(cds))
		result = INT_MIN;
}

bool _check_SetObjectRotate(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 3)
	{
		*p_msg = "'SetObjectRotate' function gets 3 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of SetObjectRotate function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of SetObjectRotate function is not a string.";
		return false;
	}
	if (p_types[2] != VARENUM::VT_I4)
	{
		*p_msg = "The third parameter of SetObjectRotate function is not a string.";
		return false;
	}
	return true;
}

/*
	SetObjectRotate("하단메뉴_Script", "txMovePage", 회전값(0~360))
*/
void __stdcall SetObjectRotate(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 3 ||
		pargs[0]->vt != VT_BSTR ||
		pargs[1]->vt != VT_BSTR ||
		pargs[2]->vt != VT_I4)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
		return;
	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_OBJROTATE;

	c_string a, b, c;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	pargs[2]->as_string(c);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);
	strcpy_s(global.szParms3, c);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	result = 1;
	if (!KWSendMsgToViewer(cds))
		result = INT_MIN;
}

bool _check_SetObjectSizeX(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 3)
	{
		*p_msg = "'SetObjectSizeX' function gets 3 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of SetObjectSizeX function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of SetObjectSizeX function is not a string.";
		return false;
	}
	if (p_types[2] != VARENUM::VT_I4)
	{
		*p_msg = "The third parameter of SetObjectSizeX function is not a string.";
		return false;
	}
	return true;
}

/*
	SetObjectSizeX("하단메뉴_Script", "txMovePage", 가로크기값)
*/
void __stdcall SetObjectSizeX(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 3 ||
		pargs[0]->vt != VT_BSTR ||
		pargs[1]->vt != VT_BSTR ||
		pargs[2]->vt != VT_I4)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
		return;
	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_OBJSIZE_X;

	c_string a, b, c;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	pargs[2]->as_string(c);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);
	strcpy_s(global.szParms3, c);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	result = 1;
	if (!KWSendMsgToViewer(cds))
		result = INT_MIN;
}

bool _check_SetObjectSizeY(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 3)
	{
		*p_msg = "'SetObjectSizeY' function gets 3 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of SetObjectSizeY function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of SetObjectSizeY function is not a string.";
		return false;
	}
	if (p_types[2] != VARENUM::VT_I4)
	{
		*p_msg = "The third parameter of SetObjectSizeY function is not a string.";
		return false;
	}
	return true;
}

/*
	SetObjectSizeX("하단메뉴_Script", "txMovePage", 세로크기값)
*/
void __stdcall SetObjectSizeY(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 3 ||
		pargs[0]->vt != VT_BSTR ||
		pargs[1]->vt != VT_BSTR ||
		pargs[2]->vt != VT_I4)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
		return;
	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_OBJSIZE_Y;

	c_string a, b, c;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	pargs[2]->as_string(c);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);
	strcpy_s(global.szParms3, c);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	result = 1;
	if (!KWSendMsgToViewer(cds))
		result = INT_MIN;
}

bool _check_SetObjectSize(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 4)
	{
		*p_msg = "'SetObjectSize' function gets 4 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of SetObjectSize function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of SetObjectSize function is not a string.";
		return false;
	}
	if (p_types[2] != VARENUM::VT_I4)
	{
		*p_msg = "The third parameter of SetObjectSize function is not a string.";
		return false;
	}
	if (p_types[3] != VARENUM::VT_I4)
	{
		*p_msg = "The third parameter of SetObjectSize function is not a string.";
		return false;
	}
	return true;
}

/*
	SetObjectSize("하단메뉴_Script", "txMovePage", 가로크기값, 세로크기값)
*/
void __stdcall SetObjectSize(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 4 ||
		pargs[0]->vt != VT_BSTR ||
		pargs[1]->vt != VT_BSTR ||
		pargs[2]->vt != VT_I4	||
		pargs[3]->vt != VT_I4)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
		return;
	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_OBJSIZE_XY;

	c_string a, b, c, d;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	pargs[2]->as_string(c);
	pargs[3]->as_string(d);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);
	strcpy_s(global.szParms3, c);
	strcpy_s(global.szParms4, d);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	result = 1;
	if (!KWSendMsgToViewer(cds))
		result = INT_MIN;
}

bool _check_SetObjectPosX(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 3)
	{
		*p_msg = "'SetObjectPosX' function gets 3 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of SetObjectPosX function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of SetObjectPosX function is not a string.";
		return false;
	}
	if (p_types[2] != VARENUM::VT_I4)
	{
		*p_msg = "The third parameter of SetObjectPosX function is not a string.";
		return false;
	}
	return true;
}

/*
	SetObjectPosX("하단메뉴_Script", "txMovePage", X위치값)
*/
void __stdcall SetObjectPosX(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 3 ||
		pargs[0]->vt != VT_BSTR ||
		pargs[1]->vt != VT_BSTR ||
		pargs[2]->vt != VT_I4)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
		return;
	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_OBJPOS_X;

	c_string a, b, c;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	pargs[2]->as_string(c);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);
	strcpy_s(global.szParms3, c);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	result = 1;
	if (!KWSendMsgToViewer(cds))
		result = INT_MIN;
}

bool _check_SetObjectPosY(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 3)
	{
		*p_msg = "'SetObjectPosY' function gets 3 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of SetObjectPosY function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of SetObjectPosY function is not a string.";
		return false;
	}
	if (p_types[2] != VARENUM::VT_I4)
	{
		*p_msg = "The third parameter of SetObjectPosY function is not a string.";
		return false;
	}
	return true;
}

/*
	SetObjectPosX("하단메뉴_Script", "txMovePage", Y위치값)
*/
void __stdcall SetObjectPosY(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 3 ||
		pargs[0]->vt != VT_BSTR ||
		pargs[1]->vt != VT_BSTR ||
		pargs[2]->vt != VT_I4)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
		return;
	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_OBJPOS_Y;

	c_string a, b, c;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	pargs[2]->as_string(c);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);
	strcpy_s(global.szParms3, c);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	result = 1;
	if (!KWSendMsgToViewer(cds))
		result = INT_MIN;
}

bool _check_SetObjectPos(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 4)
	{
		*p_msg = "'SetObjectPos' function gets 4 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of SetObjectPos function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of SetObjectPos function is not a string.";
		return false;
	}
	if (p_types[2] != VARENUM::VT_I4)
	{
		*p_msg = "The third parameter of SetObjectPos function is not a string.";
		return false;
	}
	if (p_types[3] != VARENUM::VT_I4)
	{
		*p_msg = "The third parameter of SetObjectPos function is not a string.";
		return false;
	}
	return true;
}

/*
	SetObjectPos("하단메뉴_Script", "txMovePage", X위치값, Y위치값)
*/
void __stdcall SetObjectPos(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 4 ||
		pargs[0]->vt != VT_BSTR ||
		pargs[1]->vt != VT_BSTR ||
		pargs[2]->vt != VT_I4 ||
		pargs[3]->vt != VT_I4)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
		return;
	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_OBJPOS_XY;

	c_string a, b, c, d;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	pargs[2]->as_string(c);
	pargs[3]->as_string(d);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);
	strcpy_s(global.szParms3, c);
	strcpy_s(global.szParms4, d);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	result = 1;
	if (!KWSendMsgToViewer(cds))
		result = INT_MIN;
}

bool _check_GetObjectVisible(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'GetObjectVisible' function gets 2 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of GetObjectVisible function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of GetObjectVisible function is not a string.";
		return false;
	}

	return true;
}

/*
dim nRet as integer
nRet = GetObjectVisible(도면 이름, 객체 이름)
*/
/*
0 : 안보이는 중
1 : 보이는 중
*/
void __stdcall GetObjectVisible(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 2 || pargs[0]->vt != VT_BSTR || pargs[1]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
	{
		result = INT_MIN;
		return;
	}

	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_Get_OBJVISBLE;

	c_string a, b;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	char buf[BUFFER_SIZE];
	buf[0] = 0x0;
	int nRet = GetReturnVal(3000, global, buf);
	if (nRet == 1)
	{
		nRet = atoi(buf);
	}

	result = nRet;
}

bool _check_GetObjectText(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'GetObjectText' function gets 2 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of GetObjectText function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of GetObjectText function is not a string.";
		return false;
	}

	return true;
}

/*
dim s as string
s = GetObjectText(도면 이름, 객체 이름)
*/
void __stdcall GetObjectText(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 2 || pargs[0]->vt != VT_BSTR || pargs[1]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
	{
		result = INT_MIN;
		return;
	}

	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_Get_OBJTEXT;

	c_string a, b;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	char buf[BUFFER_SIZE];
	buf[0] = 0x0;
	int nRet = GetReturnVal(3000, global, buf);

	CString sValue;
	result.vt = VT_BSTR;

	if (nRet == 1)
		sValue = buf;

	result.bstrVal = sValue.AllocSysString();
}

bool _check_GetObjectFont(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'GetObjectFont' function gets 2 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of GetObjectFont function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of GetObjectFont function is not a string.";
		return false;
	}

	return true;
}

/*
dim s as string
s = GetObjectFont(도면 이름, 객체 이름)
*/
/*
폰트 종류
*/
void __stdcall GetObjectFont(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 2 || pargs[0]->vt != VT_BSTR || pargs[1]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
	{
		result = INT_MIN;
		return;
	}

	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_Get_OBJFONT;

	c_string a, b;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	char buf[BUFFER_SIZE];
	buf[0] = 0x0;
	int nRet = GetReturnVal(3000, global, buf);

	CString sValue;
	result.vt = VT_BSTR;

	if (nRet == 1)
		sValue = buf;

	result.bstrVal = sValue.AllocSysString();
}

bool _check_GetObjectFontSize(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'GetObjectFontSize' function gets 2 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of GetObjectFontSize function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of GetObjectFontSize function is not a string.";
		return false;
	}

	return true;
}

/*
dim nRet as integer
nRet = GetObjectFontSize(도면 이름, 객체 이름)
*/
/*
폰트 사이즈
*/
void __stdcall GetObjectFontSize(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 2 || pargs[0]->vt != VT_BSTR || pargs[1]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
	{
		result = INT_MIN;
		return;
	}

	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_Get_OBJFONT_SIZE;

	c_string a, b;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	char buf[BUFFER_SIZE];
	buf[0] = 0x0;
	int nRet = GetReturnVal(3000, global, buf);
	if (nRet == 1)
	{
		nRet = atoi(buf);
	}

	result = nRet;
}

bool _check_GetObjectFontType(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'GetObjectFontType' function gets 2 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of GetObjectFontType function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of GetObjectFontType function is not a string.";
		return false;
	}

	return true;
}

/*
dim nRet as integer
nRet = GetObjectFontType(도면 이름, 객체 이름)
*/
/*
0 : 일반
1 : 굵게
2 : 기울게
3 : 굵게 + 기울게
*/
void __stdcall GetObjectFontType(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 2 || pargs[0]->vt != VT_BSTR || pargs[1]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
	{
		result = INT_MIN;
		return;
	}

	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_Get_OBJFONT_TYPE;

	c_string a, b;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	char buf[BUFFER_SIZE];
	buf[0] = 0x0;
	int nRet = GetReturnVal(3000, global, buf);
	if (nRet == 1)
	{
		nRet = atoi(buf);
	}

	result = nRet;
}

bool _check_GetObjectForegroundColor(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'GetObjectForegroundColor' function gets 2 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of GetObjectForegroundColor function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of GetObjectForegroundColor function is not a string.";
		return false;
	}

	return true;
}

/*
dim s as string
s = GetObjectForegroundColor(도면 이름, 객체 이름)
*/
/*
전면색상("R,G,B")
*/
void __stdcall GetObjectForegroundColor(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 2 || pargs[0]->vt != VT_BSTR || pargs[1]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
	{
		result = INT_MIN;
		return;
	}

	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_Get_OBJCOLOR_FORE;

	c_string a, b;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	char buf[BUFFER_SIZE];
	buf[0] = 0x0;
	int nRet = GetReturnVal(3000, global, buf);

	CString sValue;
	result.vt = VT_BSTR;

	if (nRet == 1)
		sValue = buf;

	result.bstrVal = sValue.AllocSysString();
}

bool _check_GetObjectBackgroundColor(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'GetObjectBackgroundColor' function gets 2 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of GetObjectBackgroundColor function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of GetObjectBackgroundColor function is not a string.";
		return false;
	}

	return true;
}

/*
dim s as string
s = GetObjectBackgroundColor(도면 이름, 객체 이름)
*/
/*
배경색상("R,G,B")
*/
void __stdcall GetObjectBackgroundColor(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 2 || pargs[0]->vt != VT_BSTR || pargs[1]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
	{
		result = INT_MIN;
		return;
	}

	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_Get_OBJCOLOR_BACK;

	c_string a, b;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	char buf[BUFFER_SIZE];
	buf[0] = 0x0;
	int nRet = GetReturnVal(3000, global, buf);

	CString sValue;
	result.vt = VT_BSTR;

	if (nRet == 1)
		sValue = buf;

	result.bstrVal = sValue.AllocSysString();
}

bool _check_GetObjectOutlineColor(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'GetObjectOutlineColor' function gets 2 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of GetObjectOutlineColor function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of GetObjectOutlineColor function is not a string.";
		return false;
	}

	return true;
}

/*
dim s as string
s = GetObjectOutlineColor(도면 이름, 객체 이름)
*/
/*
외각선색상("R,G,B")
*/
void __stdcall GetObjectOutlineColor(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 2 || pargs[0]->vt != VT_BSTR || pargs[1]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
	{
		result = INT_MIN;
		return;
	}

	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_Get_OBJCOLOR_LINE;

	c_string a, b;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	char buf[BUFFER_SIZE];
	buf[0] = 0x0;
	int nRet = GetReturnVal(3000, global, buf);

	CString sValue;
	result.vt = VT_BSTR;

	if (nRet == 1)
		sValue = buf;

	result.bstrVal = sValue.AllocSysString();
}

bool _check_GetObjectFillValueX(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'GetObjectFillValueX' function gets 2 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of GetObjectFillValueX function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of GetObjectFillValueX function is not a string.";
		return false;
	}

	return true;
}

/*
dim nRet as integer
nRet = GetObjectFillValueX(도면 이름, 객체 이름)
*/
/*
채우기 X 값 (0 ~ 100)
*/
void __stdcall GetObjectFillValueX(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 2 || pargs[0]->vt != VT_BSTR || pargs[1]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
	{
		result = INT_MIN;
		return;
	}

	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_Get_OBJCOLOR_FILL_X;

	c_string a, b;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	char buf[BUFFER_SIZE];
	buf[0] = 0x0;
	int nRet = GetReturnVal(3000, global, buf);
	if (nRet == 1)
	{
		nRet = atoi(buf);
	}

	result = nRet;
}

bool _check_GetObjectFillValueY(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'GetObjectFillValueY' function gets 2 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of GetObjectFillValueY function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of GetObjectFillValueY function is not a string.";
		return false;
	}

	return true;
}

/*
dim nRet as integer
nRet = GetObjectFillValueY(도면 이름, 객체 이름)
*/
/*
채우기 Y 값 (0 ~ 100)
*/
void __stdcall GetObjectFillValueY(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 2 || pargs[0]->vt != VT_BSTR || pargs[1]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
	{
		result = INT_MIN;
		return;
	}

	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_Get_OBJCOLOR_FILL_Y;

	c_string a, b;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	char buf[BUFFER_SIZE];
	buf[0] = 0x0;
	int nRet = GetReturnVal(3000, global, buf);
	if (nRet == 1)
	{
		nRet = atoi(buf);
	}

	result = nRet;
}

bool _check_GetObjectFillDirectionX(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'GetObjectFillDirectionX' function gets 2 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of GetObjectFillDirectionX function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of GetObjectFillDirectionX function is not a string.";
		return false;
	}

	return true;
}

/*
dim nRet as integer
nRet = GetObjectFillDirectionX(도면 이름, 객체 이름)
*/
/*
0 : 위 -> 아래
1 : 가운데 -> 바깥
2 : 아래 -> 위
*/
void __stdcall GetObjectFillDirectionX(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 2 || pargs[0]->vt != VT_BSTR || pargs[1]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
	{
		result = INT_MIN;
		return;
	}

	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_Get_OBJCOLOR_FILLDIR_X;

	c_string a, b;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	char buf[BUFFER_SIZE];
	buf[0] = 0x0;
	int nRet = GetReturnVal(3000, global, buf);
	if (nRet == 1)
	{
		nRet = atoi(buf);
	}

	result = nRet;
}

bool _check_GetObjectFillDirectionY(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'GetObjectFillDirectionY' function gets 2 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of GetObjectFillDirectionY function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of GetObjectFillDirectionY function is not a string.";
		return false;
	}

	return true;
}

/*
dim nRet as integer
nRet = GetObjectFillDirectionY(도면 이름, 객체 이름)
*/
/*
0 : 왼쪽 -> 오른쪽
1 : 가운데 -> 바깥
2 : 오른쪽 -> 왼쪽
*/
void __stdcall GetObjectFillDirectionY(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 2 || pargs[0]->vt != VT_BSTR || pargs[1]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
	{
		result = INT_MIN;
		return;
	}

	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_Get_OBJCOLOR_FILLDIR_Y;

	c_string a, b;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	char buf[BUFFER_SIZE];
	buf[0] = 0x0;
	int nRet = GetReturnVal(3000, global, buf);
	if (nRet == 1)
	{
		nRet = atoi(buf);
	}

	result = nRet;
}

bool _check_GetObjectRotate(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'GetObjectRotate' function gets 2 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of GetObjectRotate function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of GetObjectRotate function is not a string.";
		return false;
	}

	return true;
}

/*
dim nRet as integer
nRet = GetObjectRotate(도면 이름, 객체 이름)
*/
/*
각도 값 (0 ~ 360)
*/
void __stdcall GetObjectRotate(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 2 || pargs[0]->vt != VT_BSTR || pargs[1]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
	{
		result = INT_MIN;
		return;
	}

	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_Get_OBJROTATE;

	c_string a, b;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	char buf[BUFFER_SIZE];
	buf[0] = 0x0;
	int nRet = GetReturnVal(3000, global, buf);
	if (nRet == 1)
	{
		nRet = atoi(buf);
	}

	result = nRet;
}

bool _check_GetObjectSizeX(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'GetObjectSizeX' function gets 2 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of GetObjectSizeX function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of GetObjectSizeX function is not a string.";
		return false;
	}

	return true;
}

/*
dim nRet as integer
nRet = GetObjectSizeX(도면 이름, 객체 이름)
*/
/*
가로 크기
*/
void __stdcall GetObjectSizeX(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 2 || pargs[0]->vt != VT_BSTR || pargs[1]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
	{
		result = INT_MIN;
		return;
	}

	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_Get_OBJSIZE_X;

	c_string a, b;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	char buf[BUFFER_SIZE];
	buf[0] = 0x0;
	int nRet = GetReturnVal(3000, global, buf);
	if (nRet == 1)
	{
		nRet = atoi(buf);
	}

	result = nRet;
}

bool _check_GetObjectSizeY(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'GetObjectSizeY' function gets 2 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of GetObjectSizeY function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of GetObjectSizeY function is not a string.";
		return false;
	}

	return true;
}

/*
dim nRet as integer
nRet = GetObjectSizeY(도면 이름, 객체 이름)
*/
/*
세로 크기
*/
void __stdcall GetObjectSizeY(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 2 || pargs[0]->vt != VT_BSTR || pargs[1]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
	{
		result = INT_MIN;
		return;
	}

	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_Get_OBJSIZE_Y;

	c_string a, b;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	char buf[BUFFER_SIZE];
	buf[0] = 0x0;
	int nRet = GetReturnVal(3000, global, buf);
	if (nRet == 1)
	{
		nRet = atoi(buf);
	}

	result = nRet;
}

bool _check_GetObjectSize(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'GetObjectSize' function gets 2 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of GetObjectSize function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of GetObjectSize function is not a string.";
		return false;
	}

	return true;
}

/*
dim s as string
s = GetObjectSize(도면 이름, 객체 이름)
*/
/*
크기("가로,세로")
*/
void __stdcall GetObjectSize(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 2 || pargs[0]->vt != VT_BSTR || pargs[1]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
	{
		result = INT_MIN;
		return;
	}

	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_Get_OBJSIZE_XY;

	c_string a, b;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	char buf[BUFFER_SIZE];
	buf[0] = 0x0;
	int nRet = GetReturnVal(3000, global, buf);

	CString sValue;
	result.vt = VT_BSTR;

	if (nRet == 1)
		sValue = buf;

	result.bstrVal = sValue.AllocSysString();
}

bool _check_GetObjectPosX(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'GetObjectPosX' function gets 2 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of GetObjectPosX function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of GetObjectPosX function is not a string.";
		return false;
	}

	return true;
}

/*
dim nRet as integer
nRet = GetObjectPosX(도면 이름, 객체 이름)
*/
/*
X 좌표
*/
void __stdcall GetObjectPosX(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 2 || pargs[0]->vt != VT_BSTR || pargs[1]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
	{
		result = INT_MIN;
		return;
	}

	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_Get_OBJPOS_X;

	c_string a, b;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	char buf[BUFFER_SIZE];
	buf[0] = 0x0;
	int nRet = GetReturnVal(3000, global, buf);
	if (nRet == 1)
	{
		nRet = atoi(buf);
	}

	result = nRet;
}

bool _check_GetObjectPosY(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'GetObjectPosY' function gets 2 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of GetObjectPosY function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of GetObjectPosY function is not a string.";
		return false;
	}

	return true;
}

/*
dim nRet as integer
nRet = GetObjectPosY(도면 이름, 객체 이름)
*/
/*
Y 좌표
*/
void __stdcall GetObjectPosY(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 2 || pargs[0]->vt != VT_BSTR || pargs[1]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
	{
		result = INT_MIN;
		return;
	}

	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_Get_OBJPOS_Y;

	c_string a, b;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	char buf[BUFFER_SIZE];
	buf[0] = 0x0;
	int nRet = GetReturnVal(3000, global, buf);
	if (nRet == 1)
	{
		nRet = atoi(buf);
	}

	result = nRet;
}

bool _check_GetObjectPos(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'GetObjectPos' function gets 2 arguments.";
		return false;
	}
	if (p_types[0] != VARENUM::VT_BSTR)
	{
		*p_msg = "The first parameter of GetObjectPos function is not a string.";
		return false;
	}
	if (p_types[1] != VARENUM::VT_BSTR)
	{
		*p_msg = "The second parameter of GetObjectPos function is not a string.";
		return false;
	}

	return true;
}

/*
dim s as string
s = GetObjectPos(도면 이름, 객체 이름)
*/
/*
좌표("X,Y")
*/
void __stdcall GetObjectPos(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 2 || pargs[0]->vt != VT_BSTR || pargs[1]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}

	if (g_pTagCol == NULL)
	{
		result = INT_MIN;
		return;
	}

	ST_GLOBAL global;

	ZeroMemory(&global, sizeof(ST_GLOBAL));
	global.nMode = GM_GV_Get_OBJPOS_XY;

	c_string a, b;
	pargs[0]->as_string(a);
	pargs[1]->as_string(b);
	strcpy_s(global.szParms1, a);
	strcpy_s(global.szParms2, b);

	COPYDATASTRUCT cds;
	ZeroMemory(&cds, sizeof(COPYDATASTRUCT));
	cds.dwData = GM_COPYDATA_SCRIPT_CODE;
	cds.cbData = sizeof(ST_GLOBAL);
	cds.lpData = &global;

	char buf[BUFFER_SIZE];
	buf[0] = 0x0;
	int nRet = GetReturnVal(3000, global, buf);

	CString sValue;
	result.vt = VT_BSTR;

	if (nRet == 1)
		sValue = buf;

	result.bstrVal = sValue.AllocSysString();
}