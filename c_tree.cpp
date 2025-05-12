#include "c_tree.h"

node::node(const Token& t) : token(t), left(nullptr), right(nullptr), next(nullptr), val_type(vt_null), symbol_table_index(-1) {}
node::node(const Token& t, node* l, node* r) : token(t), left(l), right(r), next(nullptr), val_type(vt_null), symbol_table_index(-1) {}
node::~node()
{
	delete left;
	delete right;
}

vector<int> variable_values;
extern vector<symbol_data> sym_table;

int evaluate_statement_list(const node* statement_head)
{
	int last_val = 0;
	const node* current = statement_head;
	while (current != nullptr)
	{
		if (variable_values.size() < sym_table.size())
		{
			variable_values.resize(sym_table.size(), 0);
		}
		last_val = current->evaluate();
		current = current->next;
	}
	return last_val;
}

int node::evaluate() const
{
	if (variable_values.size() < sym_table.size())
	{
		variable_values.resize(sym_table.size(), 0);
	}

	if (token.id == TOKEN_INTEGER)
	{
		return stoi(token.val);
	}
	if (token.id == TOKEN_TRUE)
	{
		return 1;
	}
	if (token.id == TOKEN_FALSE)
	{
		return 0;
	}
	if (token.id == TOKEN_STRING)
	{
		return 0;
	}

	if (token.id == TOKEN_IDENT)
	{
		if (symbol_table_index < 0 || symbol_table_index >= variable_values.size())
		{
			cerr << "Runtime Error: Invalid symbol table index " << symbol_table_index << " for " << token.val << endl;
			exit(1);
		}
		return variable_values[symbol_table_index];
	}
	if (token.id == TOKEN_PLUS)
	{
		return left->evaluate() + right->evaluate();
	}
	else if (token.id == TOKEN_MINUS)
	{
		if (left == nullptr && right != nullptr)
		{
			return -right->evaluate();
		}
		else if (left != nullptr && right != nullptr)
		{
			return left->evaluate() - right->evaluate();
		}
		else
		{
			cerr << "Runtime Error: Invalid structure for TOKEN_MINUS node at line " << token.line << endl;
			exit(1);
		}
	}
	if (token.id == TOKEN_MULT)
	{
		return left->evaluate() * right->evaluate();
	}
	if (token.id == TOKEN_DIV)
	{
		int right_val = right->evaluate();
		if (right_val == 0)
		{
			cerr << "Runtime Error: Division by zero at line " << token.line << endl;
			exit(1);
		}
		return left->evaluate() / right_val;
	}
	if (token.id == TOKEN_MOD)
	{
		int right_val = right->evaluate();
		if (right_val == 0) {
			cerr << "Runtime Error: Modulo by zero at line " << token.line << endl;
			exit(1);
		}
		return left->evaluate() % right_val;
	}

	if (token.id == TOKEN_LESS)
	{
		return left->evaluate() < right->evaluate() ? 1 : 0;
	}
	if (token.id == TOKEN_LESS_EQ)
	{
		return left->evaluate() <= right->evaluate() ? 1 : 0;
	}
	if (token.id == TOKEN_GREATER)
	{
		return left->evaluate() > right->evaluate() ? 1 : 0;
	}
	if (token.id == TOKEN_GREATER_EQ)
	{
		return left->evaluate() >= right->evaluate() ? 1 : 0;
	}
	if (token.id == TOKEN_EQUAL)
	{
		return left->evaluate() == right->evaluate() ? 1 : 0;
	}
	if (token.id == TOKEN_NOT_EQUAL)
	{
		return left->evaluate() != right->evaluate() ? 1 : 0;
	}
	if (token.id == TOKEN_AND)
	{
		return (left->evaluate() != 0 && right->evaluate() != 0) ? 1 : 0;
	}
	if (token.id == TOKEN_OR)
	{
		return (left->evaluate() != 0 || right->evaluate() != 0) ? 1 : 0;
	}
	if (token.id == TOKEN_NOT)
	{
		return left->evaluate() == 0 ? 1 : 0;
	}

	if (token.id == TOKEN_ASSIGN)
	{
		int target_var_index = left->symbol_table_index;
		int value_to_assign = right->evaluate();
		if (target_var_index < 0 || target_var_index >= variable_values.size())
		{
			exit(1);
		}
		variable_values[target_var_index] = value_to_assign;
		return value_to_assign;
	}

	if (token.id == TOKEN_PRINT)
	{
		node* expr = left;
		while (expr != nullptr)
		{
			if (expr->val_type == vt_bool)
			{
				cout << (expr->evaluate() ? "true" : "false");
			}
			else if (expr->val_type == vt_string)
			{
				if (expr->token.id == TOKEN_STRING)
				{
					cout << expr->token.val;
				}
			}
			else
			{
				cout << expr->evaluate();
			}
			expr = expr->next;
		}
		return 0;
	}

	if (token.id == TOKEN_READ)
	{
		if (left == nullptr || left->token.id != TOKEN_IDENT)
		{
			cerr << "Runtime Error: Invalid structure for read node at line " << token.line << endl;
			exit(1);
		}

		int target_var_index = left->symbol_table_index;

		if (target_var_index < 0 || target_var_index >= variable_values.size())
		{
			cerr << "Runtime Error: Invalid variable index (" << target_var_index << ") for read statement at line " << token.line << endl;
			exit(1);
		}

		int read_value;
		cin >> read_value;

		if (cin.fail()) {
			cerr << "\nRuntime Error: Invalid or missing integer input for read at line " << token.line << endl;
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			exit(1);
		}
		variable_values[target_var_index] = read_value;

		return 0;
	}

	if (token.id == TOKEN_INT4)
	{
		return 0;
	}

	if (token.id == TOKEN_IF)
	{
		node* condition = left;
		node* if_body = right;
		node* else_body = next;

		if (condition->evaluate() != 0)
		{
			return (if_body->token.id == TOKEN_BLOCK) ? evaluate_statement_list(if_body->left) : if_body->evaluate();
		}
		else if (else_body != nullptr)
		{
			return (else_body->token.id == TOKEN_BLOCK) ? evaluate_statement_list(else_body->left) : else_body->evaluate();
		}
		else {
			return 0;
		}
	}

	if (token.id == TOKEN_WHILE)
	{
		node* condition = left;
		node* body = right;
		int last_val = 0;
		while (condition->evaluate() != 0)
		{
			last_val = (body->token.id == TOKEN_BLOCK) ? evaluate_statement_list(body->left) : body->evaluate();
		}
		return last_val;
	}

	if (token.id == TOKEN_BLOCK)
	{
		return evaluate_statement_list(left);
	}

	cerr << "Runtime Error: Cannot evaluate node type: " << token.id << " ('" << token.val << "') at line " << token.line << endl;
	exit(1);
	return 0;
}

