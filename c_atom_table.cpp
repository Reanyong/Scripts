#include "pch.h"

#include <crtdbg.h>
#include "c_atom_table.h"
#include "c_routine_entry_atom.h"
#include "c_end_routine_atom.h"
#include "c_expression.h"
#include "c_for_atom.h"
#include "c_while_atom.h"
#include "c_do_atom.h"
#include "c_engine.h"

//-----------------------------------------------------------------------------
// c_atom_table ---------------------------------------------------------------
//-----------------------------------------------------------------------------

c_atom_table::c_atom_table(c_call_stack* p_call_stack, c_engine* p_engine)
{
	m_pcur_atom		= 0;
	m_pentry_atom	= 0;
	m_run_mode		= run_mode::run_undef;
	m_nto_line		= -1;
	m_pcall_stack	= p_call_stack;
	m_pengine		= p_engine;

	m_binput		= false;
	m_bwaiting		= false;
	m_bforce_stop	= false;
	m_nlevel = 0;

//	m_hstep_event = CreateEvent(0,	// security
//		true,							// auto reset
//		false,							// initially signaled
//		0);							// name
//
//	_ASSERT(m_hstep_event);
}

c_atom_table::~c_atom_table()
{
	free_all();
}

void c_atom_table::free_all()
{
#ifndef _DEBUG
	try
	{
#endif
		// this will delete m_pbp_condition expressions inside atoms.
		remove_all_breakpoints();

		m_atoms.delete_items();
		m_atoms.reset();
#ifndef _DEBUG
	}
	catch (...)
	{
//		AfxMessageBox("Exception in 'c_atom_table::~c_atom_table()'!");
	}
#endif

	m_pentry_atom	= 0;
	m_pcur_atom		= 0;
	reset();
}

void c_atom_table::reset()
{
	m_nlevel		= 0;
	m_binput		= false;
	//20220718 YDK 수정
	//m_bforce_stop	= false;
//	m_run_mode		= run_undef;
	m_nto_line		= -1;
}

int c_atom_table::get_count()
{
	return m_atoms.get_size();
}

c_atom*	c_atom_table::get_atom(int n_atom)
{
	return m_atoms[n_atom];
}

void c_atom_table::add(c_atom* patom)
{
	_ASSERT(patom);
	_ASSERT(!m_atoms.contains(patom));
	m_atoms.add(patom);
}

#ifdef _DEBUG
void c_atom_table::dump()
{
	if(m_pcur_atom!=NULL)
	{
		FILE* pf;
		pf = fopen("c:\\_c_atom_table.txt", "wt");

		fprintf(pf, "entry = %llX\n",(UINT64) m_pcur_atom);
		for (int i = 0; i < m_atoms.get_size(); i++)
			m_atoms[i]->dump(pf);

		fclose(pf);
	}
}
#endif

c_routine_entry_atom* c_atom_table::get_routine_entry(LPCTSTR p_sub_name, bool b_is_sub)
{
	for (int i = 0; i < m_atoms.get_size(); i++)
	{
		c_atom* p_atom = m_atoms[i];
		if (p_atom->m_type == atom_type::routine_entry_atom)
		{
			c_routine_entry_atom* p_sub_entry_atom = (c_routine_entry_atom*)p_atom;
			if (p_sub_entry_atom->is_sub() == b_is_sub)
				if (_stricmp(p_sub_entry_atom->m_var_desc.get_name(), p_sub_name) == 0)
					return p_sub_entry_atom;
		}
	}

	return 0;
}

c_end_routine_atom* c_atom_table::get_matching_endsub(c_routine_entry_atom* p_sub_entry)
{
	_ASSERT(p_sub_entry->m_type == atom_type::routine_entry_atom);

	int found_at = -1;
	int i = 0;
	for ( i = 0; i < m_atoms.get_size(); i++)
		if (p_sub_entry == m_atoms[i])
		{
			found_at = i;
			break;
		}

	if (found_at == -1) return 0;	// not in list

	for (i = found_at; i < m_atoms.get_size(); i++)
	{
		c_atom* p_atom = m_atoms[i];
		if (p_atom->m_type == atom_type::end_routine_atom)
			return (c_end_routine_atom*)p_atom;
	}

	return 0;						// no matching endsub
}

