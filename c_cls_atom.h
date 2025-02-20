#ifndef C_CLS_ATOM_H
#define C_CLS_ATOM_H

#include "c_atom_table.h"
#include "Terminal.h"

class c_cls_atom : public c_atom  
{
public:
	c_cls_atom(c_atom_table* ptable,
				c_call_stack* p_stack,
				c_engine* p_engine,
				int nline);

	virtual ~c_cls_atom();

	virtual void prepare_func_helper();
	virtual void exec_();
	virtual void enum_expressions(c_ptr_array<c_expression>* p_expressions);

#ifdef _DEBUG
	virtual void dump(FILE* f);
	virtual void assert_valid();
#endif
};

#endif