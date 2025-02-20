#include "extensions.h"

void __stdcall IsInteger	(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall IsDouble		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall IsString		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall IsArray		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall IsObject		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall IsEmpty		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall IsNull		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

void __stdcall CInt			(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall CDbl			(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall CStr			(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall CDate		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

// check functions ------------------------------------------------------------

bool std_cast_check(int n,
					VARENUM* p_types,
					c_string* p_msg,
					const char* p_funcname);

bool _check_IsInteger		(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_IsDouble		(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_IsString		(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_IsArray			(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_IsObject		(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_IsEmpty			(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_IsNull			(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);

bool _check_CInt			(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_CDbl			(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_CStr			(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_CDate			(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
