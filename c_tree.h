#ifndef C_TREE_H
#define C_TREE_H

#include "lex.h"
#include "s_table.h"

#include <string>
#include <iostream>
#include <cstdint>
#include <vector>
#include <stdexcept>
#include <limits>
#include <map>

class node
{
public:
	Token token;
	node* left;
	node* right;
	node* next;
	value_type val_type;
	int symbol_table_index;

	node(const Token& t);
	node(const Token& t, node* l, node* r);
	~node();

	int evaluate() const;
};

class parse
{
public:
	parse();
	~parse();
	node* parse_expression();
	node* parse_term();
	node* parse_factor();
	node* parse_arithmetic_expression();
	node* parse_relational_expression();
	node* parse_logical_or_expression();
	node* parse_logical_and_expression();
	node* parse_logical_not_expression();
	node* parse_if_statement();
	node* parse_statement_or_block();
	node* parse_while_statement();
	Token get_current_token();

	node* parse_statement();
	node* parse_print_statement();
	node* parse_read_statement();
	node* parse_assignment_statement();
	node* parse_decl_statement();

	void error(const string& e, int line, int col);

private:
	Token current_token;
	void next_token();
	void consume(token_id id);
};

extern vector<int> variable_values;

void print_tree(node* root, int space = 0);
int evaluate_statement_list(const node* stmt_head);

#endif