parse::parse()
{
	next_token();
}
parse::~parse()
{
}

void parse::next_token()
{
	Token t;
	Error e = get_token(t);
	if (e.error != NCC_OK)
	{
		error("failed to read next token", current_token.line, current_token.col);
	}
	if (t.id == TOKEN_EOF)
	{
		current_token = t;
		return;
	}
	current_token = t;
}

Token parse::get_current_token()
{
	return current_token;
}

void parse::consume(token_id id)
{
	if (current_token.id == TOKEN_EOF)
	{
		error("EOF", current_token.line, current_token.col);
	}
	if (current_token.id == id)
	{
		next_token();
	}
	else
	{
		string expected_token;
		if (id == TOKEN_SEMICOLON)
		{
			expected_token = ";";
		}
		else
		{
			expected_token = "some token";
		}
		error("Unexpected symbol", current_token.line, current_token.col);
	}
}

void parse::error(const string& e, int line, int col)
{
	cerr << e << " at " << line << ":" << col << endl;

	string source_line;
	if (get_src_line(line, source_line) == 0)
	{
		cerr << source_line << endl;
		for (int i = 0; i < col - 1; ++i)
		{
			cerr << "-";
		}
		cerr << "^" << endl;
	}
	else {
		cerr << "Could not retrieve source line" << endl;
	}
	exit(1);
}

