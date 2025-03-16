#ifndef C_TREE_H
#define C_TREE_H

#include "lex.h"

#include <string>
#include <iostream>

class node
{
public:
	Token token;
	node* left;
	node* right;

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
	Token get_current_token() const;
private:
	Token current_token;
	void next_token();
	void consume(token_id id);
	void error(const string& e);
};

class instruction
{
public:

	instruction(const string& operation, const string& operand1 = "", const string& operand2 = "");
	void print() const;
	const string& get_op();
	const string& get_operand1();
	const string& get_operand2();

private:
	string op;
	string operand1;
	string operand2;
};

extern vector<instruction> machine_code;
void print_tree(node* root, int space = 0);
void generate_code(node* this_node, vector<instruction>& machine_code);

#endif