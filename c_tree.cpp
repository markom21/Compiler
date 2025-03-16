#include "c_tree.h"

node::node(const Token& t) : token(t), left(nullptr), right(nullptr) {}
node::node(const Token& t, node* l, node* r) : token(t), left(l), right(r) {}
node::~node()
{
	delete left;
	delete right;
}

int node::evaluate() const
{
	if (token.id == TOKEN_INTEGER || token.id == TOKEN_REAL)
	{
		return stod(token.val);
	}
	if (token.id == TOKEN_PLUS)
	{
		return left->evaluate() + right->evaluate();
	}
	else if (token.id == TOKEN_MINUS)
	{
		if (left == nullptr)
		{
			return -right->evaluate();
		}
		else
		{
			return left->evaluate() - right->evaluate();
		}
	}
	else if (token.id == TOKEN_MULT)
	{
		return left->evaluate() * right->evaluate();
	}
	else if (token.id == TOKEN_DIV)
	{
		return left->evaluate() / right->evaluate();
	}
	else if (token.id == TOKEN_MOD)
	{
		return left->evaluate() % right->evaluate();
	}
	cerr << "Unexpected token in evaluation" << endl;
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
		error("failed to read next token");
	}
	if (t.id == TOKEN_EOF)
	{
		current_token = t;
		return;
	}
	current_token = t;
}

Token parse::get_current_token() const
{
	return current_token;
}

void parse::consume(token_id id)
{
	if (current_token.id == TOKEN_EOF)
	{
		error("EOF"); //change later
	}
	if (current_token.id == id)
	{
		next_token();
	}
	else
	{
		error("unexpected token in consume()"); //change with error.h later
	}
}

void parse::error(const string& e)
{
	cerr << "error function called: " << e << endl;
	exit(1);
}

node* parse::parse_expression()
{
	node* this_node = parse_term();
	while (current_token.id == TOKEN_PLUS || current_token.id == TOKEN_MINUS)
	{
		Token op = current_token;
		consume(op.id);
		node* right = parse_term();
		this_node = new node(op, this_node, right);
	}
	return this_node;
}

node* parse::parse_term()
{
	node* this_node = parse_factor();
	while (current_token.id == TOKEN_MULT || current_token.id == TOKEN_DIV || current_token.id == TOKEN_MOD)
	{
		Token op = current_token;
		consume(op.id);
		node* right = parse_factor();
		this_node = new node(op, this_node, right);
	}
	return this_node;
}

node* parse::parse_factor()
{
	node* this_node = nullptr;
	if (current_token.id == TOKEN_LPAREN)
	{
		consume(current_token.id);
		this_node = parse_expression();
		consume(TOKEN_RPAREN);
	}
	else if (current_token.id == TOKEN_MINUS)
	{
		Token op = current_token;
		consume(TOKEN_MINUS);
		node* right = parse_factor();
		this_node = new node(op, nullptr, right);
	}
	else if (current_token.id == TOKEN_INTEGER || current_token.id == TOKEN_REAL)
	{
		this_node = new node(current_token);
		consume(current_token.id);
	}
	else if (current_token.id == TOKEN_EOF)
	{
		exit(1);
	}
	else
	{
		error("unexpected token in parse_function()");
		exit(1);
	}
	return this_node;
}

void print_tree(node* root, int space)
{
	if (root == nullptr)
	{
		return;
	}
	space += 1;
	for (int i = 0; i < space; i++)
	{
		cout << " ";
	}
	if (root->token.id == TOKEN_PLUS)
	{
		cout << "+ (add)" << endl;
	}
	else if (root->token.id == TOKEN_MINUS)
	{
		cout << "- (sub)" << endl;
	}
	else if (root->token.id == TOKEN_MULT)
	{
		cout << "* (mult)" << endl;
	}
	else if (root->token.id == TOKEN_DIV)
	{
		cout << "/ (div)" << endl;
	}
	else if (root->token.id == TOKEN_MOD)
	{
		cout << "mod" << endl;
	}
	else if (root->token.id == TOKEN_NEGATE)
	{
		cout << "neg" << endl;
	}
	else
	{
		cout << root->token.val << endl;
	}
	if (root->left)
	{
		print_tree(root->left, space);
	}
	if (root->right)
	{
		print_tree(root->right, space);
	}
}

instruction::instruction(const std::string& op, const std::string& operand1, const std::string& operand2) : op(op), operand1(operand1), operand2(operand2) {}

void instruction::print() const
{
	cout << op;
	if (!operand1.empty())
	{
		cout << " " << operand1;
	}
	if (!operand2.empty())
	{
		cout << ", " << operand2;
	}
	cout << endl;
}

const std::string& instruction::get_op()
{
	return op;
}

const std::string& instruction::get_operand1()
{
	return operand1;
}

const std::string& instruction::get_operand2()
{
	return operand2;
}

void generate_code(node* current_node, std::vector<instruction>& machine_code) {
	if (current_node == nullptr) {
		return;
	}
	if (current_node->token.id == TOKEN_INTEGER)
	{
		machine_code.push_back(instruction("PUSH", current_node->token.val));
		return;
	}

	generate_code(current_node->left, machine_code);
	generate_code(current_node->right, machine_code);

	machine_code.push_back(instruction("POP", "EBX"));
	machine_code.push_back(instruction("POP", "EAX"));

	switch (current_node->token.id)
	{
	case TOKEN_PLUS:
		machine_code.push_back(instruction("ADD", "EAX, EBX"));
		break;
	case TOKEN_MINUS:
		machine_code.push_back(instruction("SUB", "EAX, EBX"));
		break;
	case TOKEN_MULT:
		machine_code.push_back(instruction("IMUL", "EAX, EBX"));
		break;
	case TOKEN_DIV:
		machine_code.push_back(instruction("CDQ"));
		machine_code.push_back(instruction("IDIV", "EBX"));
		break;
	case TOKEN_MOD:
		machine_code.push_back(instruction("CDQ"));
		machine_code.push_back(instruction("IDIV", "EBX"));
		machine_code.push_back(instruction("MOV", "EAX, EDX"));
		break;
	case TOKEN_NEGATE:
		machine_code.push_back(instruction("NEG", "EAX"));
		break;
	default:
		break;
	}

	machine_code.push_back(instruction("PUSH", "EAX"));
}

