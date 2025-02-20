#include "pch.h"
#include "c_disp_level.h"
#include "c_engine.h"

//-----------------------------------------------------------------------------
// construction/destruction ---------------------------------------------------
//-----------------------------------------------------------------------------

c_disp_level::c_disp_level(FUNCDESC* p_fd, c_call_stack* p_call_stack)
{
//	m_type			= type;
//	m_memid			= -1;
//	m_invkind		= (INVOKEKIND)0;

	memcpy(&m_funcdesc, p_fd, sizeof(FUNCDESC));

	m_passign_expr	= 0;
	m_pcall_stack	= p_call_stack;
}

c_disp_level::~c_disp_level()
{
	if (m_passign_expr) delete m_passign_expr;
	m_expressions.delete_items();
}

//-----------------------------------------------------------------------------
// can_be_used_in_set ---------------------------------------------------------
//-----------------------------------------------------------------------------

bool c_disp_level::can_be_used_in_set()
{
	if (m_funcdesc.invkind == DISPATCH_PROPERTYGET) return false;

	if (m_funcdesc.invkind == DISPATCH_PROPERTYPUT || m_funcdesc.invkind == DISPATCH_PROPERTYPUTREF)
	{
		if (!m_passign_expr) return false;
	}

	// todo: more conditions?

	return true;
}

//-----------------------------------------------------------------------------
// get ------------------------------------------------------------------------
//-----------------------------------------------------------------------------

bool c_disp_level::get(LPDISPATCH lpd,
					   c_variable* p_result,
					   c_engine* p_engine,
					   int n_line)
{
	HRESULT hr;
	unsigned int n_error = 0;
	DISPPARAMS disp_params;
	EXCEPINFO exc = {0};

	if (!lpd)
	{
		p_engine->runtime_error(CUR_ERR_LINE, "trying to call method of NULL object.");
		return false;
	}

	c_array<c_variable> args;
	c_expression::prepare_args(&m_expressions, &args, m_pcall_stack, p_engine, true);

/*
	while (args.get_size() < m_funcdesc.cParams)
	{
		c_variable v;
		v.vt = VT_ERROR;
		v.scode = DISP_E_PARAMNOTFOUND;

		args.insert(v, 0);
		//args.add(v);
	}
*/

	c_array<c_var_storage> storage;
	c_expression::store_args(&args, &storage);

	disp_params.cArgs = args.get_size();
	disp_params.rgvarg = args.get_data();
	disp_params.cNamedArgs = 0;
	disp_params.rgdispidNamedArgs = 0;

	switch (m_funcdesc.invkind)
	{
	case DISPATCH_PROPERTYGET:
		{
			_ASSERT(!m_passign_expr);

			PUSH_RESERVED(p_result);

			hr = lpd->Invoke(
				m_funcdesc.memid,
				IID_NULL,
				LOCALE_USER_DEFAULT,
				m_funcdesc.invkind,
				&disp_params,
				p_result,
				&exc,
				&n_error);

			POP_RESERVED(p_result);

			if (FAILED(hr))
			{
				if (p_engine)
				{
					c_string msg;
					msg.format("Error getting property '%s': ", m_member_name.get_buffer());
					p_engine->disp_error(n_line, hr, &exc, msg.get_buffer());
				}

				c_expression::restore_args(&args, &storage);
				return false;
			}
		}
		break;

	case DISPATCH_METHOD:
		{
			PUSH_RESERVED(p_result);

			// invoke
			hr = lpd->Invoke(
				m_funcdesc.memid,
				IID_NULL,
				LOCALE_USER_DEFAULT,
				m_funcdesc.invkind,
				&disp_params,
				p_result,
				&exc,
				&n_error);

			POP_RESERVED(p_result);

			if (FAILED(hr))
			{
				c_string msg;
				msg.format("Error invoking method '%s': ", m_member_name.get_buffer());
				if (p_engine) p_engine->disp_error(n_line, hr, &exc, msg.get_buffer());

				c_expression::restore_args(&args, &storage);
				return false;
			}
		}
		break;

	default: _ASSERT(0);
	}

	c_expression::restore_args(&args, &storage);

	return true;
}

