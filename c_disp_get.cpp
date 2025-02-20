
#include "pch.h"
#include "c_disp_get.h"

c_disp_get::c_disp_get(c_call_stack* p_call_stack, c_engine* p_engine)
{
	m_pcall_stack	= p_call_stack;
	m_pengine		= p_engine;
//	m_lpd			= 0;
}

c_disp_get::~c_disp_get()
{
	m_disp_levels.delete_items();
}

void c_disp_get::exec(c_variable* p_result)
{
	IDispatch* lpd = 0;

	if (m_obj_var.inited())
	{
		// this is not extension object. this is object variable.

		_ASSERT(!m_lpd);

		c_variable* p_obj_var = 0;

		m_pcall_stack->get(&m_obj_var, &p_obj_var);
		_ASSERT(p_obj_var);

		c_variable* p_target = p_obj_var->get_scalar_ptr();

		if (p_target->vt != VT_DISPATCH)
		{
			_ASSERT(0);// for debug. make sure that runtime error was reported before this.

			_ASSERT(p_target->vt == VT_EMPTY);
			p_target->vt = VT_DISPATCH;
			p_target->pdispVal = 0;

			return;
		}

		lpd = p_target->pdispVal;
	}
	else
		lpd = m_lpd;

	if (!m_disp_levels[0]->get(lpd, p_result, m_pengine, -1)) return;

	// exec all other levels
	for (int i = 1; i < m_disp_levels.get_size(); i++)
	{
		_ASSERT(p_result->vt == VT_DISPATCH);
		if (p_result->vt != VT_DISPATCH) break;
		if (!m_disp_levels[i]->get(p_result->pdispVal, p_result, m_pengine, -1)) return;
	}
}
