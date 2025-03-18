#include "pch.h"

#include <crtdbg.h>
#include "c_engine.h"

#include "c_ext_sub_atom.h"
#include "c_assign_atom.h"
#include "c_disp_set_atom.h"
#include "c_dll_sub_atom.h"

#include "math_extensions.h"
#include "string_extensions.h"
#include "cast_extensions.h"
#include "file_extensions.h"
#include "array.h"
#include "date.h"
#include "other.h"
#include "c_disp_level.h"

#include "c_str_array.h"
#include "ScriptSystem.h"

#ifndef VERIFY
#define VERIFY	// nothing for now...
#endif

//-----------------------------------------------------------------------------
// standard extensions --------------------------------------------------------
//-----------------------------------------------------------------------------

void c_engine::add_standard_extension_constants()
{
// common -----------------------------

	c_variable t, f, e;
	t = true;
	f = false;

	VERIFY(add_extension_constant("True",	&t));
	VERIFY(add_extension_constant("False",	&f));
	VERIFY(add_extension_constant("Empty",	&e));
	VERIFY(add_extension_constant("vbCrLf",	"\r\n"));

// math -------------------------------

	VERIFY(add_extension_constant("Pi",	3.1415926535897932));

// MsgBox -----------------------------

	VERIFY(add_extension_constant("vbAbortRetryIgnore",	2));
	VERIFY(add_extension_constant("vbOKCancel",			1));
	VERIFY(add_extension_constant("vbYesNo",			4));
	VERIFY(add_extension_constant("vbOKOnly",			0));
	VERIFY(add_extension_constant("vbRetryCancel",		5));
	VERIFY(add_extension_constant("vbYesNoCancel",		3));

	VERIFY(add_extension_constant("vbAbort",		3));
	VERIFY(add_extension_constant("vbCancel",		2));
	VERIFY(add_extension_constant("vbIgnore",		5));
	VERIFY(add_extension_constant("vbNo",			7));
	VERIFY(add_extension_constant("vbOK",			1));
	VERIFY(add_extension_constant("vbRetry",		4));
	VERIFY(add_extension_constant("vbYes",			6));

// date -------------------------------

	VERIFY(add_extension_constant("vbGeneralDate",	0));
	VERIFY(add_extension_constant("vbLongDate",		1));
	VERIFY(add_extension_constant("vbShortDate",	2));
	VERIFY(add_extension_constant("vbLongTime",		3));
	VERIFY(add_extension_constant("vbShortTime",	4));

// color ------------------------------

	VERIFY(add_extension_constant("vbBlack",		0));
	VERIFY(add_extension_constant("vbBlue",			16711680));
	VERIFY(add_extension_constant("vbCyan",			16776960));
	VERIFY(add_extension_constant("vbGreen",		65280));
	VERIFY(add_extension_constant("vbMagenta",		16711935));
	VERIFY(add_extension_constant("vbRed",			255));
	VERIFY(add_extension_constant("vbWhite",		16777215));
	VERIFY(add_extension_constant("vbYellow",		65535));
}