node* parse::parse_expression()
{
	return parse_logical_or_expression();
}


node* parse::parse_term()
{
	node* this_node = parse_factor();

	while (current_token.id == TOKEN_MULT || current_token.id == TOKEN_DIV || current_token.id == TOKEN_MOD)
	{
		Token op = current_token;
		consume(op.id);
		node* right = parse_factor();
		if (this_node->val_type != vt_int4 || right->val_type != vt_int4)
		{
			error(op.val + "' must be int4", op.line, op.col);
		}

		this_node = new node(op, this_node, right);
		this_node->val_type = vt_int4;

	}
	return this_node;
}

node* parse::parse_factor()
{
	node* this_node = nullptr;

	if (current_token.id == TOKEN_LPAREN)
	{
		consume(TOKEN_LPAREN);
		this_node = parse_expression();
		if (current_token.id == TOKEN_RPAREN)
		{
			consume(TOKEN_RPAREN);
		}
		else
		{
			error("Expected ')'", current_token.line, current_token.col);
			return nullptr;
		}
	}
	else if (current_token.id == TOKEN_MINUS)
	{
		Token op = current_token;
		consume(TOKEN_MINUS);
		node* operand = parse_factor();

		if (operand == nullptr)
		{
			error("Expected expression after '-'", op.line, op.col);
			return nullptr;
		}
		if (operand->val_type != vt_int4)
		{
			error("Operand for '-' must be int4", op.line, op.col);
		}

		this_node = new node(op, nullptr, operand);
		this_node->val_type = vt_int4;
	}
	else if (current_token.id == TOKEN_INTEGER)
	{
		this_node = new node(current_token);
		consume(current_token.id);
		this_node->val_type = vt_int4;
	}
	else if (current_token.id == TOKEN_STRING) 
	{
		this_node = new node(current_token);
		consume(current_token.id);
		this_node->val_type = vt_string;
	}
	else if (current_token.id == TOKEN_TRUE || current_token.id == TOKEN_FALSE)
	{
		this_node = new node(current_token);
		consume(current_token.id);
		this_node->val_type = vt_bool;
	}
	else if (current_token.id == TOKEN_IDENT) 
	{
		this_node = new node(current_token);
		string var_name = current_token.val;
		int symbol_index = find(var_name);
		if (symbol_index == -1)
		{
			error("Undeclared variable '" + var_name + "'", current_token.line, current_token.col);
		}
		this_node->symbol_table_index = symbol_index;

		if (symbol_index != -1)
		{
			this_node->val_type = sym_table[symbol_index].val_type;
		}
		else
		{
			this_node->val_type = vt_null;
		}

		consume(TOKEN_IDENT);
	}
	else if (current_token.id == TOKEN_EOF)
	{
		error("Unexpected end of file encountered while parsing factor", current_token.line, current_token.col);
		return nullptr;
	}
	else
	{
		error("Unexpected token '" + current_token.val + "' encountered while parsing factor", current_token.line, current_token.col);
		return nullptr;
	}

	return this_node;
}

node* parse::parse_arithmetic_expression()
{
	node* this_node = parse_term();

	while (current_token.id == TOKEN_PLUS || current_token.id == TOKEN_MINUS)
	{
		Token op = current_token;
		consume(op.id);
		node* right = parse_term();

		this_node = new node(op, this_node, right);

		if (this_node->left->val_type == vt_int4 && this_node->right->val_type == vt_int4)
		{
			this_node->val_type = vt_int4;
		}
		else {
			this_node->val_type = vt_null;
		}
	}
	return this_node;
}

