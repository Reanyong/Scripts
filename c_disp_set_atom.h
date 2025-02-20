#pragma once
#include <Atlbase.h>

#include "c_variable.h"
#include "c_string.h"
#include "c_expression.h"
#include "c_call_stack.h"
#include "c_disp_level.h"
#include "c_atom.h"

class c_engine;

class c_disp_set_atom : public c_atom
{
	CComPtr<IDispatch>		m_lpd;
	CComPtr<ITypeInfo>		m_pti;

	c_disp_levels	m_disp_levels;
	c_var_desc		m_obj_var;

public:
	c_disp_set_atom(c_atom_table* ptable,
		c_call_stack* p_call_stack,
		c_engine* p_engine,
		int nline);

	~c_disp_set_atom();

	inline void set_obj_var_name(const char* p_name);

	inline void set_lpdispatch(IDispatch* lpd, ITypeInfo* pti);
	inline void add_disp_level(c_disp_level* p_disp_level);

	virtual void prepare_func_helper();
	virtual void exec_();
	virtual void enum_expressions(c_ptr_array<c_expression>* p_expressions);

#ifdef _DEBUG
	virtual void dump(FILE* f);
	virtual void assert_valid();
#endif
};

// inlines --------------------------------------------------------------------

inline void c_disp_set_atom::set_obj_var_name(const char* p_name)
{
	_ASSERT(!m_obj_var.inited());
	_ASSERT(p_name);

	m_obj_var.set_name(p_name);
}

inline void c_disp_set_atom::set_lpdispatch(IDispatch* lpd, ITypeInfo* pti)
{
	_ASSERT(lpd || pti);
	_ASSERT(!m_lpd && !m_pti);

	m_lpd = lpd;
	m_pti = pti;
}

inline void c_disp_set_atom::add_disp_level(c_disp_level* p_disp_level)
{
	m_disp_levels.add(p_disp_level);
}