void c_engine::add_standard_extension_functions()
{
// math -------------------------------

	VERIFY(add_extension_function("Sqr",	sqr,		_check_sqr));
	VERIFY(add_extension_function("Hypot",	hypot,		_check_hypot));
	VERIFY(add_extension_function("Abs",	abs,		_check_abs));

	VERIFY(add_extension_function("Sin",	sin,		_check_sin));
	VERIFY(add_extension_function("Cos",	cos,		_check_cos));
	VERIFY(add_extension_function("Tan",	tan,		_check_tan));
	VERIFY(add_extension_function("ATan",	atan,		_check_atan));
	VERIFY(add_extension_function("ASin",	asin,		_check_asin));
	VERIFY(add_extension_function("ACos",	acos,		_check_acos));
	VERIFY(add_extension_function("ATan2",	atan2,		_check_atan2));

	VERIFY(add_extension_function("DegToRad",DegToRad,	_check_DegToRad));
	VERIFY(add_extension_function("RadToDeg",RadToDeg,	_check_RadToDeg));

	VERIFY(add_extension_function("Exp",	exp,		_check_exp));
	VERIFY(add_extension_function("Log",	log,		_check_log));
	VERIFY(add_extension_function("Log10",	log10,		_check_log10));

	VERIFY(add_extension_function("Ceil",	ceil,		_check_ceil));
	VERIFY(add_extension_function("Floor",	floor,		_check_floor));
	VERIFY(add_extension_function("Rnd",	rnd,		_check_rnd));
	VERIFY(add_extension_function("Sign",	sign,		_check_sign));

	VERIFY(add_extension_function("Min",	bee_min,	_check_min));
	VERIFY(add_extension_function("Max",	bee_max,	_check_max));
	VERIFY(add_extension_function("Avg",	bee_avg,	_check_avg));
	VERIFY(add_extension_function("Sum",	bee_sum,	_check_sum));

	VERIFY(add_extension_function("Round",	round,		_check_round));

	VERIFY(add_extension_function("InvLog",		InvLog,			_check_InvLog));
	VERIFY(add_extension_function("InvLog10",	InvLog10,		_check_InvLog10));

// string -----------------------------

	VERIFY(add_extension_function("Split",	Split,		_check_Split));
	VERIFY(add_extension_function("Len",	Len,		_check_Len));
	VERIFY(add_extension_function("InStr",	InStr,		_check_InStr));
	VERIFY(add_extension_function("Mid",	Mid,		_check_Mid));
	VERIFY(add_extension_function("Left",	Left,		_check_Left));
	VERIFY(add_extension_function("Right",	Right,		_check_Right));
	VERIFY(add_extension_function("RTrim",	RTrim,		_check_RTrim));
	VERIFY(add_extension_function("LTrim",	LTrim,		_check_LTrim));
	VERIFY(add_extension_function("Trim",	Trim,		_check_Trim));
	VERIFY(add_extension_function("Ucase",	UCase,		_check_UCase));
	VERIFY(add_extension_function("Lcase",	LCase,		_check_LCase));
	VERIFY(add_extension_function("Chr",	Chr,		_check_Chr));
	VERIFY(add_extension_function("Asc",	Asc,		_check_Asc));

// typecast ---------------------------

	VERIFY(add_extension_function("IsInteger",	IsInteger,	_check_IsInteger));
	VERIFY(add_extension_function("IsDouble",	IsDouble,	_check_IsDouble));
	VERIFY(add_extension_function("IsString",	IsString,	_check_IsString));
	VERIFY(add_extension_function("IsArray",	IsArray,	_check_IsArray));
	VERIFY(add_extension_function("IsObject",	IsObject,	_check_IsObject));
	VERIFY(add_extension_function("IsEmpty",	IsEmpty,	_check_IsEmpty));
	VERIFY(add_extension_function("IsNull",		IsNull,		_check_IsNull));

	VERIFY(add_extension_function("CInt",		CInt,		_check_CInt));
	VERIFY(add_extension_function("CDbl",		CDbl,		_check_CDbl));
	VERIFY(add_extension_function("CStr",		CStr,		_check_CStr));
	VERIFY(add_extension_function("CDate",		CDate,		_check_CDate));

// array ------------------------------

	VERIFY(add_extension_function("UBound",		UBound,		_check_UBound));
	VERIFY(add_extension_function("NumDims",	NumDims,	_check_NumDims));
	VERIFY(add_extension_function("ReDim",		ReDim,		_check_ReDim));
	VERIFY(add_extension_function("ArrCount",	ArrCount,	_check_ArrCount));

// file -------------------------------

	VERIFY(add_extension_function("CopyFile",	copy_file,		_check_copy_file));
	VERIFY(add_extension_function("MoveFile",	move_file,		_check_move_file));
	VERIFY(add_extension_function("FileExists",	file_exists,	_check_file_exists));
	VERIFY(add_extension_function("DeleteFile",	delete_file,	_check_delete_file));
	VERIFY(add_extension_function("OpenFileDlg",open_file_dlg,	_check_open_file_dlg));
	VERIFY(add_extension_function("SaveFileDlg",save_file_dlg,	_check_save_file_dlg));
	VERIFY(add_extension_function("MkDir",		mk_dir,			_check_mk_dir));
	VERIFY(add_extension_function("FileSize",	file_size,		_check_file_size));
	VERIFY(add_extension_function("DirSize",	dir_size,		_check_dir_size));
	VERIFY(add_extension_function("SelectDirDlg",select_dir_dlg,_check_select_dir_dlg));
	VERIFY(add_extension_function("GetCurrentDirectory",get_cur_dir,_check_get_cur_dir));
	VERIFY(add_extension_function("SetCurrentDirectory",set_cur_dir,_check_set_cur_dir));

// date -------------------------------

	VERIFY(add_extension_function("StrToDate",	StrToDate,		_check_StrToDate));
	VERIFY(add_extension_function("FormatDateTime",FormatDateTime,_check_FormatDateTime));
	VERIFY(add_extension_function("DateSerial",	DateSerial,		_check_DateSerial));
	VERIFY(add_extension_function("TimeSerial",	TimeSerial,		_check_TimeSerial));

	VERIFY(add_extension_function("Now",		Now,			_check_Now));
	VERIFY(add_extension_function("Year",		Year,			_check_Year));
	VERIFY(add_extension_function("Month",		Month,			_check_Month));
	VERIFY(add_extension_function("Day",		DayOfMonth,		_check_DayOfMonth));
	VERIFY(add_extension_function("Hour",		Hour,			_check_Hour));
	VERIFY(add_extension_function("Minute",		Minute,			_check_Minute));
	VERIFY(add_extension_function("Second",		Second,			_check_Second));
	VERIFY(add_extension_function("DayOfWeek",	DayOfWeek,		_check_DayOfWeek));
	VERIFY(add_extension_function("DayOfYear",	DayOfYear,		_check_DayOfYear));
	VERIFY(add_extension_function("WeekOfYear",	WeekOfYear,		_check_WeekOfYear));
	VERIFY(add_extension_function("WeekOfYear2",WeekOfYear2,	_check_WeekOfYear2));

// other ------------------------------

	VERIFY(add_extension_function("CreateObject",CreateObject,	_check_CreateObject));
	VERIFY(add_extension_function("MsgBox",		MsgBox,			_check_MsgBox));

	VERIFY(add_extension_function("IIF",		iif,			_check_iif));
	VERIFY(add_extension_function("Eval",		eval,			_check_eval));

	//20220715 YDK 수정 - GetTagVlaue
	VERIFY(add_extension_function("GetTagValue", GetTagValue, _check_GetTagValue));

	//20220715 YDK 수정 - SetTagVlaue
	VERIFY(add_extension_function("SetTagValue", SetTagValue, _check_SetTagValue));
	VERIFY(add_extension_function("Sleep", Sleep, _check_Sleep));

	//20230118 PHK 추가 - SetDiTagToggle
	VERIFY(add_extension_function("ToggleDigitalPoint", ToggleDigitalPoint, _check_ToggleDigitalPoint));
	VERIFY(add_extension_function("AcknowledgeAllAlarms", AcknowledgeAllAlarms, _check_AcknowledgeAllAlarms));
	VERIFY(add_extension_function("AcknowledgeAnAlarm", AcknowledgeAnAlarm, _check_AcknowledgeAnAlarm));
	VERIFY(add_extension_function("DisableAlarm", DisableAlarm, _check_DisableAlarm));
	VERIFY(add_extension_function("EnableAlarm", EnableAlarm, _check_EnableAlarm));
	VERIFY(add_extension_function("CloseDigitalPoint", CloseDigitalPoint, _check_CloseDigitalPoint));
	VERIFY(add_extension_function("OpenDigitalPoint", OpenDigitalPoint, _check_OpenDigitalPoint));

	VERIFY(add_extension_function("ClosePicture", ClosePicture, _check_ClosePicture));
	VERIFY(add_extension_function("OpenPicture", OpenPicture, _check_OpenPicture));
	VERIFY(add_extension_function("ReplacePicture", ReplacePicture, _check_ReplacePicture));
	VERIFY(add_extension_function("OffScan", OffScan, _check_OffScan));
	VERIFY(add_extension_function("OnScan", OnScan, _check_OnScan));
	VERIFY(add_extension_function("ToggleScan", ToggleScan, _check_ToggleScan));
	VERIFY(add_extension_function("ReadValue", ReadValue, _check_ReadValue));
	VERIFY(add_extension_function("WriteValue", WriteValue, _check_WriteValue));
	VERIFY(add_extension_function("RampValue", RampValue, _check_RampValue));
	//VERIFY(add_extension_function("LocateObject", LocateObject, _check_LocateObject));

	VERIFY(add_extension_function("Login", Login, _check_Login));
	//VERIFY(add_extension_function("PictureAlias", PictureAlias, _check_PictureAlias));

	VERIFY(add_extension_function("Execute", Execute, _check_Execute));
	VERIFY(add_extension_function("HmiDuplexTransfer", HmiDuplexTransfer, _check_HmiDuplexTransfer));
	VERIFY(add_extension_function("HmiNetworkTransfer", HmiNetworkTransfer, _check_HmiNetworkTransfer));
	VERIFY(add_extension_function("sprintf", sprintf, _check_sprintf));

	VERIFY(add_extension_function("SetObjectVisible", SetObjectVisible, _check_SetObjectVisible));
	VERIFY(add_extension_function("SetObjectText", SetObjectText, _check_SetObjectText));
	VERIFY(add_extension_function("SetNodePrimaryType", SetNodePrimaryType, _check_SetNodePrimaryType));
	VERIFY(add_extension_function("ChgEngActiveStandby", ChgEngActiveStandby, _check_ChgEngActiveStandby));

	VERIFY(add_extension_function("GetHmiRedundancyStatusCur", GetHmiRedundancyStatusCur, _check_GetHmiRedundancyStatusCur));
	VERIFY(add_extension_function("SetObjectFont", SetObjectFont, _check_SetObjectFont));
	VERIFY(add_extension_function("SetObjectFontSize", SetObjectFontSize, _check_SetObjectFontSize));
	VERIFY(add_extension_function("SetObjectFontType", SetObjectFontType, _check_SetObjectFontType));
	VERIFY(add_extension_function("SetObjectForegroundColor", SetObjectForegroundColor, _check_SetObjectForegroundColor));
	VERIFY(add_extension_function("SetObjectBackgroundColor", SetObjectBackgroundColor, _check_SetObjectBackgroundColor));
	VERIFY(add_extension_function("SetObjectOutlineColor", SetObjectOutlineColor, _check_SetObjectOutlineColor));
	VERIFY(add_extension_function("SetObjectFillValueX", SetObjectFillValueX, _check_SetObjectFillValueX));
	VERIFY(add_extension_function("SetObjectFillValueY", SetObjectFillValueY, _check_SetObjectFillValueY));
	VERIFY(add_extension_function("SetObjectFillDirectionX", SetObjectFillDirectionX, _check_SetObjectFillDirectionX));
	VERIFY(add_extension_function("SetObjectFillDirectionY", SetObjectFillDirectionY, _check_SetObjectFillDirectionY));
	VERIFY(add_extension_function("SetObjectRotate", SetObjectRotate, _check_SetObjectRotate));
	VERIFY(add_extension_function("SetObjectSizeX", SetObjectSizeX, _check_SetObjectSizeX));
	VERIFY(add_extension_function("SetObjectSizeY", SetObjectSizeY, _check_SetObjectSizeY));
	VERIFY(add_extension_function("SetObjectSize", SetObjectSize, _check_SetObjectSize));
	VERIFY(add_extension_function("SetObjectPosX", SetObjectPosX, _check_SetObjectPosX));
	VERIFY(add_extension_function("SetObjectPosY", SetObjectPosY, _check_SetObjectPosY));
	VERIFY(add_extension_function("SetObjectPos", SetObjectPos, _check_SetObjectPos));
	VERIFY(add_extension_function("GetObjectVisible", GetObjectVisible, _check_GetObjectVisible));
	VERIFY(add_extension_function("GetObjectText", GetObjectText, _check_GetObjectText));
	VERIFY(add_extension_function("GetObjectFont", GetObjectFont, _check_GetObjectFont));
	VERIFY(add_extension_function("GetObjectFontSize", GetObjectFontSize, _check_GetObjectFontSize));
	VERIFY(add_extension_function("GetObjectFontType", GetObjectFontType, _check_GetObjectFontType));
	VERIFY(add_extension_function("GetObjectForegroundColor", GetObjectForegroundColor, _check_GetObjectForegroundColor));
	VERIFY(add_extension_function("GetObjectBackgroundColor", GetObjectBackgroundColor, _check_GetObjectBackgroundColor));
	VERIFY(add_extension_function("GetObjectOutlineColor", GetObjectOutlineColor, _check_GetObjectOutlineColor));
	VERIFY(add_extension_function("GetObjectFillValueX", GetObjectFillValueX, _check_GetObjectFillValueX));
	VERIFY(add_extension_function("GetObjectFillValueY", GetObjectFillValueY, _check_GetObjectFillValueY));
	VERIFY(add_extension_function("GetObjectFillDirectionX", GetObjectFillDirectionX, _check_GetObjectFillDirectionX));
	VERIFY(add_extension_function("GetObjectFillDirectionY", GetObjectFillDirectionY, _check_GetObjectFillDirectionY));
	VERIFY(add_extension_function("GetObjectRotate", GetObjectRotate, _check_GetObjectRotate));
	VERIFY(add_extension_function("GetObjectSizeX", GetObjectSizeX, _check_GetObjectSizeX));
	VERIFY(add_extension_function("GetObjectSizeY", GetObjectSizeY, _check_GetObjectSizeY));
	VERIFY(add_extension_function("GetObjectSize", GetObjectSize, _check_GetObjectSize));
	VERIFY(add_extension_function("GetObjectPosX", GetObjectPosX, _check_GetObjectPosX));
	VERIFY(add_extension_function("GetObjectPosY", GetObjectPosY, _check_GetObjectPosY));
	VERIFY(add_extension_function("GetObjectPos", GetObjectPos, _check_GetObjectPos));

	VERIFY(add_extension_function("$System.Graphic",(PEXTENSION_FUNCTION_FUNCTION)System_Graphic,_check_System_Graphic));
	VERIFY(add_extension_function("Graphic.Object",(PEXTENSION_FUNCTION_FUNCTION)Graphic_Object,_check_Graphic_Object));

	VERIFY(add_extension_function("Object.Visible",(PEXTENSION_FUNCTION_FUNCTION)Object_GetVisible,_check_Object_GetVisible));
	//VERIFY(add_extension_sub("Object.Visible", (PEXTENSION_SUB_FUNCTION)Object_SetVisible, _check_Object_SetVisible));

	VERIFY(add_extension_function("Object.AddString", (PEXTENSION_FUNCTION_FUNCTION)Object_GetAddString, _check_Object_GetAddString));
	//VERIFY(add_extension_sub("Object.AddString", (PEXTENSION_SUB_FUNCTION)Object_SetAddString, _check_Object_SetAddString));
}