node* parse::parse_print_statement()
{
	consume(TOKEN_PRINT);
	consume(TOKEN_LPAREN);

	Token print_token;
	print_token.id = TOKEN_PRINT;
	print_token.val = "print";
	node* print_node = new node(print_token, nullptr, nullptr);

	node* first_expr = parse_expression();
	print_node->left = first_expr;
	node* current_expr = first_expr;

	while (current_token.id == TOKEN_COMMA)
	{
		consume(TOKEN_COMMA);
		node* next_expr = parse_expression();
		if (!next_expr)
		{
			error("Failed to parse argument after comma in print statement.", current_token.line, current_token.col);
		}
		current_expr->next = next_expr;
		current_expr = next_expr;
	}

	print_node->left = first_expr;

	consume(TOKEN_RPAREN);
	consume(TOKEN_SEMICOLON);

	return print_node;
}



node* parse::parse_read_statement()
{
	Token read_token = current_token;
	consume(TOKEN_READ);
	consume(TOKEN_LPAREN);

	if (current_token.id != TOKEN_IDENT)
	{
		error("Read statement expects identifier inside parentheses.", current_token.line, current_token.col);
		return nullptr;
	}

	Token ident_token = current_token;
	string var_name = ident_token.val;
	int symbol_index = find(var_name);

	if (symbol_index == -1)
	{
		error("Undeclared variable '" + var_name + "' used in read statement.", ident_token.line, ident_token.col);
		return nullptr;
	}

	if (sym_table[symbol_index].val_type != vt_int4) {
		error("Variable '" + var_name + "' in read statement not int4.", ident_token.line, ident_token.col);
		return nullptr;
	}

	node* var_node = new node(ident_token);
	var_node->symbol_table_index = symbol_index;
	var_node->val_type = vt_int4;


	consume(TOKEN_IDENT);
	consume(TOKEN_RPAREN);
	consume(TOKEN_SEMICOLON);

	node* read_node = new node(read_token, var_node, nullptr);
	read_node->val_type = vt_null;

	return read_node;
}

node* parse::parse_assignment_statement()
{
	if (current_token.id != TOKEN_IDENT)
	{
		error("Assignment statement must begin with identifier.", current_token.line, current_token.col);
	}

	node* var_node = new node(current_token);
	string var_name = current_token.val;
	int symbol_index = find(var_name);

	if (symbol_index == -1)
	{
		error("Undeclared variable '" + var_name + "'.", current_token.line, current_token.col);
	}
	var_node->symbol_table_index = symbol_index;
	consume(TOKEN_IDENT);

	if (current_token.id != TOKEN_ASSIGN)
	{
		error("Expected '<-' in assignment statement.", current_token.line, current_token.col);
	}

	Token assignToken = current_token;
	assignToken.val = "<-";
	consume(TOKEN_ASSIGN);

	node* expr_node = parse_expression();

	if (current_token.id != TOKEN_SEMICOLON)
	{
		error("Expected ';' at end of assignment statement.", current_token.line, current_token.col);
	}
	consume(TOKEN_SEMICOLON);

	node* assign_node = new node(assignToken, var_node, expr_node);

	return assign_node;
}

node* parse::parse_decl_statement()
{
	Token decl_token = current_token;
	consume(TOKEN_INT4);
	Token ident_token = current_token;
	string var_name = current_token.val;
	consume(TOKEN_IDENT);
	consume(TOKEN_SEMICOLON);

	if (find(var_name) != -1)
	{
		error("Duplicate symbol: " + var_name, ident_token.line, ident_token.col);
	}
	else
	{
		insert(var_name, symbol_var, vt_int4);
	}

	node* var_node = new node(ident_token);
	var_node->symbol_table_index = find(var_name);
	node* decl_node = new node(decl_token, var_node, nullptr);

	return decl_node;
}