c_variable c_atom_table::call_function(c_routine_entry_atom* p_atom,
									 c_variable* p_variable,
									 int n_variable)
{
	_ASSERT(p_atom != 0);
	_ASSERT(p_atom->m_type == atom_type::routine_entry_atom);
	if (n_variable) _ASSERT(p_variable);

	m_pcur_atom = p_atom;

//	run_mode _run_mode = m_run_mode;
//	m_run_mode = run_run;
	run_();
//	m_run_mode = _run_mode;

	if (m_pcur_atom)
		_ASSERT (m_pcur_atom->m_type == atom_type::end_routine_atom);

	c_variable* p_ret;
	m_pcall_stack->get(p_atom->get_var_desc(), &p_ret);
	if (m_pcur_atom) m_pcur_atom->exec();
	c_atom* p_next = m_pcall_stack->ret();
	_ASSERT(p_next == 0);

	return *p_ret;
}

void c_atom_table::prepare_function_callers(int n_first_atom)
{
	int i;
	for (i = n_first_atom; i < m_atoms.get_size(); i++)
		m_atoms[i]->prepare_func_helper();
}

bool c_atom_table::find_entry_atom()
{
	if (m_pentry_atom) return true; // prepared by 'c_engine::run_routine'

	bool b_subs_are_present = false;
	int i;

	for (i = 0; i < get_count(); i++)
	{
		c_atom* p_atom = get_atom(i);

		if (p_atom->m_type == atom_type::routine_entry_atom)
		{
			b_subs_are_present = true;	// we have subs in our source
			m_pentry_atom = 0;
		}

		if (p_atom->m_type == atom_type::end_routine_atom)
			if (get_count() - i > 1)				// is there is something after 'endsub'
				m_pentry_atom = get_atom(i + 1);	// entry is after last 'endsub'
	}

	if (b_subs_are_present)
	{
		if (m_pentry_atom == 0)
		{
			m_pengine->error(NO_ERR_LINE, "cannot find entry after last 'endsub'.");
			return false;
		}
	}

	return true;
}

void c_atom_table::run_()
{
// startup --------------------------------------------------------------------

#ifdef _DEBUG
	m_pengine->assert_valid();
#endif

	if (m_pcur_atom == 0)
		if (m_atoms.get_size() != 0)
		{
			if (!find_entry_atom()) return;
			if (m_pentry_atom) m_pcur_atom = m_pentry_atom;
			else m_pcur_atom = m_atoms[0];
			if (m_run_mode != run_mode::run_run) return;
		}

// main loops -----------------------------------------------------------------

	switch (m_run_mode)
	{
	case run_mode::run_run:
		while (m_pcur_atom && !m_binput)
		{
#ifdef _DEBUG
			m_pengine->assert_valid();
#endif

			if (m_run_mode != run_mode::run_run) break; // runtime error can cause it.
			if (m_bforce_stop) break;

			m_pcur_atom->exec();
			if (!m_pcur_atom) break;
			if (m_pcur_atom->brk()) break; // breakpoint

#ifdef _DEBUG
			m_pengine->assert_valid();
#endif
		}
		break;

	case run_mode::run_step_out:
		if (!m_binput)
		{
			while(m_pcur_atom && !m_binput)
			{
				if (m_run_mode != run_mode::run_step_out) break; // runtime error can cause it.
				if (m_bforce_stop) break;

				if (m_pcur_atom->m_type == atom_type::routine_entry_atom) m_nlevel ++;
				if (m_pcur_atom->m_type == atom_type::end_routine_atom && m_nlevel >= 0) m_nlevel --;

				m_pcur_atom->exec();

				if (!m_pcur_atom) break;
				if (m_nlevel <= -1)
				{
					m_nlevel = 0;
					break;
				}
			}
		}
		break;

	case run_mode::run_step_into:
		if (!m_binput)
		{
			//20220713 YDK 수정
			int n_line = -1;
			if (m_pcur_atom)
				n_line = m_pcur_atom->m_nline;
			while(m_pcur_atom && !m_binput)
			{
				if (m_bforce_stop) break;

				// exec current atom
				if (n_line != m_pcur_atom->m_nline) break;

				m_pcur_atom->exec();

				if (!m_pcur_atom) break;

				// ommit routine entry and end routine
				while (m_pcur_atom && (m_pcur_atom->m_type == atom_type::routine_entry_atom ||
						m_pcur_atom->m_type == atom_type::end_routine_atom))
						m_pcur_atom->exec();

				if (!m_pcur_atom) break;
				if (m_pcur_atom->brk()) break; // breakpoint
			}
		}
		break;

	case run_mode::run_step_over:
		if (!m_binput)
		{
			int n_line = 0;
			if (m_pcur_atom!=NULL)
			{
				n_line = m_pcur_atom->m_nline;
				while (1 && !m_binput)
				{
					if (m_run_mode != run_mode::run_step_over) break; // runtime error can cause it.
					if (m_bforce_stop) break;

					if (m_nlevel < 0) m_nlevel = 0;

					// execute
					if (m_pcur_atom->m_type == atom_type::routine_entry_atom) m_nlevel++;
					if (m_pcur_atom->m_type == atom_type::end_routine_atom && m_nlevel > 0) m_nlevel--;

					if (m_pcur_atom!=NULL)
					{
						m_pcur_atom->exec();
					}


					if (!m_pcur_atom) break;
					if (m_pcur_atom->m_type == atom_type::routine_entry_atom) continue;

					if (m_nlevel == 0 && n_line != m_pcur_atom->m_nline) break;
					if (m_pcur_atom->brk()) break; // breakpoint
				}
			}
		}
		break;

	case run_mode::run_to_line:
		while (m_pcur_atom && !m_binput)
		{
			if (m_run_mode != run_mode::run_to_line) break; // runtime error can cause it.
			if (m_bforce_stop) break;

			m_pcur_atom->exec();

			if (!m_pcur_atom) break;
			if (m_pcur_atom->brk()) break; // breakpoint
			if (m_pcur_atom->m_nline == m_nto_line) break;
		}

		break;

	default:_ASSERT(0);
	}

#ifdef _DEBUG
	m_pengine->assert_valid();
#endif

	if (!m_pcur_atom && !m_binput) // make sure we stopped not because of 'input'
		m_pengine->on_end_execution();
}

