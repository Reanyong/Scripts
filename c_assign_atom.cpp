#include "pch.h"

#include <crtdbg.h>
#include "c_assign_atom.h"
#include "c_engine.h"

c_assign_atom::c_assign_atom(c_atom_table* ptable,
							 c_call_stack* p_call_stack,
							 c_engine* p_engine,
							 int nline)
:c_atom(ptable, p_call_stack, p_engine, nline)
{
	m_pengine		= p_engine;
	m_type			= atom_type::assign_atom;
	m_pnext			= 0;
	m_pexpression	= 0;
	_type			= type::_undef;
}

c_assign_atom::~c_assign_atom()
{
	if (m_pexpression != 0) delete m_pexpression;
	m_dims.delete_items();
	m_pengine = NULL;
	m_pnext = NULL;
	m_pexpression = NULL;
}

void c_assign_atom::set_var_name(LPCTSTR p_var_name)
{
	_type = type::_var;
	m_var_desc = p_var_name;
}

void c_assign_atom::set_expression(c_expression* p_expression)
{
	_ASSERT(!m_pexpression);
	m_pexpression = p_expression;
}

void c_assign_atom::exec_()
{
	_ASSERT(_type != type::_undef);
	_ASSERT(m_pexpression != 0);
	_ASSERT(this != m_pnext);			// infinite spin
	c_variable exec;

	switch (_type)
	{
	case type::_var:
		{

			_ASSERT(m_var_desc.inited());
			
			if (m_var_desc.get_name()[0] == '@' ) // 202201 jkh
			{	
				
				m_pexpression->exec(&exec);
				CString szName;
				//exec.as_string -> 값 확인해야함 
				szName = m_var_desc.get_name();

				if (szName.Left(1) == "@")
					szName = szName.Right(szName.GetLength() - 1);
				
				if (!CheckValidTagName(szName))
				{
					break;
				}
				else
				{
					// bool 값 확인해야되는데 jkh
					double insertValue = exec.as_double();
					KWPutTagValue(szName, exec);
					break;
				}
			}
			else
			{
				c_variable* p_target;
				m_pcall_stack->get(&m_var_desc, &p_target);

				if (m_dims.get_size())
				{
					//				c_variable* p_var;
					//				m_pcall_stack->get(&m_var_desc, &p_var);

					c_variable v;
					m_pexpression->exec(&v);

					_ASSERT(p_target);
					_ASSERT(p_target->vt == (VT_ARRAY | VT_VARIANT));

					set_item(p_target->parray, m_dims.get_data(), m_dims.get_size(), &v, m_pexpression->m_pengine);
				}
				else
				{
					m_pexpression->exec(p_target);
					//				m_pcall_stack->set_var(&m_var_desc, &v);
				}
			}
		}
		break;

	case type::_prop:
		break;

	default: _ASSERT(false);
	}

	m_ptable->set_cur_atom(m_pnext);
}

void c_assign_atom::prepare_func_helper()
{
	_ASSERT(m_pexpression);

	int i;
	for (i = 0; i < m_dims.get_size(); i++)
		m_func_caller.from_expr(m_dims[i]);

	m_func_caller.from_expr(m_pexpression);
}

void c_assign_atom::enum_expressions(c_ptr_array<c_expression>* p_expressions)
{
	int i;
	for (i = 0; i < m_dims.get_size(); i++)
		p_expressions->add(m_dims[i]);

	p_expressions->add(m_pexpression);
}

#ifdef _DEBUG
void c_assign_atom::dump(FILE* f)
{
	fprintf(f,
"--------------------------------------\n\
type =\t\tassign\n\
m_varname =\t%s\n\
_type =\t\t%s\n\
this =\t\t%llX\n\
m_pnext = \t%llX\n\
m_pexpression =\t%llX\n\
line =\t\t%d\n",
		m_var_desc.get_name(),    
		_type== type::_var ? "var" : "property",
		(UINT64)this,
		(UINT64)m_pnext,
		(UINT64)m_pexpression,
		m_nline);

	m_func_caller.dump(f);
}
#endif

#ifdef _DEBUG
void c_assign_atom::assert_valid()
{
	_ASSERT(m_pexpression);
	m_pexpression->assert_valid();

	int i;
	for (i = 0; i < m_dims.get_size(); i++)
		m_dims[i]->assert_valid();

	c_atom::assert_valid();
}
#endif