node* parse::parse_relational_expression()
{
	node* left_node = parse_arithmetic_expression();

	while (current_token.id == TOKEN_LESS || current_token.id == TOKEN_LESS_EQ || current_token.id == TOKEN_GREATER || current_token.id == TOKEN_GREATER_EQ || current_token.id == TOKEN_EQUAL || current_token.id == TOKEN_NOT_EQUAL)
	{
		Token op = current_token;
		consume(op.id);
		node* right_node = parse_arithmetic_expression();
		if (left_node->val_type != vt_int4 || right_node->val_type != vt_int4)
		{
			error("Operands for relational operator '" + op.val + "' must be int4", op.line, op.col);
		}
		left_node = new node(op, left_node, right_node);
		left_node->val_type = vt_bool;
	}
	return left_node;
}

node* parse::parse_logical_or_expression()
{
	node* left_node = parse_logical_and_expression();
	while (current_token.id == TOKEN_OR)
	{
		Token op = current_token;
		consume(TOKEN_OR);
		node* right_node = parse_logical_and_expression();
		if (left_node->val_type != vt_bool || right_node->val_type != vt_bool)
		{
			error("Operands for '|' must be boolean", op.line, op.col);
		}
		left_node = new node(op, left_node, right_node);
		left_node->val_type = vt_bool;
	}
	return left_node;
}

node* parse::parse_logical_and_expression()
{
	node* left_node = parse_logical_not_expression();
	while (current_token.id == TOKEN_AND)
	{
		Token op = current_token;
		consume(TOKEN_AND);
		node* right_node = parse_logical_not_expression();
		if (left_node->val_type != vt_bool || right_node->val_type != vt_bool)
		{
			error("Operands for '&' must be boolean", op.line, op.col);
		}
		left_node = new node(op, left_node, right_node);
		left_node->val_type = vt_bool;
	}
	return left_node;
}

node* parse::parse_logical_not_expression()
{
	if (current_token.id == TOKEN_NOT)
	{
		Token op = current_token;
		consume(TOKEN_NOT);
		node* operand = parse_logical_not_expression();
		if (operand->val_type != vt_bool)
		{
			error("Operand for '!' must be boolean", op.line, op.col);
		}
		node* not_node = new node(op, operand, nullptr);
		not_node->val_type = vt_bool;
		return not_node;
	}
	return parse_relational_expression();
}

node* parse::parse_statement()
{
	switch (current_token.id)
	{
	case TOKEN_PRINT:
		return parse_print_statement();

	case TOKEN_READ:
		return parse_read_statement();

	case TOKEN_INT4:
		return parse_decl_statement();

	case TOKEN_IDENT:
		return parse_assignment_statement();

	case TOKEN_IF:
		return parse_if_statement();

	case TOKEN_WHILE:
		return parse_while_statement();

	case TOKEN_LBRACE:
		return parse_statement_or_block();

	case TOKEN_SEMICOLON:
		consume(TOKEN_SEMICOLON);

		return nullptr;

	default:
		error("Unexpected token '" + current_token.val + "' - Expected statement start", current_token.line, current_token.col);
		return nullptr;
	}
}

node* parse::parse_if_statement()
{
	Token if_token = current_token;
	consume(TOKEN_IF);
	consume(TOKEN_LPAREN);
	node* condition = parse_expression();
	consume(TOKEN_RPAREN);

	node* if_body = parse_statement_or_block();
	node* else_body = nullptr;

	if (current_token.id == TOKEN_ELSE)
	{
		consume(TOKEN_ELSE);

		if (current_token.id == TOKEN_IF)
		{
			else_body = parse_if_statement();
		}
		else
		{
			else_body = parse_statement_or_block();
		}
	}

	node* if_node = new node(if_token, condition, if_body);
	if (else_body)
	{
		if_node->next = else_body;
	}
	if_node->val_type = vt_null;
	return if_node;
}

