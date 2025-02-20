#include "pch.h"
#include "c_char_stream.h"

c_char_stream::c_char_stream()
{
	m_nlength		= 0;
	m_pdata			= 0;
	m_ptemp_buffer	= 0;
	reset();
}

c_char_stream::~c_char_stream()
{
	if (m_pdata) free((void*)m_pdata);
}

void c_char_stream::init()
{
}

void c_char_stream::reset()
{
	m_ncur_pos	= 0;
	m_ncur_line	= 0;
}

void c_char_stream::gettok(c_token& curtok)
{
	char c;

	do
	{
		c = get();
		//한글처리
		if ((unsigned char)c>0x7f)
		{
			return;
		}
		if(c == '\n')
			if (curtok.type != token_type::eos)
			{
				curtok.type = token_type::eos;
				return;
			}
			else c = get();

		if (c == 0)
		{
			curtok.type = token_type::eof;
			return;
		}
	}
	while(c == ' ' || c == '\t' || c == '\r' || c == '\n');
	//while (isspace(c));

	switch (c)
	{
	case '.':
		if (!isdigit(check()))
		{
			curtok.type = token_type::dot;
			break;
		}

	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':// case '.':
		{
			back();

			if (_getnum(curtok._int, curtok._float))
				curtok.type = token_type::integer;
			else
				curtok.type = token_type::floating;
		}
		break;

	case '(': case ')': case '^': case '*': case '/':
	case '+': case '-': case '=': case '<': case '>':
	case '\'': case ';': case ',': case '[': case ']':
	case '&':
		curtok.type = (token_type)c;
		c = get();
		
		if(curtok.type      == (token_type)'<' && c == '>') curtok.type = token_type::notequal;
		else if(curtok.type == (token_type)'<' && c == '=') curtok.type = token_type::lesse;
		else if(curtok.type == (token_type)'>' && c == '=') curtok.type = token_type::greatere;
		else if (c) back();
		break;

	case '"':
		{
			curtok._string.empty();
			curtok.type = token_type::string;

			while (c != 0)
			{
				c = get();

				if (c == '"')
				{
					if (check() == '"')
						c = get();
					else
						break;
				}

				curtok._string += c;
			}
		}
		break;

	default:
		{
			curtok.m_name.empty();
			// 태그이름 토큰을 위한 로직 추가 20220211 jkh
			if (c == '@' )
			{
				do
				{
					curtok.m_name += c;
					c = get();
				} while ((c & 0x80) || isalnum(c) || c == '_' || c == '$' || c == '.' || c == '@' || c == '-');
			}
			else
			{
				do
				{
					curtok.m_name += c;
					c = get();
				} while (c && isalnum(c) || c == '_' || c == '$');
			}

			if (c) back();
			
			// label
			if (curtok.type == token_type::eos)	// label can only be at the beginning of the line
			{
				int n_pos = pos();
				while (isspace(c)) c = get();

				if (c == ':')
				{
					curtok.type = token_type::label;
					get();
				}
				else
				{
					pos(n_pos);
					curtok.type = token_type::name;
				}
			}
			else
			{
				curtok.type = token_type::name;
			}

			// 추가 20210211 jkh
			int i;
			if (curtok.m_name.get_buffer()[0] != '@')
			{
				for(i = 0; i < NUM_TOKENS; i++)
				{
					if(_stricmp(curtok.m_name.get_buffer(), reserved_w[i]) == 0)
					{
						curtok.type = reserved_cond[i];
						break;
					}
				}
			}

			if (curtok.type == token_type::exit_cond)
			{
				int pos = this->pos();
				gettok(curtok);

				switch(curtok.type)
				{
				case token_type::sub_cond:
					curtok.type = token_type::exit_sub_cond;
					break;

				case token_type::function_cond:
					curtok.type = token_type::exit_function_cond;
					break;

				case token_type::for_cond:
					curtok.type = token_type::exit_for_cond;
					break;

				case token_type::while_cond:
					curtok.type = token_type::exit_while_cond;
					break;

				case token_type::do_cond:
					curtok.type = token_type::exit_do_cond;
					break;

				default:
					// this is error, but it will be reported inside "c_engine::_parse"
					curtok.type = token_type::exit_cond;
					this->pos(pos);
				}
			}



			if (curtok.type == token_type::end_cond)
			{
				int pos = this->pos();

				gettok(curtok);

				switch (curtok.type)
				{
				case token_type::if_cond:
					curtok.type = token_type::endif_cond;
					break;

				case token_type::dialog_cond:
					curtok.type = token_type::enddialog_cond;
					break;

				case token_type::sub_cond:
					curtok.type = token_type::endsub_cond;
					break;

				case token_type::function_cond:
					curtok.type = token_type::endfunction_cond;
					break;

				case token_type::select_cond:
					curtok.type = token_type::endselect_cond;
					break;

				default:
					curtok.type = token_type::end_cond;
					this->pos(pos);
				}
			}
		}
	}

	if(curtok.type == token_type::remark)
	{
		do
		{
			c = get();
		}
		while(c != 0 && c != '\n');
		curtok.type = (c ? ::token_type::eos : ::token_type::eof);
	}
}