void c_engine::add_standard_extension_subs()
{
// file

	VERIFY(add_extension_sub("SplitPath",		split_path,		_check_split_path));
	VERIFY(add_extension_sub("ListDir",			list_dir,		_check_list_dir));

// array

	VERIFY(add_extension_sub("NumSort",			NumSort,		_check_NumSort));
	VERIFY(add_extension_sub("AlphaSort",		AlphaSort,		_check_AlphaSort));
	VERIFY(add_extension_sub("Erase",			Erase,			_check_Erase));

// other
//
	VERIFY(add_extension_sub("DbgPrint",		DbgPrint,		_check_DbgPrint));

	//20210402 YYYY-MM-DD HH:MM:SS.MIL 을 찍어보자 DbgPrintMil
	VERIFY(add_extension_sub("DbgPrintMil",		DbgPrintMil,		_check_DbgPrintMil));

	VERIFY(add_extension_sub("DbgLog",			DbgLog,			_check_DbgLog));
	VERIFY(add_extension_sub("DbgBreak",		DbgBreak,		_check_DbgBreak));
	VERIFY(add_extension_sub("StopRun",			Stop,			_check_Stop));
	VERIFY(add_extension_sub("Assert",			Assert,			_check_Assert));

	//...

	//20210406  canvas  는 어떻게 하냐.
}

