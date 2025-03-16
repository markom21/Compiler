#include "lex.h"
using namespace std;

char c;

Error lex_init(const char* src_code)
{
	int result = buffer_init(src_code);
	if (result != 0)
	{
		return { NCC_FILE_NOT_FOUND, 0, 0 };
	}
	return { NCC_OK, src_line_no, src_col_no };

}

Error get_token(Token& t)
{
	if (buffer_eof())
	{
		t.id = TOKEN_EOF;
		return { NCC_OK, src_line_no, src_col_no };
	}
	int e = buffer_get_next_char(c);
	if (e == -1)
	{
		src_line_no++;
		src_col_no = 1;
		t.line = src_line_no;
		t.col = src_col_no;

		t.id = TOKEN_EOF;
		return { NCC_EOF, src_line_no, src_col_no };
	}
	while (isspace(c))
	{
		e = buffer_get_next_char(c);
		if (e == -1)
		{
			src_line_no++;
			src_col_no = 1;
			t.line = src_line_no;
			t.col = src_col_no;

			t.id = TOKEN_EOF;
			return { NCC_EOF, src_line_no, src_col_no };
		}
	}

	t.line = src_line_no;
	t.col = src_col_no;

	bool last_op = true;

	switch (c)
	{
		case '+':
		{
			int next_char = buffer_get_next_char(c);
			if (isdigit(c) || c == '(' || isalpha(c))
			{
				t.id = TOKEN_PLUS;
				buffer_back_char();
				return { NCC_OK, src_line_no, src_col_no };
			}
			t.id = TOKEN_PLUS;
			buffer_back_char();
			break;
		}
		case '-':
		{
			int next_char = buffer_get_next_char(c);
			if (isdigit(c))
			{
				t.id = TOKEN_INTEGER;
				t.val = "-" + std::string(1, c);

				while (buffer_get_next_char(c) == 0 && isdigit(c)) {
					t.val += c;
				}
				buffer_back_char();
				last_op = false;
				return { NCC_OK, src_line_no, src_col_no };
			}
			else
			{
				buffer_back_char();
				if (last_op)
				{
					t.id = TOKEN_NEGATE;
				}
				else
				{
					t.id = TOKEN_MINUS;
				}
			}
			last_op = true;
			break;
		}
		case '*':	t.id = TOKEN_MULT; break;
		case '/':	t.id = TOKEN_DIV; break;
		case '^':	t.id = TOKEN_EXP; break;
		case '<':	
			t.id = TOKEN_LESS;
			if (buffer_get_next_char(c) == 0 && c == '<')
			{
				if (buffer_get_next_char(c) == 0 && c == '-')
				{
					t.id = TOKEN_COMMENT;
					bool end = false;

					while (buffer_get_next_char(c) == 0)
					{
						if (c == '-' && buffer_get_next_char(c) == 0 && '>')
						{
							end = true;
							break;
						}
					}
					if (!end)
					{
						src_line_no--;
						return { NCC_UNEXPECTED_EOF, src_line_no, src_col_no };
					}
					buffer_next_char();
					return { NCC_OK, src_line_no, src_col_no };
				}
			}
			if (buffer_get_next_char(c) == 0 && c == '=')
			{
				t.id = TOKEN_LESS_EQ;
				return { NCC_OK, src_line_no, src_col_no };
			}
			if (buffer_get_next_char(c) == 0 && c == '-')
			{
				t.id = TOKEN_ASSIGN;
				return { NCC_OK, src_line_no, src_col_no };
			}
			buffer_back_char();
			break;
		case '>':
			t.id = TOKEN_GREATER;
			if (buffer_get_next_char(c) == 0 && c == '=')
			{
				t.id = TOKEN_GREATER_EQ;
				return { NCC_OK, src_line_no, src_col_no };
			}
			buffer_back_char();
			break;
		case '=':	t.id = TOKEN_EQUAL; break;
		case '!':	
			t.id = TOKEN_NOT;
			if (buffer_get_next_char(c) == 0 && c == '=')
			{
				t.id = TOKEN_NOT_EQUAL;
				return { NCC_OK, src_line_no, src_col_no };
			}
			buffer_back_char();
			break;
		case '(':	t.id = TOKEN_LPAREN; break;
		case ')':	t.id = TOKEN_RPAREN; break;
		case '{':	t.id = TOKEN_LBRACE; break;
		case '}':	t.id = TOKEN_RBRACE; break;
		case '[':	t.id = TOKEN_LBRACKET; break;
		case ']':	t.id = TOKEN_RBRACKET; break;
		case '&':	t.id = TOKEN_AND; break;
		case '|':	t.id = TOKEN_OR; break;
		case '.':	t.id = TOKEN_DOT; break;
		case '@':	t.id = TOKEN_AT; break;
		case ':':	t.id = TOKEN_COLON; break;
		case ';':	t.id = TOKEN_SEMICOLON; break;
		case ',':	t.id = TOKEN_COMMA; break;
		case '\\':
			while (buffer_get_next_char(c) == 0)
			{
				if (c == '"')
				{
					break;
				}
				if (c == '\\')
				{
					buffer_get_next_char(c);
					switch(c)
					{
						case 'n':
							t.val += '\n';
							break;
						case 't':
							t.val += '\t';
							break;
						case '\'':
							t.val += '\'';
							break;
						case '"':
							t.val += '"';
							break;
					}
				}
				else
				{
					t.val += c;
				}
			}
			t.id = TOKEN_STRING;
			buffer_back_char();
			return { NCC_OK, src_line_no, src_col_no };
		case '"':
			t.val.clear();
			while (buffer_get_next_char(c) == 0)
			{
				if (c == '"')
				{
					break;
				}
				else
				{
					t.val += c;
				}
			}
			t.id = TOKEN_STRING;
			buffer_back_char();
			return { NCC_OK, src_line_no, src_col_no };
			break;
		case '#':
			while (buffer_get_next_char(c) == 0 && c != '\n')
			{

			}
			t.id = TOKEN_COMMENT;
			buffer_next_char();
			return { NCC_OK, src_line_no, src_col_no };
		default:
			if (isdigit(c))
			{
				t.id = TOKEN_INTEGER;
				t.val = c;
				while (buffer_get_next_char(c) == 0 && isdigit(c))
				{
					t.val += c;
				}
				buffer_back_char();
				return { NCC_OK, src_line_no, src_col_no };
			}
			if (c == 'm')
			{
				char c2, c3;
				if (buffer_get_next_char(c2) == 0 && c2 == 'o' && buffer_get_next_char(c3) == 0 && c3 == 'd')
				{
					t.id = TOKEN_MOD;
					t.val = "mod";
					return { NCC_OK, src_line_no, src_col_no };
				}
				else
				{
					buffer_back_char();
					buffer_back_char();
					buffer_back_char();
				}
			}
			if (c == '_')
			{
				t.id = TOKEN_IDENT;
				t.val = c;
				while (buffer_get_next_char(c) == 0 && isalnum(c) || c == '_')
				{
					t.val += c;
				}
				buffer_back_char();
				return { NCC_OK, src_line_no, src_col_no };
			}
			t.id = TOKEN_NULL;
			return { NCC_INVALID_CHAR, src_line_no, src_col_no };
	}
	return { NCC_OK, src_line_no, src_col_no };
}

bool lex_eof(void)
{
	return buffer_eof();
}

void lex_cleanup()
{
	buffer_cleanup();
}
