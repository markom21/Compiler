#ifndef S_TABLE_H
#define S_TABLE_H

#include <iostream>
#include <vector>
using namespace std;

extern vector<string> string_table;
int add_string_constant(const std::string& val);

enum symbol_type
{
	symbol_null,
	symbol_var
};

enum location_type
{
	loc_stack,
	loc_mem,
	loc_reg
};

enum value_type
{
	vt_null,
	vt_int4,
	vt_string,
	vt_bool
};

struct symbol_data
{
	string name;
	symbol_type sym_type;
	location_type loc_type;
	value_type val_type;
	int offset;
};

extern vector<symbol_data> sym_table;

int insert(const string& name, symbol_type stype, value_type vtype);
int find(const string& name);

#endif