void c_atom_table::run()
{
	m_bforce_stop = false;
	// fix invalid use

	if (m_run_mode == run_mode::run_undef)
	{
		_ASSERT(false);
		m_run_mode = run_mode::run_run;
	}

	if (m_run_mode == run_mode::run_to_line)
		if (m_nto_line < 0) m_run_mode = run_mode::run_run;

	if (m_bforce_stop) m_pcur_atom = 0;

	run_();
}

void c_atom_table::stop()
{
	m_bforce_stop = true;
	m_pcur_atom = 0;
}

int c_atom_table::get_cur_line()
{
	if (!m_pcur_atom) return -1;
	return m_pcur_atom->m_nline;
}

int c_atom_table::set_breakpoint(int n_line)
{
	for (int i = 0; i < m_atoms.get_size(); i++)
		if (m_atoms[i]->m_nline >= n_line)
		{
			m_atoms[i]->set_break(true);
			return m_atoms[i]->m_nline;
		}

	return -1;
}

int c_atom_table::remove_breakpoint(int n_line)
{
	for (int i = 0; i < m_atoms.get_size(); i++)
		if (m_atoms[i]->m_nline >= n_line)
		{
			m_atoms[i]->set_break(false);
			return m_atoms[i]->m_nline;
		}

	return -1;
}

int c_atom_table::toggle_breakpoint(int n_line, bool* p_bset)
{
	for (int i = 0; i < m_atoms.get_size(); i++)
		if (m_atoms[i]->m_nline >= n_line)
		{
			if (m_atoms[i]->get_break())
			{
				if (p_bset) *p_bset = false;
				m_atoms[i]->set_break(false);
			}
			else
			{
				if (p_bset) *p_bset = true;
				m_atoms[i]->set_break(true);
			}
			return m_atoms[i]->m_nline;
		}

	return -1;
}

int c_atom_table::get_breakpoint_count()
{
	int n_bp = 0;
	for (int i = 0; i < m_atoms.get_size(); i++)
		if (m_atoms[i]->get_break()) n_bp ++;

	return n_bp;
}