DWORD c_char_stream::pos(DWORD newpos)
{
	if (newpos > m_nlength) newpos = m_nlength;
	DWORD oldpos = m_ncur_pos;
	m_ncur_pos = newpos;

	if (oldpos < newpos)
	{
		for (DWORD i = oldpos + 1; i <= newpos; i++)
			if (m_pdata[i] == '\n') m_ncur_line ++;
	}

	if (oldpos > newpos)
	{
		for (DWORD i = oldpos - 1; i > newpos; i--)
			if (m_pdata[i] == '\n')
				m_ncur_line --;
	}

	return oldpos;
}

char c_char_stream::get()
{
	if (m_ncur_pos >= m_nlength) return 0;
	char c = m_pdata[m_ncur_pos];
	skip();
	return c;
}

char c_char_stream::check()
{
	return m_pdata[m_ncur_pos];
}

bool c_char_stream::skip()
{
	if (m_pdata[m_ncur_pos] == '\n') 
		m_ncur_line ++;

	++m_ncur_pos;
	
	if (m_ncur_pos > m_nlength)
	{
		m_ncur_pos = m_nlength;
		return false;
	}

	return true;
}

bool c_char_stream::back()
{
	if (m_ncur_pos <= 0) return false; 

	--m_ncur_pos;

	if (m_pdata[m_ncur_pos] == '\n')
	{
//		--m_ncur_pos; // ommit '\r' also
		m_ncur_line --;
	}

	return true;
}

int c_char_stream::getnum()
{
	char c;
	int n = 0;

	for(;;)
	{
		c = get();
		if(c < '0' || c > '9')
		{
			if (c) back();
			break;
		}
		n = 10*n + c - '0';
	}

	return n ;
}

bool c_char_stream::_getnum(int& int_val, double& float_val)
{
	bool is_int = true;
	char c;
	DWORD p = pos();
	int_val = 0;
	int i;

	for(;;)
	{
		c = get();
		if(c < '0' || c > '9')
		{
			if (c == '.') is_int = false;

			if (c) back();
			break;
		}
		i = 10 * int_val + c - '0';
		if (i < int_val)
		{
			// overflow
			is_int = false;
			break;
		}
		else int_val = i;
	}

	if (! is_int)
	{
		pos(p);

		char a[20];
		int index = 0;

		while((c >= '0' && c <= '9') || c == '.')
		{
			c = get();
			a[index++] = c;
			if (index == 19) break;
		}
		if (c) back();
		a[index] = 0;

		float_val = atof(a);
	}

	return is_int;
}

bool c_char_stream::eol()
{
	if (m_ncur_pos == m_nlength - 1) return true;
	if (m_pdata[m_ncur_pos] == '\r' || m_pdata[m_ncur_pos] == '\n') return true;

	return false;
}

bool c_char_stream::eot()
{
	DWORD curpos = m_ncur_pos;

	while(!eof())
	{
		if (eol())
		{
			pos (curpos);
			return false;
		}
		skip();
	}

	pos (curpos);
	return true;
}

bool c_char_stream::eof()
{
	return m_ncur_pos >= m_nlength;
}

void c_char_stream::lineend()
{
	while(!eol() && m_ncur_pos < m_nlength)
		++ m_ncur_pos;
}

bool c_char_stream::skipline()
{
	DWORD old_pos = m_ncur_pos;

	lineend();
	if(m_ncur_pos < m_nlength) return(!skip());
	pos(old_pos);
	
	return false;
}

void c_char_stream::set_buffer(LPCTSTR pdata)
{
	reset();
	if (m_pdata) free((void*)m_pdata);
	if (pdata == NULL)
	{
		m_pdata = " ";
	}
	else
		m_pdata = _strdup(pdata);
	// m_pdata 가 NULL 일때 exception 발생으로 예외처리 추가 할때 사용 20220211 jkh
	
	/*if (m_pdata == NULL)
	{
		m_pdata = " ";
		m_nlength = 1;
	}
	else if (m_pdata = "")
		m_nlength = 0;
	*/

	m_nlength = (DWORD)strlen(m_pdata);
}

DWORD c_char_stream::cur_line()
{
	return m_ncur_line;
}

