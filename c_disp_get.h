#pragma once
#include "c_variable.h"
#include "c_string.h"
#include "c_call_stack.h"
#include "c_disp_level.h"

class c_disp_get
{
	c_call_stack*	m_pcall_stack;
	c_engine*		m_pengine;
	c_disp_levels	m_disp_levels;
	c_var_desc		m_obj_var;


	CComPtr<IDispatch>	m_lpd;
	CComPtr<ITypeInfo>	m_pti;
public:
	c_disp_get(c_call_stack* p_call_stack, c_engine* p_engine);
	~c_disp_get();

	inline void set_obj_var_name(const char* p_name);
	inline void add_disp_level(c_disp_level* p_disp_level);
	inline void set_lpdispatch(IDispatch* lpd, ITypeInfo* pti);
	void exec(c_variable* p_result);

	friend class c_func_caller;
	inline bool is_sub();
};

// inlines --------------------------------------------------------------------

inline void c_disp_get::add_disp_level(c_disp_level* p_disp_level)
{
	m_disp_levels.add(p_disp_level);
}

inline void c_disp_get::set_lpdispatch(IDispatch* lpd, ITypeInfo* pti)
{
	_ASSERT(lpd || pti);
	_ASSERT(!m_lpd && !m_pti);

	m_lpd = lpd;
	m_pti = pti;
}

inline void c_disp_get::set_obj_var_name(const char* p_name)
{
	_ASSERT(!m_obj_var.inited());
	_ASSERT(p_name);

	m_obj_var.set_name(p_name);
}


inline bool c_disp_get::is_sub()
{
	_ASSERT(m_disp_levels.get_size());
	return m_disp_levels[0]->is_sub();
}
