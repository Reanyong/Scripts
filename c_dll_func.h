#pragma once
#include "c_string.h"
#include "c_array.h"
#include "c_variable.h"
#include "c_ptr_array.h"
#include "c_expression.h"

//-----------------------------------------------------------------------------
// c_dll_func -----------------------------------------------------------------
//-----------------------------------------------------------------------------

class c_engine;
class c_call_stack;

class c_dll_func
{
	HMODULE				m_hmodule;
	FARPROC				m_fp;

	c_string			m_dll_name;
	c_string			m_routine_name;	// inside DLL
	c_string			m_alias_name;	// inside script

	c_array<VARTYPE>	m_arg_types;
	c_array<bool>		m_byref_args;
	VARTYPE				m_ret_type;

public:

	c_dll_func();
	~c_dll_func();

	void operator = (c_dll_func& d);

	bool load(c_string* p_err);
	void unload();

	inline void set_dll_name(const char* p);
	inline const char* get_dll_name();

	inline void set_routine_name(const char* p);
	inline const char* get_routine_name();

	inline void set_alias_name(const char* p);
	inline const char* get_alias_name();

	inline void add_arg(VARTYPE vt, bool b_by_ref);
	inline void set_type(VARTYPE vt);

	inline int get_arg_count();
	inline bool is_sub();

	bool exec(int n_args, c_variable** p_args, c_variable* p_result);

#ifdef _DEBUG
	void assert_valid();
#endif
};

// inlines --------------------------------------------------------------------

inline void c_dll_func::set_dll_name(const char* p)
{
	_ASSERT(!m_dll_name.get_length());
	m_dll_name = p;
}

inline const char* c_dll_func::get_dll_name()
{
	return m_dll_name.get_buffer();
}

inline void c_dll_func::set_routine_name(const char* p)
{
	_ASSERT(!m_routine_name.get_length());
	m_routine_name = p;
}

inline const char* c_dll_func::get_routine_name()
{
	return m_routine_name.get_buffer();
}

inline void c_dll_func::set_alias_name(const char* p)
{
	_ASSERT(!m_alias_name.get_length());
	m_alias_name = p;
}

inline const char* c_dll_func::get_alias_name()
{
	return m_alias_name.get_buffer();
}

inline void c_dll_func::add_arg(VARTYPE vt, bool b_by_ref)
{
	m_arg_types.add(vt);
	m_byref_args.add(b_by_ref);
}

inline void c_dll_func::set_type(VARTYPE vt)
{
	m_ret_type = vt;
}

inline int c_dll_func::get_arg_count()
{
	return m_arg_types.get_size();
}

inline bool c_dll_func::is_sub()
{
	return m_ret_type == VT_EMPTY;
}

//-----------------------------------------------------------------------------
// c_dll_caller ---------------------------------------------------------------
//-----------------------------------------------------------------------------

class c_dll_caller
{
	c_dll_func*					m_pdll_func;
	c_ptr_array<c_expression>	m_expressions;
	c_engine*					m_pengine;
	c_call_stack*				m_pcall_stack;
public:

	c_dll_caller(c_engine* p_engine, c_call_stack* p_call_stack)
	{
		m_pengine		= p_engine;
		m_pcall_stack	= p_call_stack;
		m_pdll_func		= 0;
	}

	~c_dll_caller()
	{
		m_expressions.delete_items();
	}

	void set_dll_func(c_dll_func* p_dll_func)
	{
		_ASSERT(p_dll_func);
		_ASSERT(!m_pdll_func);
		m_pdll_func = p_dll_func;
	}

	void add_expr(c_expression* p_expr)
	{
		_ASSERT(!m_expressions.contains(p_expr));
		m_expressions.add(p_expr);
	}

	void exec(c_variable* p_result);
};

