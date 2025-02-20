#include "pch.h"

#include <crtdbg.h>
#include "c_for_atom.h"
#include "c_engine.h"

c_for_atom::c_for_atom(c_atom_table* ptable,
					   c_call_stack* p_call_stack,
					   c_engine* p_engine,
					   int nline)

:c_atom(ptable, p_call_stack, p_engine, nline)
{
	m_type			= atom_type::for_atom;
	m_ploop			= 0;
	m_pexit			= 0;
	m_pengine		= p_engine;
	m_pcall_stack	= p_call_stack;
	m_pinit_expr	= 0;
	m_pfinal_expr	= 0;
	m_pstep_expr	= 0;
	m_bfirst_pass	= true;
	m_bcount_down	= false;
}

c_for_atom::~c_for_atom()
{
	if (m_pinit_expr  != 0) delete m_pinit_expr;
	if (m_pfinal_expr != 0) delete m_pfinal_expr;
	if (m_pstep_expr  != 0) delete m_pstep_expr;
}

void c_for_atom::set_var_name(LPCTSTR varname)
{
	m_var_desc = varname;
}

void c_for_atom::set_init_expr (c_expression* pinit_expr)
{
	m_pinit_expr = pinit_expr;
}

void c_for_atom::set_final_expr(c_expression* pfinal_expr)
{
	m_pfinal_expr = pfinal_expr;
}

void c_for_atom::set_step_expr (c_expression* pstep_expr)
{
	m_pstep_expr = pstep_expr;
}

void c_for_atom::exec_()
{
//	_ASSERT(this != m_ploop);
	_ASSERT(this != m_pexit);
	_ASSERT(m_ploop != m_pexit);
	_ASSERT(m_var_desc.inited());
	_ASSERT(m_pfinal_expr != 0);
	_ASSERT(m_pinit_expr != 0);

	if (this == m_ploop)				// empty loop. exit immediately
	{
		m_ptable->set_cur_atom(m_pexit);
		return;
	}
	
	static c_variable temp_val;			// static helper to increase performance

	if (m_bfirst_pass)
	{
		m_pinit_expr->exec(&temp_val);
		m_pcall_stack->set_var(&m_var_desc, &temp_val);
		m_pfinal_expr->exec(&m_final_val);

		if (m_pstep_expr != 0)
		{
			m_pstep_expr->exec(&m_step_val);
			if (m_step_val < c_variable(0)) m_bcount_down = true;

			// detect possible endless loop on integer variables
			VARENUM counter_type;
			if (m_pcall_stack->declared(m_var_desc.get_name(), &counter_type))
			{
				if ((counter_type == VT_I4 || counter_type == VT_I2) &&
					(m_step_val.vt == VT_R8))
				{
					int i = temp_val.as_integer();
					double s = m_step_val.as_double();
					if (int(i + s) == i)
					{
						c_string msg;
						msg = "integer loop counter '";
						msg += m_var_desc.get_name();
						msg += "' cannot be changed by double increment. endless loop.";

						m_pengine->runtime_error(CUR_ERR_LINE, msg.get_buffer());
					}
				}
			}
		}
		else
		{
			if (temp_val > m_final_val)
			{
//				if (m_pinit_expr->m_action == _const && m_pfinal_expr->m_action == _const)
//				{
//					m_bcount_down = true;
//					m_step_val = -1;
//				}
			}
			else m_step_val = 1;
		}

		m_bfirst_pass = false;

		if (!m_bcount_down)
		{
			if (temp_val <= m_final_val) m_ptable->set_cur_atom(m_ploop);	// go inside loop
			else															// don't enter loop
			{
				m_ptable->set_cur_atom(m_pexit);
				m_bfirst_pass = true;
				m_bcount_down = false;
			}
		}
		else
		{
			if (temp_val >= m_final_val) m_ptable->set_cur_atom(m_ploop);	// go inside loop
			else															// don't enter loop
			{
				m_ptable->set_cur_atom(m_pexit);
				m_bfirst_pass = true;
				m_bcount_down = false;
			}
		}
	}
	else
	{
		c_variable* p_new_val;
		m_pcall_stack->get(&m_var_desc, &p_new_val);
		temp_val = *p_new_val;
		temp_val += m_step_val;
		m_pcall_stack->set_var(&m_var_desc, &temp_val);

		if (!m_bcount_down)
		{
			if (temp_val > m_final_val)
			{
				m_ptable->set_cur_atom(m_pexit);
				m_bfirst_pass = true;
				m_bcount_down = false;
			}
			else m_ptable->set_cur_atom(m_ploop);
		}
		else
		{
			if (temp_val < m_final_val)
			{
				m_ptable->set_cur_atom(m_pexit);
				m_bfirst_pass = true;
				m_bcount_down = false;
			}
			else m_ptable->set_cur_atom(m_ploop);
		}
	}
}

void c_for_atom::prepare_func_helper()
{
	if (m_pinit_expr)	m_func_caller.from_expr(m_pinit_expr);
	if (m_pfinal_expr)	m_func_caller.from_expr(m_pfinal_expr);
	if (m_pstep_expr)	m_func_caller.from_expr(m_pstep_expr);
}

void c_for_atom::enum_expressions(c_ptr_array<c_expression>* p_expressions)
{
	if (m_pinit_expr)	p_expressions->add(m_pinit_expr);
	if (m_pfinal_expr)	p_expressions->add(m_pfinal_expr);
	if (m_pstep_expr)	p_expressions->add(m_pstep_expr);
}

#ifdef _DEBUG
void c_for_atom::dump(FILE* f)
{
	fprintf(f,
"--------------------------------------\n\
type =\t\tfor\n\
m_varname =\t%s\n\
this =\t\t%llX\n\
m_ploop = \t%llX\n\
m_pexit = \t%llX\n\
line =\t\t%d\n",
		m_var_desc.get_name(),
		(UINT64)this,
		(UINT64)m_ploop,
		(UINT64)m_pexit,
		m_nline);

	m_func_caller.dump(f);
}
#endif

#ifdef _DEBUG
void c_for_atom::assert_valid()
{
	_ASSERT(m_pinit_expr);
	m_pinit_expr->assert_valid();

	_ASSERT(m_pfinal_expr);
	m_pfinal_expr->assert_valid();

	if (m_pstep_expr) m_pstep_expr->assert_valid();

	m_final_val.assert_valid();
	m_step_val.assert_valid();

	if (m_ploop) m_ploop->assert_valid();
	if (m_pexit) m_pexit->assert_valid();

	c_atom::assert_valid();
}
#endif
