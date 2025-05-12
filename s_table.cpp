#include "s_table.h"

vector<symbol_data> sym_table;
vector<string> string_table;

int add_string_constant(const std::string& val)
{
	for (int i = 0; i < (int)string_table.size(); i++)
	{
		if (string_table[i] == val)
		{
			return i;
		}
	}
	string_table.push_back(val);
	return (int)string_table.size() - 1;
}

int insert(const string& name, symbol_type stype, value_type vtype)
{
	symbol_data new_sym;
	new_sym.name = name;
	new_sym.sym_type = stype;
	new_sym.loc_type = loc_stack;
	new_sym.val_type = vtype;
	new_sym.offset = -4 * (sym_table.size() + 1);

	sym_table.push_back(new_sym);
	return sym_table.size() - 1;
}

int find(const string& name)
{
	for (size_t i = 0; i < sym_table.size(); ++i)
	{
		if (sym_table[i].name == name)
		{
			return i;
		}
	}
	return -1;
}