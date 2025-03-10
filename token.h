#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <crtdbg.h>

#include "c_variable.h"
#include "c_string.h"
#include "config.h"


enum class token_type
{
	lb					= '(',
	rb					= ')',
	power				= '^',
	multiply			= '*',
	divide				= '/',
	reminder			= '%',
	plus				= '+',
	minus				= '-',
	cat					= '&',

	equal				= '=',
	notequal			= '<>',
	less				= '<',
	greater				= '>',
	lesse				= '<=',
	greatere			= '>=',

	not					= 'N',
	and					= 'A',
	or					= 'O',
	xor					= 'X',

	remark				= '\'',
	assign				= '=',
	integer				= 'n',
	floating			= 'd',
	string				= '"',
	name				= 'i',
	func				= 'f',
	semi				= ';',
	comma				= ',',
	eos					= '\n',
	eof					= 0,
	label				= ':',
	dot					= '.',

	slb					= '[',
	srb					= ']',

	if_cond				= 1000,
	then_cond			= 1001,
	else_cond			= 1002,
	for_cond			= 1003,
	to_cond				= 1004,
	step_cond			= 1005,
	next_cond			= 1006,
	goto_cond			= 1007,
	stop_cond			= 1008,
	end_cond			= 1009,
	dim_cond			= 1010,
	as_cond				= 1011,
	new_cond			= 1012,
	endif_cond			= 1013,
	print_cond			= 1014,
	input_cond			= 1015,
	while_cond			= 1016,
	wend_cond			= 1017,
	dialog_cond			= 1018,
	enddialog_cond		= 1019,
	beep_cond			= 1020,
	cls_cond			= 1021,
	sub_cond			= 1022,
	endsub_cond			= 1023,
	function_cond		= 1024,
	endfunction_cond	= 1025,

	do_cond				= 1026,
	loop_cond			= 1027,
	until_cond			= 1028,

	select_cond			= 1029,
	case_cond			= 1030,
	endselect_cond		= 1031,

	byref_cond			= 1032,
	byval_cond			= 1033,

	declare_cond		= 1034,
	lib_cond			= 1035,
	alias_cond			= 1036,

	option_cond			= 1037,
	explicit_cond		= 1038,

	exit_cond			= 1041,
	exit_sub_cond		= 1042,
	exit_function_cond	= 1043,
	exit_for_cond		= 1044,
	exit_while_cond		= 1045,
	exit_do_cond		= 1046,

	system				= 1047,
	graphic				= 1048,
	object				= 1049,
	property			= 1050,
};

//-----------------------------------------------------------------------------
// reserved_w -----------------------------------------------------------------
//-----------------------------------------------------------------------------

static const char *reserved_w[] =
{
	"If",
	"Then",
	"Else",
	"EndIf",
	"For",
	"To",
	"Step",
	"Next",
	"While",
	"Wend",
	"GoTo",
	"End",
	"Dim",
	"As",
	"New",
	"Print",
	"Input",
	"Mod",
	"Not",
	"And",
	"Or",
	"Xor",
	"Dialog",
	"EndDialog",
	"Beep",
	"Cls",
	"Sub",
	"EndSub",
	"Function",
	"EndFunction",

	"Do",
	"Loop",
	"Until",

	"Select",
	"Case",
	"EndSelect",

	"ByRef",
	"ByVal",

	"Declare",
	"Lib",
	"Alias",

	"Option",
	"Explicit",

	"Exit",

	"System",
	"Graphic",
	"Object",
	"Visible",
};

#define NUM_TOKENS	(sizeof(reserved_w) / sizeof(reserved_w[0]))

