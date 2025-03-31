#include "pch.h"

#include <crtdbg.h>
#include "c_assign_atom.h"
#include "c_engine.h"

extern bool __stdcall System_SetProperty(const char* path, VARIANT* value);

c_assign_atom::c_assign_atom(c_atom_table* ptable,
							 c_call_stack* p_call_stack,
							 c_engine* p_engine,
							 int nline)
:c_atom(ptable, p_call_stack, p_engine, nline),
m_is_system_object(false),
m_is_get_property(false)
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

	if (m_is_system_object)
	{
		// ǥ���� ��
		m_pexpression->exec(&exec);

		/*
		DebugLog("�ý��� ��ü �Ӽ� ����: %s (Ÿ��=%d, ��=%d)",
			m_system_path.get_buffer(),
			exec.vt,
			exec.vt == VT_BOOL ? exec.boolVal :
			exec.vt == VT_I4 ? exec.lVal : 0);
		*/

		VARIANT value;
		VariantInit(&value);

		if (strstr(m_system_path.get_buffer(), ".Visible") != NULL) {
			// ��� Ÿ���� boolean���� ��ȯ
			value.vt = VT_BOOL;

			switch (exec.vt) {
			case VT_BOOL:
				value.boolVal = exec.boolVal ? VARIANT_TRUE : VARIANT_FALSE;
				break;
			case VT_I4:
				value.boolVal = (exec.lVal != 0) ? VARIANT_TRUE : VARIANT_FALSE;
				break;
			case VT_BSTR:
				// ���ڿ��� ��� "true", "1" ���� ���� Ȯ��
			{
				c_string str_val;
				exec.as_string(str_val);
				if (_stricmp(str_val.get_buffer(), "true") == 0 ||
					_stricmp(str_val.get_buffer(), "1") == 0)
					value.boolVal = VARIANT_TRUE;
				else
					value.boolVal = VARIANT_FALSE;
			}
			break;
			default:
				// �ٸ� Ÿ���� as_integer�� ���� ��ȯ
				value.boolVal = (exec.as_integer() != 0) ? VARIANT_TRUE : VARIANT_FALSE;
				break;
			}
		}
		else {
			// �ٸ� �Ӽ��� ��� ���� Ÿ�� ����
			switch (exec.vt) {
			case VT_BOOL:
				value.vt = VT_BOOL;
				value.boolVal = exec.boolVal ? VARIANT_TRUE : VARIANT_FALSE;
				break;
			case VT_I4:
				value.vt = VT_I4;
				value.lVal = exec.lVal;
				break;
			case VT_BSTR:
				value.vt = VT_BSTR;
				value.bstrVal = SysAllocString(exec.bstrVal);
				break;
			default:
				// �⺻������ ������ ��ȯ
				value.vt = VT_I4;
				value.lVal = exec.as_integer();
				break;
			}
		}

		// �Ӽ� ���� �Լ� ȣ��
		bool result = System_SetProperty(m_system_path.get_buffer(), &value);

		if (!result)
		{
			m_pengine->runtime_error(m_nline, "�ý��� ��ü �Ӽ� ���� ����: %s", m_system_path.get_buffer());
		}

		// bstrVal �޸� ������ �߰�
		if (value.vt == VT_BSTR && value.bstrVal)
		{
			SysFreeString(value.bstrVal);
		}

		m_ptable->set_cur_atom(m_pnext);
		return;
	}

	switch (_type)
	{
	case type::_var:
		{

			_ASSERT(m_var_desc.inited());

			if (m_var_desc.get_name()[0] == '@' ) // 202201 jkh
			{

				m_pexpression->exec(&exec);
				CString szName;
				//exec.as_string -> �� Ȯ���ؾ���
				szName = m_var_desc.get_name();

				if (szName.Left(1) == "@")
					szName = szName.Right(szName.GetLength() - 1);

				if (!CheckValidTagName(szName))
				{
					break;
				}
				else
				{
					// bool �� Ȯ���ؾߵǴµ� jkh
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


