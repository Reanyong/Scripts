#include "extensions.h"

void __stdcall Len		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall InStr	(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall Mid		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall Right	(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall Left		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall RTrim	(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall LTrim	(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall Trim		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall UCase	(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall LCase	(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall Chr		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall Asc		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall Split	(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

// check functions ------------------------------------------------------------

bool _check_Len(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_InStr(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_Mid(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_Right(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_Left(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_RTrim(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_LTrim(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_Trim(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_UCase(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_LCase(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_Chr(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_Asc(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_Split(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
