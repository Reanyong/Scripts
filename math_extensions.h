//#include "..\c_extension_function_table.h"
#include "extensions.h"

void __stdcall sqr	(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall hypot(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall abs	(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

void __stdcall sin	(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall cos	(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall tan	(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall atan	(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall asin	(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall acos	(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall atan2(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

void __stdcall RadToDeg(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall DegToRad(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

void __stdcall exp	(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall log	(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall log10(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

void __stdcall ceil(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall floor(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall rnd(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall sign(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

void __stdcall bee_max(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall bee_min(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall bee_avg(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall bee_sum(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

void __stdcall round(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

// check functions ------------------------------------------------------------

bool std_math_check(int n,
					VARENUM* p_types,
					c_string* p_msg,
					const char* p_funcname);

bool _check_sqr(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_hypot(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_abs(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);

bool _check_sin(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_cos(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_tan(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_atan(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_asin(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_acos(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_atan2(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);

bool _check_RadToDeg(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_DegToRad(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);

bool _check_exp(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_log(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_log10(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);

bool _check_ceil(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_floor(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_rnd(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_sign(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);

bool _check_max(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_min(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_avg(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_sum(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);

bool _check_round(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);


// Murray request
void __stdcall InvLog	(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall InvLog10	(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_InvLog		(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_InvLog10	(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);


