#include "pch.h"

#include <crtdbg.h>
#include "c_assign_atom.h"
#include "c_engine.h"

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

	// System 객체 처리 추가
	if (m_is_system_object)
	{
		// 시스템 객체 경로 구성 (파라미터 표현식 평가)
		c_string final_path = m_system_path;

		// 경로의 파라미터들을 실제 값으로 치환
		// c_string에서 필요한 함수가 없으므로 단순화된 접근법 사용

		// 표현식 평가
		m_pexpression->exec(&exec);

		// System_SetProperty 함수 호출 (ScriptSystem.cpp에 정의됨)
		extern bool __stdcall System_SetProperty(const char* path, VARIANT * value);

		VARIANT value;
		VariantInit(&value);

		// c_variable을 VARIANT로 변환
		switch (exec.vt)
		{
		case VT_I4:
			value.vt = VT_I4;
			value.lVal = exec.lVal;
			break;
		case VT_R8:
			value.vt = VT_R8;
			value.dblVal = exec.dblVal;
			break;
		case VT_BOOL:
			value.vt = VT_BOOL;
			value.boolVal = exec.boolVal;
			break;
		case VT_BSTR:
			value.vt = VT_BSTR;
			value.bstrVal = SysAllocString(exec.bstrVal);
			break;
		default:
			value.vt = VT_I4;
			value.lVal = exec.as_integer();
			break;
		}

		// 경로에서 파라미터 처리를 위한 간단한 방법
		// 실제 구현에서는 필요에 따라 경로 처리 로직을 작성하세요

		// 속성 설정 함수 호출
		bool result = System_SetProperty(final_path.get_buffer(), &value);

		// VARIANT 정리
		VariantClear(&value);

		if (!result)
		{
			m_pengine->runtime_error(m_nline, "failed to set property for System object path: %s", final_path.get_buffer());
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


