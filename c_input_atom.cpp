#include "pch.h"

#include <crtdbg.h>
#include "c_input_atom.h"
#include "c_engine.h"

//extern c_engine* __p_engine;

c_input_atom::c_input_atom(c_atom_table* ptable,
						   c_call_stack* p_call_stack,
						   c_engine* p_engine,
						   int nline)

:c_atom(ptable, p_call_stack, p_engine, nline)
{
	m_pengine		= p_engine;
	m_type			= atom_type::input_atom;
	m_pnext			= 0;
	m_pcall_stack	= p_call_stack;

	m_prev_run_mode	= run_mode::run_undef;
}

c_input_atom::~c_input_atom()
{
}

void c_input_atom::set_string(LPCTSTR pstr)
{
	m_string = pstr;
}

void c_input_atom::add_var(LPCTSTR pvar_name)
{
	c_var_desc* p_var_desc = m_var_desc_array.add_new();
	*p_var_desc = pvar_name;
}

void c_input_atom::exec_()
{
	m_ptable->m_binput = true;		// can't step on input

	c_terminal* p_terminal = m_pengine->get_terminal();
	_ASSERT(p_terminal);
	p_terminal->show();
	p_terminal->append(m_string);
	p_terminal->request_input();
	SetFocus(m_pengine->get_terminal_HWND());
	m_prev_run_mode = m_ptable->get_run_mode();
}

void c_input_atom::on_terminal_enter()
{
	m_ptable->m_binput = false;

	c_terminal* p_terminal = m_pengine->get_terminal();
	_ASSERT(p_terminal);

	c_variable v;
	LPTSTR presult = _strdup((LPTSTR)p_terminal->get_input_string());
	LPTSTR p_alloc = presult;
	if (m_var_desc_array.get_size() != 0)
	{
		LPCTSTR pcur_string = presult;	// pointer to curent string

		int n_var = 0;					// current c_variable

		do
		{
			if (n_var == m_var_desc_array.get_size() - 1) // last var gets the rest of the string
			{
				v.from_input(pcur_string);
				m_pcall_stack->set_var(m_var_desc_array.get_ptr(n_var), &v);
				break;
			}

			if (*presult == 0)
			{
				v.from_input(pcur_string);
				m_pcall_stack->set_var(m_var_desc_array.get_ptr(n_var), &v);
				n_var ++;
				break;
			}

			if (*presult == ';' || *presult == ',')
			{
				*presult = 0;
				v.from_input(pcur_string);
				m_pcall_stack->set_var(m_var_desc_array.get_ptr(n_var), &v);
				n_var ++;
				pcur_string = presult + 1;
			}
			presult++;
		}
		while (n_var < m_var_desc_array.get_size());
	}

	if (p_alloc != 0) free(p_alloc);

	_ASSERT(this != m_pnext);			// infinite spin

	if (p_terminal->get_continue())
		m_ptable->set_cur_atom(m_pnext);
	else
		m_ptable->set_cur_atom(0);

	m_ptable->m_binput = false;		// ok, now can step

	// resume execution
	if (m_pnext)
	{
		switch (m_prev_run_mode)
		{
		case run_mode::run_run:
			m_ptable->run_go();
			break;

		case run_mode::run_step_out:
			m_ptable->step_out();
			break;

		case run_mode::run_step_into:
			m_ptable->step_into();
			break;

		case run_mode::run_step_over:
			m_ptable->step_over();
			break;

		case run_mode::run_to_line:
			m_ptable->run_go(); // todo: remember destination line also.
			break;

		default:
			_ASSERT(0);
		}
	}
}

void c_input_atom::prepare_func_helper()
{
}

void c_input_atom::enum_expressions(c_ptr_array<c_expression>* p_expressions)
{
}

#ifdef _DEBUG
void c_input_atom::dump(FILE* f)
{
	fprintf(f,
"--------------------------------------\n\
type =\t\tinput\n\
this =\t\t%llX\n\
m_pnext = \t%llX\n\
m_nvars =\t%d\n\
line =\t\t%d\n",
(UINT64)this,
(UINT64)m_pnext,
		m_var_desc_array.get_size(),
		m_nline);

	m_func_caller.dump(f);
}
#endif

#ifdef _DEBUG
void c_input_atom::assert_valid()
{
	c_atom::assert_valid();
}
#endif
