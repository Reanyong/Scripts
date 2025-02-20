#ifndef FILE_EXTENSIONS_H
#define FILE_EXTENSIONS_H

#include <stdio.h>
#include "c_string.h"
#include "c_variable.h"

void __stdcall copy_file	(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall move_file	(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall file_exists	(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall delete_file	(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall open_file_dlg(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall save_file_dlg(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall mk_dir		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall file_size	(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall dir_size		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall select_dir_dlg(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

void __stdcall split_path	(int nargs, c_variable** pargs, c_engine* p_engine);
void __stdcall list_dir		(int nargs, c_variable** pargs, c_engine* p_engine);

void __stdcall get_cur_dir(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall set_cur_dir(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

// check functions ------------------------------------------------------------

bool _check_copy_file(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_move_file(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_file_exists(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_delete_file(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_open_file_dlg(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_save_file_dlg(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_mk_dir(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_file_size(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_dir_size(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_select_dir_dlg(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);

bool _check_split_path(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_list_dir(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);

bool _check_get_cur_dir(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_set_cur_dir(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);

#endif