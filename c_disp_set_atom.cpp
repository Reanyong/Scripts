#include "pch.h"

#include <crtdbg.h>
#include "c_disp_set_atom.h"
#include "c_engine.h"

//-----------------------------------------------------------------------------
// construction/destruction ---------------------------------------------------
//-----------------------------------------------------------------------------

c_disp_set_atom::c_disp_set_atom(c_atom_table* ptable,
								 c_call_stack* p_call_stack,
								 c_engine* p_engine,
								 int nline)

:c_atom(ptable, p_call_stack, p_engine, nline)
{
//	m_lpd			= 0;
//	m_pti			= 0;
	m_type			= atom_type::disp_set_atom;
	m_pcall_stack	= p_call_stack;
	m_pnext			= 0;
	m_pengine		= p_engine;
}

c_disp_set_atom::~c_disp_set_atom()
{
	m_disp_levels.delete_items();
}

//-----------------------------------------------------------------------------
// exec -----------------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_disp_set_atom::exec_()
{
	c_variable v;
	IDispatch* lpd = 0;

	_ASSERT(m_ptable);

	if (m_obj_var.inited())
	{
		// this is not extension object. this is object variable.

		_ASSERT(!m_lpd);

		c_variable* p_obj_var = 0;

		m_pcall_stack->get(&m_obj_var, &p_obj_var);
		_ASSERT(p_obj_var);

		c_variable* p_target = p_obj_var->get_scalar_ptr();
		_ASSERT(p_target->vt == VT_DISPATCH);

		lpd = p_target->pdispVal;
	}
	else
		lpd = m_lpd;

	if (!lpd)
	{
		m_pengine->runtime_error(CUR_ERR_LINE, "trying to call method of NULL object. please set object variable or create object first.");
	}
	else
	{
		int i;
		for (i = 0; i < m_disp_levels.get_size() - 1; i++)
		{
			v.reset();
			if (!m_disp_levels[i]->get(lpd, &v, m_pengine, m_nline)) return;
			_ASSERT(v.vt == VT_DISPATCH);
			if (v.vt != VT_DISPATCH) break;
			lpd = v.pdispVal;
		}

		m_disp_levels[i]->set(lpd, m_pengine, m_nline);
	}

	m_ptable->set_cur_atom(m_pnext);
}

void c_disp_set_atom::prepare_func_helper()
{
	int i, j;
	for (i = 0; i < m_disp_levels.get_size(); i++)
	{
		for (j = 0; j < m_disp_levels[i]->m_expressions.get_size(); j++)
			m_func_caller.from_expr(m_disp_levels[i]->m_expressions.get(j));
	}
}

void c_disp_set_atom::enum_expressions(c_ptr_array<c_expression>* p_expressions)
{
	int i, j;
	for (i = 0; i < m_disp_levels.get_size(); i++)
	{
		for (j = 0; j < m_disp_levels[i]->m_expressions.get_size(); j++)
			p_expressions->add(m_disp_levels[i]->m_expressions.get(j));
	}
}

//-----------------------------------------------------------------------------
// dump -----------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifdef _DEBUG
void c_disp_set_atom::dump(FILE* f)
{
	fprintf(f,
"--------------------------------------\n\
type =\t\tDISP SET\n\
this =\t\t%llX\n\
m_pnext = \t%llX\n\
line =\t\t%d\n",
		(UINT64)this,
		(UINT64)m_pnext,
		m_nline);

	m_func_caller.dump(f);
}

#endif

#ifdef _DEBUG
void c_disp_set_atom::assert_valid()
{
	int i;
	for (i = 0; i < m_disp_levels.get_size(); i++)
		m_disp_levels[i]->assert_valid();

	c_atom::assert_valid();
}
#endif