void c_engine::add_standard_hints()
{
// math

	VERIFY(add_hint("pi",		"pi=3.1415926535897932"));
	VERIFY(add_hint("sqr",		"sqr(d as double) as double"));
	VERIFY(add_hint("hypot",	"hypot(a as double, b as double) as double"));
	VERIFY(add_hint("abs",		"abs(val) as double/integer"));
	VERIFY(add_hint("sin",		"sin(d as double) as double"));
	VERIFY(add_hint("cos",		"cos(d as double) as double"));
	VERIFY(add_hint("tan",		"tan(d as double) as double"));
	VERIFY(add_hint("atan",		"atan(d as double) as double"));
	VERIFY(add_hint("asin",		"asin(d as double) as double"));
	VERIFY(add_hint("acos",		"acos(d as double) as double"));
	VERIFY(add_hint("atan2",	"atan2(a as double, b as double) as double"));
	VERIFY(add_hint("degtorad",	"degtorad(d as double) as double"));
	VERIFY(add_hint("radtodeg",	"radtodeg(d as double) as double"));
	VERIFY(add_hint("exp",		"exp(d as double) as double"));
	VERIFY(add_hint("log",		"log(d as double) as double"));
	VERIFY(add_hint("log10",	"log10(d as double) as double"));

	VERIFY(add_hint("Ceil",		"ceil(d as double) as double"));
	VERIFY(add_hint("Floor",	"floor(d as double) as double"));
	VERIFY(add_hint("Rnd",		"rnd([d as double]) as double"));
	VERIFY(add_hint("Sign",		"sign(d as double) as integer"));

	VERIFY(add_hint("Max",		"Max(val1, val2, ...) as double"));
	VERIFY(add_hint("Min",		"Min(val1, val2, ...) as double"));
	VERIFY(add_hint("Avg",		"Avg(val1, val2, ...) as double"));
	VERIFY(add_hint("Sum",		"Sum(val1, val2, ...) as double"));

	VERIFY(add_hint("Round",	"Round(num As Double, DecimalPlaces As Integer) as Double"));

	VERIFY(add_hint("InvLog",	"InvLog(d As Double) As Double"));
	VERIFY(add_hint("InvLog10",	"InvLog10(d As Double) As Double"));

// string

	VERIFY(add_hint("split",	"Split(S As String, Delim As String) As Array"));
	VERIFY(add_hint("len",		"len(s as string) as integer"));
	VERIFY(add_hint("instr",	"instr(s1 as string, s2 as string) as integer"));
	VERIFY(add_hint("mid",		"mid(s as string, start as integer, length as integer) as string"));
	VERIFY(add_hint("left",		"left(s as string, length as integer) as string"));
	VERIFY(add_hint("right",	"right(s as string, length as integer) as string"));
	VERIFY(add_hint("rtrim",	"rtrim(s as string) as string"));
	VERIFY(add_hint("ltrim",	"ltrim(s as string) as string"));
	VERIFY(add_hint("trim",		"trim(s as string) as string"));
	VERIFY(add_hint("ucase",	"ucase(s as string) as string"));
	VERIFY(add_hint("lcase",	"lcase(s as string) as string"));
	VERIFY(add_hint("chr",		"chr(c as integer, ...) as string"));
	VERIFY(add_hint("asc",		"asc(s as string) as integer"));

// typecast

	VERIFY(add_hint("IsInteger","IsInteger(val) as bool"));
	VERIFY(add_hint("IsDouble",	"IsDouble(val) as bool"));
	VERIFY(add_hint("IsString",	"IsString(val) as bool"));
	VERIFY(add_hint("IsArray",	"IsArray(val) as bool"));
	VERIFY(add_hint("IsObject",	"IsObject(val) as bool"));
	VERIFY(add_hint("IsEmpty",	"IsEmpty(val) as bool"));
	VERIFY(add_hint("IsNull",	"IsNull(val) as bool"));
	VERIFY(add_hint("CInt",		"CInt(val) as integer"));
	VERIFY(add_hint("CDbl",		"CDbl(val) as double"));
	VERIFY(add_hint("CStr",		"CStr(val) as string"));
	VERIFY(add_hint("CDate",	"CDate(val) as date"));

// array

	VERIFY(add_hint("UBound",	"UBound(val[, dimension]) as integer"));
	VERIFY(add_hint("NumDims",	"NumDims(val) as integer"));
	VERIFY(add_hint("Redim",	"Redim(val, dim1, ...) as boolean"));
	VERIFY(add_hint("NumSort",	"NumSort(a as Array[, descending as boolean[, dimension as integer]])"));
	VERIFY(add_hint("Erase",	"Erase(var1, var2, ...)"));

// file

	VERIFY(add_hint("CopyFile",		"CopyFile(source as string, dest as string) as bool"));
	VERIFY(add_hint("MoveFile",		"MoveFile(source as string, dest as string) as bool"));
	VERIFY(add_hint("FileExists",	"FileExists(name as strig) as bool"));
	VERIFY(add_hint("DeleteFile",	"DeleteFile(name as string) as bool"));

	VERIFY(add_hint("OpenFileDlg",	"OpenFileDlg([filter[, name[, dir]]]) as string"));
	VERIFY(add_hint("SaveFileDlg",	"SaveFileDlg([filter[, name[, dir]]]) as string"));
	VERIFY(add_hint("MkDir",		"MkDir(name as string) as bool"));
	VERIFY(add_hint("FileSize",		"FileSize(name as string) as integer"));
	VERIFY(add_hint("DirSize",		"DirSize(name as string) as integer"));
	VERIFY(add_hint("SelectDirDlg",	"SelectDirDlg() as string"));

	VERIFY(add_hint("SplitPath",	"SplitPath(path, drive, dir, name, ext)"));
	VERIFY(add_hint("ListDir",		"ListDir(array, dir[, pattern])"));

	VERIFY(add_hint("GetCurrentDirectory",	"GetCurrentDirectory() As String"));
	VERIFY(add_hint("SetCurrentDirectory",	"SetCurrentDirectory(path As String) As Boolean"));

// date

	VERIFY(add_hint("StrToDate",		"StrToDate(str as string) as date"));
	VERIFY(add_hint("FormatDateTime",	"FormatDateTime(d as date[, format as integer]) as string"));
	VERIFY(add_hint("DateSerial",		"DateSerial(year as integer, month as integer, day as integer) as date"));
	VERIFY(add_hint("TimeSerial",		"TimeSerial(Hour As Integer, Minute As Integer, Second As Integer) As Date"));

	VERIFY(add_hint("Now",			"Now() As Date"));
	VERIFY(add_hint("Year",			"Year(d as date) as integer"));
	VERIFY(add_hint("Month",		"Month(d as date) as integer"));
	VERIFY(add_hint("Day",			"Day(d as date) as integer"));
	VERIFY(add_hint("Hour",			"Hour(d as date) as integer"));
	VERIFY(add_hint("Minute",		"Minute(d as date) as integer"));
	VERIFY(add_hint("Second",		"Second(d as date) as integer"));
	VERIFY(add_hint("DayOfWeek",	"DayOfWeek(d as date) as integer"));
	VERIFY(add_hint("DayOfYear",	"DayOfYear(d as date) as integer"));
	VERIFY(add_hint("OccurDOW",		"OccurDOW(DOW as integer, count as integer[, offset as integer]) as boolean"));

	VERIFY(add_hint("YesterdayIsHolliday",		"YesterdayIsHolliday() as boolean"));
	VERIFY(add_hint("TomorrowIsHolliday",		"TomorrowIsHolliday() as boolean"));
	VERIFY(add_hint("IsHollidayThisWeek",		"IsHollidayThisWeek() as boolean"));

	VERIFY(add_hint("MigrateDate",			"MigrateDate(Date as integer[, USformat as boolean]) as integer"));
	VERIFY(add_hint("MigrateTime",			"MigrateTime(Time as integer) as double"));
	VERIFY(add_hint("TSDateToMigrateDate",	"TSDateToMigrateDate(Date as integer) as integer"));

// other

	VERIFY(add_hint("CreateObject",		"CreateObject(classname As String)"));
	VERIFY(add_hint("MsgBox",			"MsgBox(text as string[, flags as integer[, caption as string]]) as integer"));

// Get Tag Value Function
	//20220715 YDK 수정
	VERIFY(add_hint("GetTagValue",		"GetTagValue(TagName as string"));
	//VERIFY(add_hint("SetAiTagValue", "SetAiTagValue(TagName as string"));
	//20220715 YDK 수정
	VERIFY(add_hint("SetDiTagValue",	"SetDiTagValue(TagName as string, Value"));
	//VERIFY(add_hint("SetDiTagValue", "GetDiTagValue(TagName as string"));

	//20210402 YYYY-MM-DD HH:MM:SS.MIL 을 찍어보자 DbgPrintMil
	VERIFY(add_hint("DbgPrintMil",		"DbgPrintMil(...)"));

	VERIFY(add_hint("DbgPrint",			"DbgPrint(...)"));
	VERIFY(add_hint("DbgLog",			"DbgLog(fileName, message)"));
	VERIFY(add_hint("DbgPreak",			"DbgBreak()"));
	VERIFY(add_hint("IIF",				"IIF(condition, trueVal, falseVal)"));
	VERIFY(add_hint("Eval",				"Eval(expression As String)"));
	VERIFY(add_hint("StopRun",			"StopRun()"));
	VERIFY(add_hint("Assert",			"Assert(condition)"));
	VERIFY(add_hint("Sleep",			"Sleep(Value)"));
// check stuff that

	VERIFY(add_hint("$System.Graphic", "그래픽 도면 객체에 접근합니다. 사용법: $System.Graphic(\"도면명\")"));
	VERIFY(add_hint("Graphic.Object", "특정 도면 내 객체에 접근합니다. 사용법: $System.Graphic(\"도면명\").Object(\"객체명\")"));
	VERIFY(add_hint("Object.Visible", "객체의 가시성을 설정하거나 가져옵니다. 사용법: $System.Graphic(\"도면명\").Object(\"객체명\").Visible = true/false"));

	_ASSERT(m_hint_kwds.get_size() == m_hint_descs.get_size());
	n_default_hints = m_hint_kwds.get_size();
}

void c_engine::list_ext_consts(c_str_array& a)
{
	int i;
	for (i = 0; i < m_ext_consts.names.get_size(); i++)
		a.add(m_ext_consts.names[i]);
}

void c_engine::list_ext_funcs(c_str_array& a)
{
	int i;
	for (i = 0; i < m_ext_funcs.get_size(); i++)
		a.add(m_ext_funcs[i].m_name.get_buffer());
}

void c_engine::list_ext_subs(c_str_array& a)
{
	int i;
	for (i = 0; i < m_ext_subs.get_size(); i++)
		a.add(m_ext_subs[i].m_name.get_buffer());
}

//-----------------------------------------------------------------------------
/*
bool c_engine::set_global_var(const char* p_name, const VARIANT* p_var)
{
	if (!can_be_identifier(p_name, false))
		return false;

	if (p_var->vt == (VT_BYREF | VT_VARIANT)) p_var = (VARIANT*)p_var->byref;

//	if (p_var->vt != VT_I2 &&
//		p_var->vt != VT_INT &&
//		p_var->vt != VT_I4 &&
//		p_var->vt != VT_BOOL &&
//		p_var->vt != VT_R8 &&
//		p_var->vt != VT_BSTR)
//	{
//		error(NO_ERR_LINE, "can set global var only of integer, double or string type. you attempted 0x%8.8X.", p_var->vt);
//		return false;
//	}

	m_call_stack.set_global(p_name, p_var);
	return true;
}

bool c_engine::get_global_var(const char* p_name, VARIANT** pp_var)
{
	return m_call_stack.get_global(p_name, pp_var);
}
*/
c_variable* c_engine::get_extension_constant(const char* p_name)
{
	int i = m_ext_consts.find(p_name);
	if (i == -1) return 0;
	return m_ext_consts.vars.get_ptr(i);
}

