#ifndef C_CHAR_STREAM_H
#define C_CHAR_STREAM_H

#include <windows.h>
#include "c_string.h"
#include "token.h"

class c_char_stream  
{
	const char*	m_pdata;
	DWORD	m_nlength;
	DWORD	m_ncur_pos;
	DWORD	m_ncur_line;
public:
	c_char_stream();
	virtual ~c_char_stream();

	void init();

//	bool Load(LPCTSTR lpszFileName);
	void reset();

	DWORD pos(LPCTSTR str);				// find string and pos at it. return prev position or -1
	DWORD pos(DWORD newpos);			// goto new position. return previous
	DWORD pos() {return m_ncur_pos;}	// return current position
	char get();							// get char at current pos. shift to next char
	char check();						// get char at current pos. don't move
	bool skip();						// move to next char. false if eof
	bool back();						// move one char back if posible
	void gettok(c_token& curtok);

	bool _getnum(int& int_val, double& float_val);

	int getnum();
	void lineend();
	bool skipline();

	bool eol();
	bool eot();
	bool eof();

	const char* get_buffer() {return m_pdata;};
	void set_buffer(LPCTSTR pdata);

	bool is_empty() {return m_nlength == 0;}
	void get_line_context(int n, int* p_line, c_string* p_routine);
	void get_line_context(int n_line, const char* p_routine, int *p_n);

	DWORD cur_line();

// support for expression watch -----------------------------------------------

private:
	const char* m_ptemp_buffer;

public:
	void set_temp_buffer(const char* p_str);
	void free_temp_buffer();
};

#endif