#ifndef DATE_H
#define DATE_H

#include "extensions.h"

void __stdcall StrToDate		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall FormatDateTime	(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall DateSerial		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall TimeSerial		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

void __stdcall Now				(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall Year				(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall Month			(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall DayOfMonth		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall Hour				(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall Minute			(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall Second			(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall DayOfWeek		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall DayOfYear		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall WeekOfYear		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall WeekOfYear2		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

// check functions ------------------------------------------------------------

bool _check_StrToDate			(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_FormatDateTime		(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_DateSerial			(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_TimeSerial			(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);

bool _check_Now					(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_Year				(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_Month				(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_DayOfMonth			(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_Hour				(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_Minute				(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_Second				(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_DayOfWeek			(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_DayOfYear			(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_WeekOfYear			(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_WeekOfYear2			(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);

#endif