int c_atom_table::get_breakpoint(int n_index)
{
	int n_bp = 0;
	for (int i = 0; i < m_atoms.get_size(); i++)
	{
		if (m_atoms[i]->get_break())
		{
			if (n_index == n_bp) return m_atoms[i]->m_nline;
			n_bp ++;
		}
	}

	return -1;
}

void c_atom_table::remove_all_breakpoints()
{
	for (int i = 0; i < m_atoms.get_size(); i++)
		m_atoms[i]->set_break(false);
}

bool c_atom_table::set_bp_condition(int n_line, c_expression* p_expr)
{
	int i;
	for (i = 0; i < m_atoms.get_size(); i++)
		if (m_atoms[i]->m_nline >= n_line)
		{
			if (!m_atoms[i]->get_break()) return false;
			m_atoms[i]->set_bp_condition(p_expr);
			return true;
		}

	return false;
}

c_atom* c_atom_table::get_routine_atom()
{
	int i;

	if (!m_pcur_atom)	// during parsing
	{
		for (i = m_atoms.get_size() - 1; i >= 0; i--)
			if (m_atoms[i]->m_type == atom_type::routine_entry_atom)
				return m_atoms[i];
			else if (m_atoms[i]->m_type == atom_type::end_routine_atom)
				break;
	}
	else				// at run-time
	{
		for (i = 0; i < m_atoms.get_size(); i++)
			if (m_atoms[i] == m_pcur_atom)
			{
				for (; i >=0; i--)
					if (m_atoms[i]->m_type == atom_type::routine_entry_atom)
						return m_atoms[i];

				break;
			}
	}

	return 0;
}

c_atom* c_atom_table::find_prev_atom(atom_type type, int n_line)
{
	_ASSERT(!m_pcur_atom); // during parsing

	int i;

	for (i = 0; i < m_atoms.get_size(); i++)
	{
		if (m_atoms[i]->m_nline == n_line)
		{
			for (; i >= 0; i--)
				if (m_atoms[i]->m_type == type)
						return m_atoms[i];
		}
	}

	return 0;
}

const char* c_atom_table::get_routine_name()
{
	c_routine_entry_atom* p_routine_atom = (c_routine_entry_atom*)get_routine_atom();
	if (p_routine_atom) return p_routine_atom->get_var_desc()->get_name();
	return 0;
}

bool c_atom_table::is_parsing_sub()
{
	_ASSERT(!m_pcur_atom); // during parsing
	c_routine_entry_atom* p_rotine_entry = (c_routine_entry_atom*)get_routine_atom();
	if (!p_rotine_entry) return false;
	return p_rotine_entry->is_sub();
}

bool c_atom_table::is_parsing_function()
{
	_ASSERT(!m_pcur_atom); // during parsing
	c_routine_entry_atom* p_rotine_entry = (c_routine_entry_atom*)get_routine_atom();
	if (!p_rotine_entry) return false;
	return !p_rotine_entry->is_sub();
}

bool c_atom_table::is_parsing_for()
{
	_ASSERT(!m_pcur_atom); // during parsing

	int i;
	for (i = m_atoms.get_size() - 1; i >= 0; i--)
		if (m_atoms[i]->m_type == atom_type::for_atom)
		{
			c_for_atom* p_for_atom = (c_for_atom*)m_atoms[i];
			if (!p_for_atom->m_pexit) return true; // not all the loop parsed yet
		}

	return false;
}

bool c_atom_table::is_parsing_while()
{
	_ASSERT(!m_pcur_atom); // during parsing

	int i;
	for (i = m_atoms.get_size() - 1; i >= 0; i--)
		if (m_atoms[i]->m_type == atom_type::while_atom)
		{
			c_while_atom* p_while_atom = (c_while_atom*)m_atoms[i];
			if (!p_while_atom->m_pexit) return true; // not all the loop parsed yet
		}

	return false;
}

bool c_atom_table::is_parsing_do()
{
	_ASSERT(!m_pcur_atom); // during parsing

	int i;
	for (i = m_atoms.get_size() - 1; i >= 0; i--)
		if (m_atoms[i]->m_type == atom_type::do_atom)
		{
			c_do_atom* p_do_atom = (c_do_atom*)m_atoms[i];
			if (!p_do_atom->m_pexit) return true; // not all the loop parsed yet
		}

	return false;
}

bool c_atom_table::ready()
{
	return (m_atoms.get_size() != 0) && !m_binput;
}