static token_type reserved_cond[] =
{
	token_type::if_cond,
	token_type::then_cond,
	token_type::else_cond,
	token_type::endif_cond,
	token_type::for_cond,
	token_type::to_cond,
	token_type::step_cond,
	token_type::next_cond,
	token_type::while_cond,
	token_type::wend_cond,
	token_type::goto_cond,
	token_type::end_cond,
	token_type::dim_cond,
	token_type::as_cond,
	token_type::new_cond,
	token_type::print_cond,
	token_type::input_cond,
	token_type::reminder,
	token_type::not,
	token_type::and,
	token_type:: or,
	token_type::xor,
	token_type::dialog_cond,
	token_type::enddialog_cond,
	token_type::beep_cond,
	token_type::cls_cond,
	token_type::sub_cond,
	token_type::endsub_cond,
	token_type::function_cond,
	token_type::endfunction_cond,

	token_type::do_cond,
	token_type::loop_cond,
	token_type::until_cond,

	token_type::select_cond,
	token_type::case_cond,
	token_type::endselect_cond,

	token_type::byref_cond,
	token_type::byval_cond,

	token_type::declare_cond,
	token_type::lib_cond,
	token_type::alias_cond,

	token_type::option_cond,
	token_type::explicit_cond,

//----------------------

	token_type::exit_cond,	// illegal by itself
	token_type::exit_sub_cond,
	token_type::exit_function_cond,

//-----------------------

	token_type::system,
	token_type::graphic,
	token_type::object,
	token_type::property,
};

//-----------------------------------------------------------------------------
// c_token --------------------------------------------------------------------
//-----------------------------------------------------------------------------

class c_token
{
	c_string	m_format;
public:
	c_string	m_name;

	token_type	type;
	int			_int;
	double		_float;
	c_string	_string;

	c_token()
	{
		reset();
	}

	void operator = (c_token& origin)
	{
		type		= origin.type;
		_float		= origin._float;
		_int		= origin._int;
		m_name		= origin.m_name;
	}

	bool disp_name()
	{
		return (type == token_type::name);
	}

	LPCTSTR format()
	{
		int i;
		for (i = 0; i < NUM_TOKENS; i++)
		{
			if (type == reserved_cond[i])
			{
				m_format.format("'%s'", reserved_w[i]);
				return m_format.get_buffer();
			}
		}

		switch(type)
		{
		case token_type::lb:		m_format = "'('"; break;
		case token_type::rb:		m_format = "')'"; break;
		case token_type::slb:		m_format = "'['"; break;
		case token_type::srb:		m_format = "']'"; break;
		case token_type::semi:		m_format = "';'"; break;
		case token_type::comma:		m_format = "','"; break;
		case token_type::plus:		m_format = "'+'"; break;
		case token_type::minus:		m_format = "'-'"; break;
		case token_type::multiply:	m_format = "'*'"; break;
		case token_type::divide:	m_format = "'/'"; break;
		case token_type::power:		m_format = "'^'"; break;
		case token_type::reminder:	m_format = "'%'"; break;
		case token_type::cat:		m_format = "'&'"; break;

		case token_type::dot:		m_format = "'.'"; break;

		case token_type::equal:		m_format = "'='"; break;
		case token_type::less:		m_format = "'<'"; break;
		case token_type::greater:	m_format = "'>'"; break;
		case token_type::lesse:		m_format = "'<'"; break;
		case token_type::greatere:	m_format = "'>'"; break;

		case token_type::not:		m_format = "not"; break;
		case token_type::and:		m_format = "and"; break;
		case token_type:: or:		m_format = "or"; break;
		case token_type::xor:		m_format = "xor"; break;

		case token_type::eos:		m_format = "end of string"; break;
		case token_type::eof:		m_format = "end of file"; break;

		case token_type::exit_cond:			m_format = "exit"; break;
		case token_type::exit_sub_cond:		m_format = "exit sub"; break;
		case token_type::exit_function_cond:m_format = "exit function"; break;
		case token_type::exit_for_cond:		m_format = "exit for"; break;
		case token_type::exit_while_cond:	m_format = "exit while"; break;
		case token_type::exit_do_cond:		m_format = "exit do"; break;

		case token_type::integer:
			m_format.format("integer %d", _int);
			break;

		case token_type::floating:
			m_format.format("floating %f", _float);
			break;

		case token_type::string:
			m_format.format("string '%s'", (LPCTSTR)_string);
			break;

		case token_type::name:
			m_format.format("identifier '%s'", (LPCTSTR)m_name);
			break;

		/*
		case token_type::system:
			m_format.format("",);
			break;
		*/
		default:
			m_format.empty();
			_ASSERT (false);	// I must forgot something
		}

		return m_format.get_buffer();
	}

	void reset()
	{
		type		= token_type::eos;
		m_name.empty();
		_string.empty();
		_float		= 0;
		_int		= 0;
		m_format.empty();
	}

};

#endif