//-----------------------------------------------------------------------------
// set ------------------------------------------------------------------------
//-----------------------------------------------------------------------------

bool c_disp_level::set(LPDISPATCH lpd,
					   c_engine* p_engine,
					   int n_line)

{
	_ASSERT(lpd);
	_ASSERT(m_funcdesc.memid != -1);

	HRESULT hr;
	c_variable result;
	unsigned int n_error = 0;
	DISPPARAMS disp_params;
	EXCEPINFO exc = {0};

	if (!lpd)
	{
		p_engine->runtime_error(CUR_ERR_LINE, "trying to call method of NULL object.");
		return false;
	}

	c_array<c_variable> args;
	c_expression::prepare_args(&m_expressions, &args, m_pcall_stack, p_engine, true);

	c_array<c_var_storage> storage;

	switch (m_funcdesc.invkind)
	{
	case DISPATCH_PROPERTYPUT:
	case DISPATCH_PROPERTYPUTREF:
		{
			_ASSERT(m_passign_expr);

			c_variable v;
			m_passign_expr->exec(&v);
			args.insert(v, 0);

			c_expression::store_args(&args, &storage);
			// attach args to 'disp_params'

			DISPID dispidNamed = DISPID_PROPERTYPUT;
			disp_params.cArgs = args.get_size();
			disp_params.rgvarg = args.get_data();
			disp_params.cNamedArgs = 1;
			disp_params.rgdispidNamedArgs = &dispidNamed;

			// invoke

			PUSH_RESERVED(&result);

			hr = lpd->Invoke(
				m_funcdesc.memid,
				IID_NULL,
				LOCALE_USER_DEFAULT,
				m_funcdesc.invkind,
				&disp_params,
				&result,
				&exc,
				&n_error);

			POP_RESERVED(&result);

			if (FAILED(hr))
			{
				c_string msg;
				msg.format("Error setting property '%s': ", m_member_name.get_buffer());
				if (p_engine) p_engine->disp_error(n_line, hr, &exc, msg.get_buffer());

				c_expression::restore_args(&args, &storage);
				return false;
			}
		}
		break;

	case DISPATCH_METHOD:
		{
			c_expression::store_args(&args, &storage);

			// attach args to 'disp_params'

			disp_params.cArgs = args.get_size();
			disp_params.rgvarg = args.get_data();
			disp_params.cNamedArgs = 0;
			disp_params.rgdispidNamedArgs = 0;

			PUSH_RESERVED(&result);
			// invoke
			hr = lpd->Invoke(
				m_funcdesc.memid,
				IID_NULL,
				LOCALE_USER_DEFAULT,
				m_funcdesc.invkind,
				&disp_params,
				&result,
				&exc,
				&n_error);

			POP_RESERVED(&result);

			//for (i = 0; i < m_args.get_size(); i++) VariantClear(args.get_ptr(i));

			if (FAILED(hr))
			{
				c_string msg;
				msg.format("Error invoking method '%s': ", m_member_name.get_buffer());
				if (p_engine) p_engine->disp_error(n_line, hr, &exc, msg.get_buffer());

				c_expression::restore_args(&args, &storage);
				return false;
			}
		}
		break;

	default: _ASSERT(0);
	}

	c_expression::restore_args(&args, &storage);

	return true;
}

//-----------------------------------------------------------------------------
// assert_valid ---------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifdef _DEBUG
void c_disp_level::assert_valid()
{
	_ASSERT(!IsBadWritePtr(this, sizeof(c_disp_level)));
	if (m_passign_expr) m_passign_expr->assert_valid();

	_ASSERT(m_pcall_stack);
	_ASSERT(m_member_name.get_length());

	int i;
	for (i = 0; i < m_expressions.get_size(); i++)
		m_expressions[i]->assert_valid();
}
#endif