node* parse::parse_statement_or_block()
{
	if (current_token.id == TOKEN_LBRACE)
	{
		consume(TOKEN_LBRACE);
		Token block_token = current_token;
		block_token.id = TOKEN_BLOCK;
		block_token.val = "{...}";

		node* statement_list_head = nullptr;
		node* statement_list_tail = nullptr;

		while (current_token.id != TOKEN_RBRACE && current_token.id != TOKEN_EOF)
		{
			node* statement = parse_statement();
			if (statement)
			{
				if (statement_list_head == nullptr)
				{
					statement_list_head = statement;
					statement_list_tail = statement;
				}
				else {
					statement_list_tail->next = statement;
					statement_list_tail = statement;
				}
				statement_list_tail->next = nullptr;
			}
			else {
				error("Expected statement or '}' inside block", current_token.line, current_token.col);
				break;
			}
		}
		consume(TOKEN_RBRACE);
		node* block_node = new node(block_token);
		block_node->left = statement_list_head;
		block_node->val_type = vt_null;
		return block_node;
	}
	else
	{
		return parse_statement();
	}
}

node* parse::parse_while_statement()
{
	Token while_token = current_token;
	consume(TOKEN_WHILE);
	consume(TOKEN_LPAREN);
	node* condition = parse_expression();
	if (condition->val_type != vt_bool)
	{
		error("While condition must be a boolean", while_token.line, while_token.col);
	}

	consume(TOKEN_RPAREN);
	node* body = parse_statement_or_block();

	node* while_node = new node(while_token, condition, body);
	while_node->val_type = vt_null;
	return while_node;
}