bool c_engine::add_extension_constant(LPCTSTR lpszname, c_variable* p_val)
{
// see if it is already added -------------------------------------------------

	int i = m_ext_consts.find(lpszname);
	if (i != -1)
	{
		m_ext_consts.vars[i] = *p_val;
		return true;
	}

// if not, see if it is a valid identifier ------------------------------------

	if (!can_be_identifier(lpszname, false))
		return false;

// and add it -----------------------------------------------------------------

	m_ext_consts.vars.add(*p_val);
	m_ext_consts.names.add(lpszname);

	return true;
}

bool c_engine::add_extension_constant(LPCTSTR lpszname, int val)
{
	c_variable constant;
	constant = val;
	return add_extension_constant(lpszname, &constant);
}

bool c_engine::add_extension_constant(LPCTSTR lpszname, double val)
{
	c_variable constant;
	constant = val;
	return add_extension_constant(lpszname, &constant);
}

bool c_engine::add_extension_constant(LPCTSTR lpszname, LPCTSTR val)
{
	c_variable constant;
	constant = val;
	return add_extension_constant(lpszname, &constant);
}

bool c_engine::add_extension_function(LPCTSTR lpszname,
									 PEXTENSION_FUNCTION_FUNCTION pfunc,
									 CHECK_ROUTINE p_check)
{
	if (!can_be_identifier(lpszname, false))
	{
		_ASSERT(0);	// name duplication
		return false;
	}

	extension_function ext_func(lpszname, pfunc, p_check);
	m_ext_funcs.add(ext_func);
	return true;
}

bool c_engine::add_extension_sub(	LPCTSTR lpszname,
									PEXTENSION_SUB_FUNCTION psub,
									CHECK_ROUTINE p_check)
{
	if (!can_be_identifier(lpszname, false))
	{
		_ASSERT(0);	// name duplication
		return false;
	}

	extension_sub ext_sub(lpszname, psub, p_check);
	m_ext_subs.add(ext_sub);
	return true;

}

//-----------------------------------------------------------------------------
// _dll_caller ----------------------------------------------------------------
//-----------------------------------------------------------------------------

c_dll_caller* c_engine::_dll_caller()
{
	c_dll_func* p_dll_func = get_dll_routine(curtok.m_name.get_buffer(), false);
	_ASSERT(p_dll_func);

// parse args -----------------------------------------------------------------

	c_ptr_array<c_expression> expressions;

	if (!parse_arg_list(expressions,
						p_dll_func->get_routine_name(),
						0,
						false))
	{
		return 0;
	}

// create dll_caller and attach expressions to it -----------------------------

	c_dll_caller* p_caller = new c_dll_caller(this, &m_call_stack);

	p_caller->set_dll_func(p_dll_func);

	int i;
	for (i = 0; i < expressions.get_size(); i++)
		p_caller->add_expr(expressions[i]);

	return p_caller;
}

//-----------------------------------------------------------------------------
// _ext_func ------------------------------------------------------------------
//-----------------------------------------------------------------------------

c_ext_func* c_engine::_ext_func()
{
	c_ext_func* p_ext_func = new c_ext_func(&m_call_stack, this);

//	char func_name[MAX_NAME_LENGTH + 1];
//	strncpy(func_name, curtok.m_name, MAX_NAME_LENGTH);

	extension_function _function;
	bool ret = get_ext_function(curtok.m_name, _function);
	_ASSERT(ret);	// this should not fail

	c_ptr_array<c_expression> expressions;

	if (!parse_arg_list(expressions,
					  curtok.m_name,
					  _function.m_pcheck,
					  false))
	{
		delete p_ext_func;
		return 0;
	}

	for (int i = 0; i < expressions.get_size(); i++)
		p_ext_func->add_expr(expressions[i]);

	p_ext_func->set_function(_function.pfunc);

	return p_ext_func;
}

//-----------------------------------------------------------------------------
// parse_dll_sub --------------------------------------------------------------
//-----------------------------------------------------------------------------

DWORD c_engine::parse_dll_sub(c_vector_table& last, DWORD stop_at)
{
	c_string sub_name;
	sub_name = curtok.m_name;

	c_dll_func* p_func = get_dll_routine(curtok.m_name.get_buffer(), true);
	if (!p_func) p_func = get_dll_routine(curtok.m_name.get_buffer(), false);
	_ASSERT(p_func);

	c_ptr_array<c_expression> expressions;

	if (!parse_arg_list(expressions,
						curtok.m_name,
						0,
						true))
	{
		return ERR_;
	}

	if (p_func->get_arg_count() != expressions.get_size())
	{
		error(CUR_ERR_LINE, "'%s' sub gets %d argument(s). you try to pass %d.",
			sub_name.get_buffer(),
			p_func->get_arg_count(),
			expressions.get_size());

		return ERR_;
	}

// parsed OK. create atom -----------------------------------------------------

	c_dll_sub_atom* p_atom = new c_dll_sub_atom(&m_atom_table, &m_call_stack, this, m_char_stream.cur_line());

	p_atom->set_dll_func(p_func);
	int i;
	for (i = 0; i < expressions.get_size(); i++)
	{
		p_atom->add_expr(expressions[i]);
	}

// finalize -------------------------------------------------------------------

	m_atom_table.add(p_atom);

	while(c_atom** p_last = last.pop())
		*p_last = p_atom;

	last.push(&p_atom->m_pnext);

	gettok();
	if (stop_at == SINGLE_ATOM) return SUCCESS;

	return TO_GO;
}

//-----------------------------------------------------------------------------
// parse_ext_sub --------------------------------------------------------------
//-----------------------------------------------------------------------------

DWORD c_engine::parse_ext_sub(c_vector_table& last, DWORD stop_at)
{
//	c_string sub_name;
//	sub_name = curtok.m_name;

	c_ptr_array<c_expression> expressions;

	extension_sub		_sub;
	extension_function	_func;

	c_ext_sub_atom* p_atom = new c_ext_sub_atom(&m_atom_table, &m_call_stack, this, m_char_stream.cur_line());

	if(get_ext_sub(curtok.m_name, _sub))
	{
		if (!parse_arg_list(expressions,
						  curtok.m_name,
						  _sub.m_pcheck,
						  true))
		{
			delete p_atom;
			return ERR_;
		}

		for (int i = 0; i < expressions.get_size(); i++)
			p_atom->add_expr(expressions[i]);

		p_atom->set_sub(_sub.m_sub);
	}
	else if (get_ext_function(curtok.m_name, _func))
	{
		if (!parse_arg_list(expressions,
						  curtok.m_name,
						  _func.m_pcheck,
						  true))
		{
			delete p_atom;
			return ERR_;
		}

		for (int i = 0; i < expressions.get_size(); i++)
			p_atom->add_expr(expressions[i]);

		p_atom->set_function(_func.pfunc);
	}
	else _ASSERT(0); // this should not normaly fail

	m_atom_table.add(p_atom);

	while(c_atom** p_last = last.pop())
		*p_last = p_atom;

	last.push(&p_atom->m_pnext);

	gettok();
	if (stop_at == SINGLE_ATOM) return SUCCESS;

	return TO_GO;
}

//-----------------------------------------------------------------------------
// parse_arg_list -------------------------------------------------------------
//-----------------------------------------------------------------------------

