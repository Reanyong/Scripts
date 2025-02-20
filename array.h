#include "extensions.h"

void __stdcall UBound		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall NumDims		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall ReDim		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall NumSort		(int nargs, c_variable** pargs, c_engine* p_engine);
void __stdcall AlphaSort	(int nargs, c_variable** pargs, c_engine* p_engine);
void __stdcall Erase		(int nargs, c_variable** pargs, c_engine* p_engine);
void __stdcall ArrCount		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
// check functions ------------------------------------------------------------

bool _check_UBound			(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_ReDim			(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_NumDims			(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_NumSort			(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_AlphaSort		(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_Erase			(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_ArrCount		(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