DWORD c_char_stream::pos(LPCTSTR str)
{
	//
	//char * pstr = strstr(m_pdata + m_ncur_pos + 1, str);


	//20210325 warning
	char* c = (char*)(m_pdata + m_ncur_pos + 1);
	char* pstr = strstr(c, str);


	if (pstr == 0) return -1;
	return (DWORD)(pstr - m_pdata);
}
																																																																																																																																																																																																																																																																																																																																																																																																																																																										
void c_char_stream::set_temp_buffer(const char* p_str)
{
	if (m_ptemp_buffer)
	{
		_ASSERT(0);	// incorrect operation
		return;
	}

	reset(); // rewind to begin

	m_ptemp_buffer = m_pdata;
	m_pdata = _strdup(p_str);
	m_nlength = (DWORD)strlen(m_pdata);
}

void c_char_stream::free_temp_buffer()
{
	if (!m_ptemp_buffer)
	{
//		_ASSERT(0);	// incorrect operation
		return;
	}

	free((void*)m_pdata);
	m_pdata = m_ptemp_buffer;
	m_ptemp_buffer = 0;
	m_nlength = (DWORD)strlen(m_pdata);
}

void c_char_stream::get_line_context(int n, int* p_line, c_string* p_routine)
{
// go to line -----------------------------------------------------------------

	_ASSERT(!p_routine->get_length());// because we're not going to empty it

	if (!m_pdata) return;

	int n_line1 = 0;
	const char* p = m_pdata;
	while (*p)
	{
		if (*p == '\n') n_line1 ++;
		if (n_line1 == n)
		{
			p++; // ommit '\n'
			break;
		}
		p++;
	}

// get routine ----------------------------------------------------------------

	if (n_line1 < n)
	{
		// source too short
		_ASSERT(0);
		return;
	}

	int n_line2 = n_line1;
	while (p >= m_pdata)
	{
		// get line start

		while (p >= m_pdata)
		{
			if (*p == '\n')
			{
				break;
			}
			p--;
		}
		p ++;

		// routine start

		bool b_sub = (_strnicmp(p, "sub", 3) == 0) && isspace( *(p + 3));
		bool b_function = (_strnicmp(p, "function", 8) == 0) && isspace( *(p + 8));

		if (b_sub || b_function)
		{
			while (isalnum(*p)) p++; // ommit 'function' or 'sub'
			while (isspace(*p)) p++;

			while(isalnum(*p) || (*p == '_'))
			{
				*p_routine += *p;
				*p_line = n_line1 - n_line2;
				p++;
			}
			return;
		}

		// routine end

		bool b_end = (_strnicmp(p, "end", 3) == 0) && isspace( *(p + 3));
		if (b_end)
		{
			const char *_p = p;

			while (isalnum(*p)) p++; // ommit 'end'
			while (isspace(*p)) p++;

			if (_strnicmp(p, "sub", 3) == 0) if (n_line1 != n_line2) return;
			if (_strnicmp(p, "function", 8) == 0) if (n_line1 != n_line2) return;

			p = _p;
		}

		p--;
		p--;
		n_line2 --;
	}
}

void c_char_stream::get_line_context(int n_line, const char* p_routine, int *p_n)
{
	*p_n = -1;

// go to routine --------------------------------------------------------------

	if (!m_pdata) return;

	int n = 0;
	bool b_found = false;
	const char* p = m_pdata;
	while (*p)
	{
		if (*p == '\n')
		{
			p ++;
			n ++;
			while (*p && *p != '\n' && isspace(*p)) p++;

			if ( (!_strnicmp(p, "sub", 3) && !isalnum(*(p + 3))) ||
				 (!_strnicmp(p, "function", 8) && !isalnum(*(p + 8))) )
			{
				while (*p && (isalnum(*p) || *p == '-')) p++;
				while (*p && isspace(*p)) p++;
				if (!_strnicmp(p, p_routine, strlen(p_routine)))
				{
					b_found = true;
					break;
				}
			}
		}

		if (*p != '\n') p ++;
	}

	if (!b_found) return;

// go to line -----------------------------------------------------------------

	b_found = false; // means END found
	while (*p)
	{
		if (*p == '\n')
		{
			p ++;
			n ++;
			n_line --;
			if (!n_line) break;
			while (*p &&  *p != '\n' && isspace(*p)) p++;

			if (!_strnicmp(p, "end", 3))
			{
				while (*p && isspace(*p)) p++;
				if ( (!_strnicmp(p, "sub", 3) && !isalnum(*(p + 3))) ||
					 (!_strnicmp(p, "function", 8) && !isalnum(*(p + 8))) )
				{
					b_found = true;
					break;
				}
			}
		}

		if (*p != '\n') p ++;
	}

	if (!b_found) *p_n = n;
}