bool c_engine::parse_arg_list(c_expressions& expressions,
							  const char* p_name,
							  CHECK_ROUTINE p_check,
							  bool b_sub)
{
	int i;
	bool b_lb = false; // argument list started from '('
	gettok();
	c_expression* p_expr;

	// p_name may point to the buffer of curtok.m_name. this buffer will be changed,
	// so we need to save routine name.
	c_string str_name = p_name;
	p_name = str_name.get_buffer();

	b_lb = (curtok.type == token_type::lb);

	if (!b_sub)
		if (curtok.type != token_type::lb && curtok.type != token_type::eos && curtok.type != token_type::eof)
		{
			error(CUR_ERR_LINE, "'(' or end of line expected after '%s'", p_name);
			return false;
		}

	if (!b_sub)
		if (curtok.type != token_type::lb) goto _args_done; // assume function gets 0 arguments

	for (i = 0;;i++)
	{
		if (i != 0 || curtok.type == token_type::lb) gettok();

		if (curtok.type == token_type::rb || curtok.type == token_type::eos)
		{
			if (i == 0) break; // empty '()'
			else
			{
				error(CUR_ERR_LINE, "expression expected, but ')' found");
				return false;
			}
		}

		p_expr = _expr();
		if (!p_expr)
		{
			error(CUR_ERR_LINE, "cannot parse argument #%d of the '%s'",
				expressions.get_size() + 1, p_name);
			expressions.delete_items();
			return false;
		}
		else expressions.add(p_expr);

		switch (curtok.type)
		{
		case token_type::comma: continue;
		case token_type::eof:
		case token_type::eos:
		case token_type::rb: goto _args_done;

		default:
			error(CUR_ERR_LINE, "',' or ')' expected where %s found", curtok.format());
			expressions.delete_items();
			return false;
		}
	}

_args_done: // all expressions parsed. check types.

	if (b_lb)
	{
		if (curtok.type != token_type::rb)
		{
			error(CUR_ERR_LINE, "',' or ')' expected where %s found", curtok.format());
			expressions.delete_items();
			return false;
		}
	}
	else
	{
		//_ASSERT(b_sub);
		if (!b_sub)
		{
			error(CUR_ERR_LINE, "argument list () expected after function '%s'. %s is illegal here", p_name, curtok.format());
			expressions.delete_items();
			return false;
		}

		if (curtok.type != token_type::eos && curtok.type != token_type::eof)
		{
			error(CUR_ERR_LINE, "end of line expected where %s found", curtok.format());
			expressions.delete_items();
			return false;
		}
	}
	//20230608 YDK 수정
	if (p_check)
	{
		c_array<VARENUM> types;
		VARENUM vt;
		for (i = 0; i < expressions.get_size(); i++)
		{
			vt = get_expr_type(expressions[i]);
			types.add(vt);
		}

		c_string msg;
		bool b_ok = p_check(types.get_size(), types.get_data(), &msg, this);
		if (b_ok)
		{

			if (_stricmp(curtok.m_name, "writevalue") == 0 ||
				_stricmp(curtok.m_name, "ToggleDigitalPoint") == 0 ||
				_stricmp(curtok.m_name, "CloseDigitalPoint") == 0 ||
				_stricmp(curtok.m_name, "OpenDigitalPoint") == 0 ||
				_stricmp(curtok.m_name, "DisableAlarm") == 0 ||
				_stricmp(curtok.m_name, "EnableAlarm") == 0 ||
				_stricmp(curtok.m_name, "OffScan") == 0 ||
				_stricmp(curtok.m_name, "OnScan") == 0 ||
				_stricmp(curtok.m_name, "ToggleScan") == 0 ||
				_stricmp(curtok.m_name, "ReadValue") == 0 ||
				_stricmp(curtok.m_name, "RampValue") == 0)
			{
				if (g_pTagCol == NULL)
				{
					error(CUR_ERR_LINE, "\nTagCollector is not initialized.\nFirst, call the ScriptInit function \nand then call the ScriptCompile function.");
					expressions.delete_items();
					return false;
				}
				c_string tagename = "";
				expressions[0]->m_constant.as_string(tagename);
				CString strTemp = tagename;
				if (tagename!="")
				{
					if (!CheckValidTagName(tagename))
					{
						error(CUR_ERR_LINE, "InValid TagName [" + strTemp + "]");
						expressions.delete_items();
						return false;
					}
				}

			}
		}

		if (msg.get_length()) error(CUR_ERR_LINE, msg);
		if (!b_ok)
		{
			expressions.delete_items();
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// get_type_info --------------------------------------------------------------
//-----------------------------------------------------------------------------

ITypeInfo* c_engine::get_type_info(IDispatch* lpd)
{
	ITypeInfo *pti = 0;
	TYPEATTR* pta = 0;
	HRESULT hr;

	if (!lpd) return 0;

	hr = lpd->GetTypeInfo(0, 0, &pti);
	if (FAILED(hr) || !pti) return 0;

	hr = pti->GetTypeAttr(&pta);
	if (FAILED(hr) || ! pta)
	{
		pti->Release();
		return 0;
	}

	if (pta->typekind == TKIND_INTERFACE) // Get the dual
	{
		ITypeInfo* pti_temp = 0;
		HREFTYPE hRef;
		hr = pti->GetRefTypeOfImplType(-1, &hRef);
		if (FAILED(hr))
		{
			pti->ReleaseTypeAttr(pta);
			pti->Release();
			return 0;
		}

		hr = pti->GetRefTypeInfo(hRef, &pti_temp);
		if (FAILED(hr) || !pti_temp)
		{
			pti->ReleaseTypeAttr(pta);
			pti->Release();
			return 0;
		}

		pti->ReleaseTypeAttr(pta);
		pti->Release();
		pti = pti_temp;
	}

	return pti;
}

//-----------------------------------------------------------------------------
// parse_disp_levels ----------------------------------------------------------
//-----------------------------------------------------------------------------

bool c_engine::check_arg_count(FUNCDESC* pfd,
							   c_ptr_array<c_expression>* p_expressions,
							   const char* p_method_name)
{
	c_string method_property;
	if (pfd->invkind == DISPATCH_METHOD) method_property = "method";
	else method_property = "property";

	int n_optional = pfd->cParamsOpt;

	if (!n_optional)
	{
// detect optional arguments

		int i;
		int n = -1;
		for (i = pfd->cParams - 1; i>= 0; i--)
		{
			if (pfd->lprgelemdescParam[i].paramdesc.wParamFlags & PARAMFLAG_FOPT)
				n = i;
			else break;
		}

		if (n != -1)
		{
			n_optional = pfd->cParams - n;
		}
	}

	if (n_optional > 0)
	{
		if (p_expressions->get_size() < pfd->cParams - n_optional)
		{
			error(CUR_ERR_LINE, "%s %s takes not less than %hd parameters",
				method_property.get_buffer(),
				p_method_name,
				pfd->cParams);

			return false;
		}
		if (p_expressions->get_size() > pfd->cParams)
		{
			error(CUR_ERR_LINE, "%s %s takes not more than %hd parameters",
				method_property.get_buffer(),
				p_method_name,
				pfd->cParams + n_optional);

			return false;
		}
	}
	else
	{
		int n_actual_args = p_expressions->get_size();
		if (pfd->invkind == DISPATCH_PROPERTYPUT || pfd->invkind == DISPATCH_PROPERTYPUTREF) n_actual_args ++; // assign expression is also argument

		if (n_actual_args != pfd->cParams)
		{
			error(CUR_ERR_LINE, "%s %s takes %hd parameter(s)",
				method_property.get_buffer(),
				p_method_name,
				pfd->cParams);

			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// get_dispatch_type_info -----------------------------------------------------
//-----------------------------------------------------------------------------

void get_dispatch_type_info(ITypeInfo** ppti, TYPEATTR** ppta)
{
	_ASSERT(ppti && *ppti);
	_ASSERT(ppta && *ppta);

	HRESULT hr;
	int i;
	if ((*ppta)->typekind == TKIND_INTERFACE) // Get the dual
	{
		ITypeInfo* pti_temp;
		HREFTYPE hRef;
		hr = (*ppti)->GetRefTypeOfImplType(-1, &hRef);
		if (!FAILED(hr))
		{
			hr = (*ppti)->GetRefTypeInfo(hRef, &pti_temp);
			_ASSERT(!FAILED(hr));

			(*ppti)->ReleaseTypeAttr(*ppta);
//			(*ppti)->Release();
			*ppti = pti_temp;
			(*ppti)->GetTypeAttr(ppta);
			_ASSERT((*ppta)->typekind == TKIND_DISPATCH);
		}
	}
	else if ((*ppta)->typekind == TKIND_COCLASS)
	{
		bool b_dispatch_found = false;

		for (i = 0; i < (*ppta)->cImplTypes; i++)
		{
			ITypeInfo* pti_temp = 0;
			HREFTYPE hRef = 0;
			TYPEATTR* pta_temp = 0;

			hr = (*ppti)->GetRefTypeOfImplType(i, &hRef);
			if (!FAILED(hr))
			{
				hr = (*ppti)->GetRefTypeInfo(hRef, &pti_temp);
				_ASSERT(!FAILED(hr));

				pti_temp->GetTypeAttr(&pta_temp);
				if (pta_temp->typekind == TKIND_DISPATCH)
				{
					(*ppti)->ReleaseTypeAttr(*ppta);
//					(*ppti)->Release();
					*ppti = pti_temp;
					*ppta = pta_temp;

					b_dispatch_found = true;
					break;
				}
				else
					pti_temp->Release();
			}
		}

		if (!b_dispatch_found)
		{
			// todo: error and exit
			//return false;
		}
	}
}

//-----------------------------------------------------------------------------
// is_property_get_context ----------------------------------------------------
//-----------------------------------------------------------------------------

bool c_engine::is_property_get_context()
{
	bool b_result = true;

	switch (prevtok.type)
	{
	case token_type::power:
	case token_type::multiply:
	case token_type::divide:
	case token_type::reminder:
	case token_type::plus:
	case token_type::minus:
	case token_type::cat:

	case token_type::equal:
	case token_type::notequal:
	case token_type::less:
	case token_type::greater:
	case token_type::lesse:
	case token_type::greatere:

	case token_type::not:
	case token_type::and:
	case token_type::or:
	case token_type::xor:

	case token_type::if_cond:
	case token_type::for_cond:
	case token_type::to_cond:
	case token_type::step_cond:
	case token_type::while_cond:
	case token_type::print_cond:
	case token_type::do_cond:
	case token_type::loop_cond:
	case token_type::until_cond:
	case token_type::select_cond:
	case token_type::case_cond:
		return true;
	}


	PUSH_CURTOK();

	gettok();
	if (curtok.type == token_type::lb)
	{
		int n_depth = 1;
		while (1)
		{
			gettok();

			if (curtok.type == token_type::eos || curtok.type == token_type::eof) break;
			else if (curtok.type == token_type::lb) n_depth ++;
			else if (curtok.type == token_type::rb)
			{
				n_depth --;
				if (n_depth <= 0) break;
			}
		}

		gettok();
	}

	b_result = curtok.type != token_type::equal;

	POP_CURTOK();
	return b_result;
}

//-----------------------------------------------------------------------------
// parse_disp_levels ----------------------------------------------------------
//-----------------------------------------------------------------------------

#ifdef FREE_BEFORE_EXIT
#undef FREE_BEFORE_EXIT
#endif

#define FREE_BEFORE_EXIT() \
if (pta)\
{\
	if (pfd)\
	{\
		pti->ReleaseFuncDesc(pfd);\
		pfd = 0;\
	}\
	pti->ReleaseTypeAttr(pta);\
	pta = 0;\
}\
p_levels->delete_items();\

bool c_engine::parse_disp_levels(c_string* p_parent,
								 c_disp_levels* p_levels,
								 unsigned int n_mask,
								 ITypeInfo* def_pti,
								 bool b_get)
{
	_ASSERT(!p_levels->get_size());

	c_string str_name;
	c_string str_member;

	// get type info from the IDispatch

	CComPtr<ITypeInfo> pti = def_pti;
	FUNCDESC* pfd = 0;
	TYPEATTR* pta = 0;
	HRESULT hr;

	// var name
//	if (curtok.type != dot) gettok();
	*p_parent = prevtok.m_name;
	str_member = prevtok.m_name;

	bool b_default = false; // processing default property
	int i, j, k;
	VARTYPE t = VT_DISPATCH; // type of result returned by current level. DISPATCH for the first level.
	for (i = 0; ; i++)
	{
		// get TYPEATTR from ITypeInfo

		_ASSERT(!pta);//pta = 0;
		hr = pti->GetTypeAttr(&pta);
		if (FAILED(hr) || ! pta)
		{
			error(CUR_ERR_LINE, "cannot get object type attributes.");
			FREE_BEFORE_EXIT()
			return false;
		}

		get_dispatch_type_info(&(pti.p), &pta);

		// iterate through functions to check name

		bool b_found{ 0, };
		long memid = -1;
		_ASSERT(pta->cFuncs);
		for (j = 0; j < pta->cFuncs; j++)
		{
			_ASSERT(!pfd);
			b_found = false;
			pti->GetFuncDesc(j, &pfd);

			if ((pfd->invkind & n_mask) &&
				(pfd->wFuncFlags & (/*FUNCFLAG_FRESTRICTED |*/ FUNCFLAG_FHIDDEN)) == 0
				)
			{
				t = pfd->elemdescFunc.tdesc.vt;

				BSTR pb_name;
				pti->GetDocumentation(pfd->memid, &pb_name, 0, 0, 0);
				str_name = pb_name;
				SysFreeString(pb_name);

				if (b_default) b_found = (pfd->memid == 0);
				else
				{
					if (!_stricmp(str_name, curtok.m_name))
					{
						if (pfd->invkind & DISPATCH_PROPERTYGET)
						{
							if (b_get) b_found = true;
							else b_found = is_property_get_context();
						}
						else b_found = true;
					}
				}

				if (b_found)
				{
					memid = pfd->memid;
					str_member = str_name;
				}
			}

			if (b_found)
			{
				_ASSERT(pfd->funckind & FUNC_DISPATCH);
				_ASSERT(pfd->cParamsOpt != -1); // not implemented. pointer to safearray.
				_ASSERT(memid != -1);

				b_default = false;
				c_expressions expressions;

				PUSH_CURTOK();
				gettok();

				if (curtok.type == token_type::lb)
				{
					// methodor property with arguments
					POP_CURTOK();

					if (pfd->invkind & DISPATCH_PROPERTYGET &&
						pfd->cParams == 0 && t == VT_PTR)
					{
						// trying to call default member. try to find it.

						FUNCDESC* pfd_default = 0;
						for (k = 0; k < pta->cFuncs; k++)
						{
							hr = pti->GetFuncDesc(k, &pfd_default);
							_ASSERT(!FAILED(hr));

							if (pfd_default->memid == 0) b_default = true;

							pti->ReleaseFuncDesc(pfd_default);
							if (b_default) break;
						}

						if (!b_default)
						{
							error(CUR_ERR_LINE, "'(' after %s is illegal.", str_name.get_buffer());
							FREE_BEFORE_EXIT()
							return false;
						}
					}
					else
					{
						if (!parse_arg_list(expressions,
										  curtok.m_name,
										  0,
										  true))
						{
							FREE_BEFORE_EXIT()
							return false;
						}
					}

				}
				else POP_CURTOK();

				if (!check_arg_count(pfd, &expressions, str_name.get_buffer()))
				{
					expressions.delete_items();
					FREE_BEFORE_EXIT()
					return false;
				}

				c_expression* p_assign_expr = 0;
				if (pfd->invkind & DISPATCH_PROPERTYPUT || pfd->invkind & DISPATCH_PROPERTYPUTREF)
				{
					gettok(); // might be '='

					if (curtok.type != token_type::equal)
					{
						error(CUR_ERR_LINE, "'=' expected after property '%s'", str_name);
						FREE_BEFORE_EXIT()
						return false;
					}

					gettok();
					p_assign_expr = _expr();
					if (!p_assign_expr)
					{
						error(CUR_ERR_LINE, "cannot parse expression after '='");
						FREE_BEFORE_EXIT()
						return false;
					}
				}

				c_disp_level* p_disp_level = new c_disp_level(pfd, &m_call_stack);
				p_disp_level->set_member_name(str_name);
				if (p_assign_expr) p_disp_level->set_assign_expr(p_assign_expr);

				for (k = 0; k < expressions.get_size(); k++)
					p_disp_level->add_expr(expressions[k]);

				p_levels->add(p_disp_level);
			}

			HREFTYPE hrftp = 0;
			if (t == VT_PTR)
				if (pfd->elemdescFunc.tdesc.lptdesc)
					hrftp = pfd->elemdescFunc.tdesc.lptdesc->hreftype;

			pti->ReleaseFuncDesc(pfd);
			pfd = 0;

			if (b_found)
			{
				// member can also be of object type. if so, get its type info.
				if (t == VT_PTR)
				{
					ITypeInfo* pti_ref;
					hr = pti->GetRefTypeInfo(hrftp, &pti_ref);
					if (!FAILED(hr) && pti_ref)
					{
						pti->ReleaseTypeAttr(pta);
						pta = 0;
						pti = pti_ref;
						t = VT_DISPATCH;
					}
					else
					{
						pti = 0;
					}
				}
				break;
			}
		}

		if (pta) pti->ReleaseTypeAttr(pta);
		pta = 0;

		if (!b_found)
		{
			error(CUR_ERR_LINE, "'%s' is not a property or method of '%s'",
				curtok.m_name.get_buffer(), str_member.get_buffer());
			FREE_BEFORE_EXIT()
			return false;
		}

		// test for '.'
		//int n_pos = m_char_stream.pos();
		PUSH_CURTOK();
		gettok();
		if (curtok.type == token_type::dot)
		{
			gettok(); // get next member name
			if (!curtok.disp_name())
			{
				error(CUR_ERR_LINE, "member name expected after '.'. '%s' unexpected here.", curtok.format());
				FREE_BEFORE_EXIT()
				return false;
			}
		}
		else if (curtok.type == token_type::lb && b_default && t == VT_DISPATCH)
		{
			// default member. continue.
			POP_CURTOK();
		}
		else
		{
//			if (curtok.type != eof && curtok.type != eos)
			{
				//m_char_stream.pos(n_pos);
				//curtok = prevtok;
				POP_CURTOK();
			}
			break;
		}

		// there was '.' after non-object property.
		if (t != VT_DISPATCH)
		{
			_ASSERT(str_name.get_length());
			error(CUR_ERR_LINE, "'%s' is not of object type. '.' unexpected here.", str_name);
			FREE_BEFORE_EXIT()
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// disp_get -------------------------------------------------------------------
//-----------------------------------------------------------------------------

c_disp_get* c_engine::disp_get(bool& b_var, ITypeInfo* def_pti)
{
	b_var = false;

	CComPtr<ITypeInfo> pti = def_pti;
	CComPtr<IDispatch> lpd;

	bool b_is_object_variable = is_object_variable(curtok.m_name.get_buffer(), 0, 0);

	if (!pti)
	{
		lpd = get_object(curtok.m_name);
		_ASSERT(lpd);
		pti = get_type_info(lpd);
		_ASSERT(pti);
	}

	if (!lpd)
	{
		lpd = get_default_parent(curtok.m_name.get_buffer());
		_ASSERT(lpd || def_pti);
	}

	if (!def_pti || b_is_object_variable)
	{
		if (m_char_stream.check() != '.')
		{
			b_var = true;
			return false;
		}

		gettok();
		if (curtok.type != token_type::dot)
		{
			error(CUR_ERR_LINE, "'.' expected after '%s'", curtok.m_name);
			return false;
		}

		gettok();
		if (!curtok.disp_name())
		{
			error(CUR_ERR_LINE, "member property or method expected after '%s'", prevtok.m_name);
			return false;
		}
	}

	c_disp_levels levels;
	c_string parent_var;
	if (!parse_disp_levels(&parent_var,
						   &levels,
						   (DISPATCH_PROPERTYGET | DISPATCH_METHOD),
						   pti,
						   true))
	{
		levels.delete_items();
		return 0;
	}

	_ASSERT(levels.get_size());
	c_disp_level* p_last = levels.get_last();

	if (p_last->is_sub())
	{
		error (CUR_ERR_LINE, "sub '%s' cannot be used in expression.", p_last->get_member_name());
		levels.delete_items();
		return 0;
	}

	c_disp_get* p_get = new c_disp_get(&m_call_stack, this);

	p_get->set_lpdispatch(lpd, pti);
	if (b_is_object_variable)
	{
		_ASSERT(is_object_variable(parent_var.get_buffer(), 0, 0));
		_ASSERT(can_be_identifier(parent_var.get_buffer(), false));
		p_get->set_obj_var_name(parent_var.get_buffer());
	}

	int i;
	for (i = 0; i < levels.get_size(); i++)
		p_get->add_disp_level(levels[i]);

	return p_get;
}

//-----------------------------------------------------------------------------
// parse_disp_set -------------------------------------------------------------
//-----------------------------------------------------------------------------
// lpd != 0 if we parse member of the object added with default = true flag.

DWORD c_engine::parse_disp_set(bool* p_bvar, c_vector_table& last, DWORD stop_at, ITypeInfo* def_pti)
{
	CComPtr<ITypeInfo> pti = def_pti;
	CComPtr<IDispatch> lpd;
	bool b_is_object_variable = is_object_variable(curtok.m_name.get_buffer(), 0, 0);

	if (p_bvar) *p_bvar = false;

	if (!pti)
	{
		lpd = get_object(curtok.m_name);
		_ASSERT(lpd);
		pti = get_type_info(lpd);
		_ASSERT(pti);
	}

	if (!lpd)
	{
		lpd = get_default_parent(curtok.m_name.get_buffer());
		_ASSERT(lpd || def_pti);
	}

	if (!def_pti || b_is_object_variable)
	{
		//int n_pos = m_char_stream.pos();
		PUSH_CURTOK();

		gettok();

		if (curtok.type == token_type::equal)
		{
			//curtok = prevtok;
			//m_char_stream.pos(n_pos);
			POP_CURTOK();

			if (p_bvar) *p_bvar = true;
			return ERR_;
		}

		if (curtok.type != token_type::dot)
		{
			error(CUR_ERR_LINE, "'.' expected after '%s'", curtok.m_name);
			return ERR_;
		}

		gettok();
		if (!curtok.disp_name())
		{
			error(CUR_ERR_LINE, "member property or method expected after '%s'", prevtok.m_name);
			return ERR_;
		}
	}

	int n_line = m_char_stream.cur_line();
	c_disp_levels levels;
	c_string parent_var;
	if (!parse_disp_levels(&parent_var,
		&levels,
		(DISPATCH_PROPERTYPUT | DISPATCH_METHOD | DISPATCH_PROPERTYPUTREF | DISPATCH_PROPERTYGET),
		pti,
		false))
	{
		levels.delete_items();
		return ERR_;
	}

	if (levels.get_size())
	{
		c_disp_level* p_last = levels[levels.get_size() - 1];
		if (!p_last->can_be_used_in_set())
		{
			error(CUR_ERR_LINE, "object expression cannot be used in assign/call context.");
			return ERR_;
		}
	}

	c_disp_set_atom* p_atom = new c_disp_set_atom(	&m_atom_table,
													&m_call_stack,
													this,
													n_line);

	p_atom->set_lpdispatch(lpd, pti);
	if (b_is_object_variable)
	{
		_ASSERT(is_object_variable(parent_var.get_buffer(), 0, 0));
		_ASSERT(can_be_identifier(parent_var.get_buffer(), false));
		p_atom->set_obj_var_name(parent_var.get_buffer());
	}

	int i;
	for (i = 0; i < levels.get_size(); i++)
		p_atom->add_disp_level(levels[i]);

// finalize ------------------------------------------------------------------

	m_atom_table.add(p_atom);

	while(c_atom** p_last = last.pop())
		*p_last = p_atom;

	last.push(&p_atom->m_pnext);

	if (stop_at == SINGLE_ATOM)
	{
		if (curtok.type == token_type::rb) gettok();
		return SUCCESS;
	}

	gettok();
	return TO_GO;
}