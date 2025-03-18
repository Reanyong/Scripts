#include "pch.h"

#include <crtdbg.h>
#include "c_dialog_atom.h"
#include "c_engine.h"

//extern c_engine* __p_engine;

c_dialog_atom::c_dialog_atom(c_atom_table* ptable,
							 c_call_stack* p_call_stack,
							 c_engine* p_engine,
							 int nline,
							 c_input_dialog* p_dialog)
:c_atom(ptable, p_call_stack, p_engine, nline)
{
	m_pengine		= p_engine;
	m_type			= atom_type::dialog_atom;
	m_pnext			= 0;
	m_prev_run_mode	= run_mode::run_undef;

	m_pdialog		= p_dialog;
}

c_dialog_atom::~c_dialog_atom()
{
}

void c_dialog_atom::set_title(LPCTSTR p_title)
{
	m_title = p_title;
}

void c_dialog_atom::add(LPCTSTR p_title, LPCTSTR p_varname)
{
	m_titles.add(p_title);


	c_var_desc* p_var_desc = m_var_desc_array.add_new();
	*p_var_desc = p_varname;
}

void c_dialog_atom::exec_()
{
	_ASSERT(this != m_pnext);			// infinite spin
	_ASSERT(m_titles.get_size() == m_var_desc_array.get_size());

	m_ptable->m_binput = true;		// can't step on dialog

	m_pdialog->SetTitle(m_title);
	c_variable* p_v;
	for (int i = 0; i < m_titles.get_size(); i++)
	{
		m_pcall_stack->get(m_var_desc_array.get_ptr(i), &p_v);
		m_pdialog->Add(m_titles[i], p_v);
	}

	m_pdialog->show();
	m_prev_run_mode = m_ptable->get_run_mode();

//	ResetEvent(m_pengine->get_io_event());
//	DWORD res = WaitForSingleObject(m_pengine->get_io_event(), INFINITE);
//	_ASSERT(res == WAIT_OBJECT_0);
//	ResetEvent(m_pengine->get_io_event());
}

void c_dialog_atom::on_window_enter()
{
	m_ptable->m_binput = false;		// ok, now can step
	m_pdialog->hide();
	if(!m_pdialog->get_continue())
	{
		m_ptable->set_cur_atom(0);
		m_pdialog->reset_contents();
		return;
	}

	c_variable v;
	int i;
	for (i = 0; i < m_titles.get_size(); i++)
	{
		v.from_input(m_pdialog->GetValue(i));
		m_pcall_stack->set_var(m_var_desc_array.get_ptr(i), &v);
	}

	m_ptable->set_cur_atom(m_pnext);
	m_pdialog->reset_contents();

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

void c_dialog_atom::prepare_func_helper()
{
}

void c_dialog_atom::enum_expressions(c_ptr_array<c_expression>* p_expressions)
{
}

#ifdef _DEBUG
void c_dialog_atom::dump(FILE* f)
{
	fprintf(f,
"--------------------------------------\n\
type =\t\tDIALOG\n\
m_title =\t%s\n\
this =\t\t%llX\n\
m_pnext =\t%llX\n\
m_ncount =\t%X\n\
line =\t\t%d\n",
		(char*)(LPCSTR)m_title,
		(UINT64)this,
		(UINT64)m_pnext,
		m_titles.get_size(),
		m_nline);

	m_func_caller.dump(f);
}
#endif

#ifdef _DEBUG
void c_dialog_atom::assert_valid()
{
	_ASSERT(m_titles.get_size());
	_ASSERT(m_titles.get_size() == m_var_desc_array.get_size());
	_ASSERT(m_pdialog);
	m_pdialog->assert_valid();

	c_atom::assert_valid();
}
#endif