void print_tree(node* root, int space)
{
	if (root == nullptr)
	{
		return;
	}

	for (int i = 0; i < space; i++)
	{
		cout << " ";
	}

	if (root->token.id == TOKEN_PRINT)
	{
		cout << "print" << endl;
		node* current_arg = root->left;
		while (current_arg != nullptr)
		{
			print_tree(current_arg, space + 2);
			current_arg = current_arg->next;
		}
		return;
	}
	else if (root->token.id == TOKEN_IF)
	{
		cout << "if" << endl;
		if (root->left)
		{
			print_tree(root->left, space + 2);
		}
		if (root->right)
		{
			print_tree(root->right, space + 2);
		}
		if (root->next)
		{
			for (int i = 0; i < space; i++)
			{
				cout << " ";
		}
			cout << "else" << endl;
			print_tree(root->next, space + 2);
		}
		return;
	}
	else if (root->token.id == TOKEN_WHILE)
	{
		cout << "while" << endl;
		if (root->left)
		{
			print_tree(root->left, space + 2);
		}
		if (root->right)
		{
			print_tree(root->right, space + 2);
		}
		return;
	}
	else if (root->token.id == TOKEN_BLOCK)
	{
		cout << "{ Block }" << endl;
		node* current_statement = root->left;
		while (current_statement != nullptr)
		{
			print_tree(current_statement, space + 2);
			current_statement = current_statement->next;
		}
		return;
	}
	else if (root->token.id == TOKEN_READ)
	{
		cout << "read" << endl;
		if (root->left)
		{
			print_tree(root->left, space + 2);
		}
		return;
	}
	else if (root->token.id == TOKEN_ASSIGN)
	{
		cout << "<-" << endl;
		if (root->left)
		{
			print_tree(root->left, space + 2);
		}
		if (root->right)
		{
			print_tree(root->right, space + 2);
		}
		return;
	}
	else if (root->token.id == TOKEN_READ)
	{
		cout << "read" << endl;
		if (root->left) 
		{
			print_tree(root->left, space + 2);
		}
		return;
	}
	else if (root->token.id == TOKEN_PLUS)
	{
		cout << "+ (add)" << endl;
		if (root->left)
		{
			print_tree(root->left, space + 2);
		}
		if (root->right)
		{
			print_tree(root->right, space + 2);
		}
		return;
	}
	else if (root->token.id == TOKEN_MINUS)
	{
		if (root->left == nullptr && root->right != nullptr)
		{
			cout << "- (neg)" << endl;
			print_tree(root->right, space + 2);
		}
		else
		{
			cout << "- (sub)" << endl;
			print_tree(root->left, space + 2);
			print_tree(root->right, space + 2);
		}
		return;
	}
	else if (root->token.id == TOKEN_MULT)
	{
		cout << "* (mult)" << endl;
		if (root->left)
		{
			print_tree(root->left, space + 2);
		}
		if (root->right)
		{
			print_tree(root->right, space + 2);
		}
		return;
	}
	else if (root->token.id == TOKEN_DIV)
	{
		cout << "/ (div)" << endl;
		if (root->left)
		{
			print_tree(root->left, space + 2);
		}
		if (root->right)
		{
			print_tree(root->right, space + 2);
		}
		return;
	}
	else if (root->token.id == TOKEN_MOD)
	{
		cout << "mod" << endl;
		if (root->left)
		{
			print_tree(root->left, space + 2);
		}
		if (root->right)
		{
			print_tree(root->right, space + 2);
		}
		return;
	}
	else if (root->token.id == TOKEN_LESS)
	{
		cout << "<" << endl; if (root->left)
		{
			print_tree(root->left, space + 2);
		} 
		if (root->right)
		{
			print_tree(root->right, space + 2);
		} 
		return;
}
	else if (root->token.id == TOKEN_LESS_EQ)
	{
		cout << "<=" << endl;
		if (root->left)
		{
			print_tree(root->left, space + 2);
		}
		if (root->right)
		{
			print_tree(root->right, space + 2);
		}
		return;
	}
	else if (root->token.id == TOKEN_GREATER)
	{
		cout << ">" << endl;
		if (root->left)
		{
			print_tree(root->left, space + 2);
		}
		if (root->right)
		{
			print_tree(root->right, space + 2);
		}
		return;
	}
	else if (root->token.id == TOKEN_GREATER_EQ)
	{
		cout << ">=" << endl;
		if (root->left)
		{
			print_tree(root->left, space + 2);
		}
		if (root->right)
		{
			print_tree(root->right, space + 2);
		}
		return;
	}
	else if (root->token.id == TOKEN_EQUAL)
	{
		cout << "=" << endl;
		if (root->left)
		{
			print_tree(root->left, space + 2);
		}
		if (root->right)
		{
			print_tree(root->right, space + 2);
		}
		return;
	}
	else if (root->token.id == TOKEN_NOT_EQUAL)
	{
		cout << "~=" << endl;
		if (root->left)
		{
			print_tree(root->left, space + 2);
		}
		if (root->right)
		{
			print_tree(root->right, space + 2);
		}
		return;
	}
	else if (root->token.id == TOKEN_AND)
	{
		cout << "&" << endl;
		if (root->left)
		{
			print_tree(root->left, space + 2);
		}
		if (root->right)
		{
			print_tree(root->right, space + 2);
		}
		return;
	}
	else if (root->token.id == TOKEN_OR)
	{
		cout << "|" << endl;
		if (root->left)
		{
			print_tree(root->left, space + 2);
		}
		if (root->right)
		{
			print_tree(root->right, space + 2);
		}
		return;
	}
	else if (root->token.id == TOKEN_NOT)
	{
		cout << "!" << endl;
		if (root->left)
		{
			print_tree(root->left, space + 2);
		}
		return;
	}
	else if (root->token.id == TOKEN_INT4)
	{
		if (root->left && root->left->token.id == TOKEN_IDENT)
		{
			cout << "  variable: " << root->left->token.val << endl;
		}
		else
		{
			cout << "int4 declaration (unexpected structure)" << endl;
		}
		return;
		}
	{
		if (root->token.id == TOKEN_TRUE)
		{
			cout << "true" << endl;
		}
		else if (root->token.id == TOKEN_FALSE)
		{
			cout << "false" << endl;
		}
		else if (root->token.id == TOKEN_STRING)
		{
			if (root->token.val == "\n")
			{
				cout << endl;
			}
			else
			{
				cout << root->token.val << endl;
			}
		}
		else if (root->token.id == TOKEN_IDENT)
		{
			cout <<  "variable: " << root->token.val << endl;
		}
		else if (root->token.id == TOKEN_INTEGER)
		{
			cout << root->token.val << endl;
		}
		else
		{
			cout << root->token.val << " (token: " << root->token.id << ")" << endl;
		}
	}
}