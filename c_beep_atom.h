#ifndef C_BEEP_ATOM
#define C_BEEP_ATOM

#include "c_atom_table.h"

class c_beep_atom : public c_atom  
{
public:
	c_beep_atom(c_atom_table* ptable,
				c_call_stack* p_stack,
				c_engine* p_engine,
				int nline);

	virtual ~c_beep_atom();

	virtual void prepare_func_helper();
	virtual void exec_();
	virtual void enum_expressions(c_ptr_array<c_expression>* p_expressions);

#ifdef _DEBUG
	virtual void dump(FILE* f);
#endif
};

#endif