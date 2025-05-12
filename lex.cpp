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
	char c;
	t.val = "";
	t.id = TOKEN_NULL;
	t.line = src_line_no;
	t.col = src_col_no;

	while (true)
	{
		int e = buffer_peek_next_char(c);
		if (e != 0)
		{
			break;
		}

		if (c == ' ' || c == '\t' || c == '\r')
		{
			buffer_get_next_char(c);
			src_col_no++;
		}
		else if (c == '\n')
		{
			buffer_get_next_char(c);
			src_line_no++;
			src_col_no = 1;
		}
		else if (c == '#')
		{
			buffer_get_next_char(c);
			src_col_no++;

			while (true)
			{
				int get_result = buffer_get_next_char(c);
				if (get_result != 0)
				{
					t.id = TOKEN_EOF;
					t.line = src_line_no;
					t.col = src_col_no;
					return { NCC_OK, src_line_no, src_col_no };
				}
				src_col_no++;

				if (c == '\n')
				{
					src_line_no++;
					src_col_no = 1;
					break;
				}
			}
			continue;
		}
		else
		{
			break;
		}
	}

	t.line = src_line_no;
	t.col = src_col_no;

	if (buffer_peek_next_char(c) != 0)
	{
		t.id = TOKEN_EOF;
		return { NCC_OK, src_line_no, src_col_no };
	}

	if (buffer_get_next_char(c) != 0)
	{
		t.id = TOKEN_EOF;
		return { NCC_OK, src_line_no, src_col_no };
	}


	switch (c)
	{
	case '+':
	{
		t.id = TOKEN_PLUS;
		t.val = "+";
		return { NCC_OK, src_line_no, src_col_no };
	}
	case '-':
	{
		if (buffer_peek_next_char(c) == 0 && c == '-')
		{
			cerr << ("Invalid sequence '--'", t.line, t.col);
			return { NCC_INVALID_CHAR, src_line_no, src_col_no };
		}
		t.id = TOKEN_MINUS;
		t.val = "-";
		return { NCC_OK, src_line_no, src_col_no };
	}
	case '*':	t.id = TOKEN_MULT; break;
	case '/':	t.id = TOKEN_DIV; break;
	case '^':	t.id = TOKEN_EXP; break;
	case '<':
		t.id = TOKEN_LESS;
		t.val = "<";
		if (buffer_peek_next_char(c) == 0 && c == '=')
		{
			buffer_get_next_char(c);
			t.id = TOKEN_LESS_EQ;
			t.val = "<=";
		}
		else if (buffer_peek_next_char(c) == 0 && c == '-')
		{
			buffer_get_next_char(c);
			t.id = TOKEN_ASSIGN;
			t.val = "<-";
		}
		return { NCC_OK, src_line_no, src_col_no };
	case '>':
		t.id = TOKEN_GREATER;
		t.val = ">";
		if (buffer_peek_next_char(c) == 0 && c == '=')
		{
			buffer_get_next_char(c);
			t.id = TOKEN_GREATER_EQ;
			t.val = ">=";
		}
		return { NCC_OK, src_line_no, src_col_no };

	case '!':
		t.id = TOKEN_NOT;
		t.val = "!";
		if (buffer_peek_next_char(c) == 0 && c == '=')
		{
			buffer_get_next_char(c);
			t.id = TOKEN_NOT_EQUAL;
			t.val = "!=";
		}
		return { NCC_OK, src_line_no, src_col_no };
	case '(':
		t.id = TOKEN_LPAREN;
		t.val = "(";
		return { NCC_OK, src_line_no, src_col_no };
	case ')':
		t.id = TOKEN_RPAREN;
		t.val = ")";
		return { NCC_OK, src_line_no, src_col_no };
	case '{':	t.id = TOKEN_LBRACE; break;
	case '}':	t.id = TOKEN_RBRACE; break;
	case '[':	t.id = TOKEN_LBRACKET; break;
	case ']':	t.id = TOKEN_RBRACKET; break;
	case '&': t.id = TOKEN_AND; t.val = "&"; return { NCC_OK, src_line_no, src_col_no };
	case '|':	t.id = TOKEN_OR; t.val = "|"; return { NCC_OK, src_line_no, src_col_no };
	case '.':	t.id = TOKEN_DOT; break;
	case '@':	t.id = TOKEN_AT; break;
	case ':':	t.id = TOKEN_COLON; break;
	case ';':	t.id = TOKEN_SEMICOLON; break;
	case ',':	t.id = TOKEN_COMMA; break;
	case '"':
		t.id = TOKEN_STRING;
		t.val = "";

		char next_char;

		while (buffer_peek_next_char(c) == 0)
		{
			buffer_get_next_char(c);

			if (c == '"')
			{
				break;
			}
			else if (c == '\\')
			{
				if (buffer_peek_next_char(next_char) != 0)
				{
					cerr << ("wrong escape sequence at end of file in string literal", t.line, t.col + t.val.length());

					break;
				}
				buffer_get_next_char(next_char);

				switch (next_char)
				{
				case 'n':  t.val += '\n'; break;
				case 't':  t.val += '\t'; break;
				case '\\': t.val += '\\'; break;
				case '"':  t.val += '"';  break;
				default:

					cerr << ("wrong escape sequence '\\" + string(1, next_char) + "' in string literal", t.line, t.col + t.val.length());

					break;
				}
			}
			else
			{
				t.val += c;
			}
		}
		if (c != '"')
		{
			cerr << ("string literal error", t.line, t.col);
		}
		return { NCC_OK, t.line, t.col };
	case '~':
		if (buffer_peek_next_char(c) == 0 && c == '=')
		{
			buffer_get_next_char(c);
			t.id = TOKEN_NOT_EQUAL;
			t.val = "~=";
		}
		else
		{
			return { NCC_INVALID_CHAR, src_line_no, src_col_no };
		}
		return { NCC_OK, src_line_no, src_col_no };
	case '=':
		t.id = TOKEN_EQUAL;
		t.val = "=";
		return { NCC_OK, src_line_no, src_col_no };
	default:
		if (isalpha(c))
		{
			t.val = c;
			while (buffer_get_next_char(c) == 0 && (isalnum(c) || c == '_'))
			{
				t.val += c;
			}
			buffer_back_char();

			if (t.val == "true")
			{
				t.id = TOKEN_TRUE;
			}
			else if (t.val == "false")
			{
				t.id = TOKEN_FALSE;
			}
			else if (t.val == "mod")
			{
				t.id = TOKEN_MOD;
			}
			else if (t.val == "if")
			{
				t.id = TOKEN_IF;
			}
			else if (t.val == "else")
			{
				t.id = TOKEN_ELSE;
			}
			else if (t.val == "while")
			{
				t.id = TOKEN_WHILE;
			}
			else if (t.val == "print")
			{
				t.id = TOKEN_PRINT;
			}
			else if (t.val == "read")
			{
				t.id = TOKEN_READ;
			}
			else if (t.val == "int4")
			{
				t.id = TOKEN_INT4;
			}
			else
			{
				t.id = TOKEN_IDENT;
			}
			return { NCC_OK, src_line_no, src_col_no };
		}
		if (isdigit(c))
		{
			t.id = TOKEN_INTEGER;
			t.val = "";
			t.val = c;
			int result = buffer_get_next_char(c);
			while (result == 0 && isdigit(c))
			{
				t.val += c;
				result = buffer_get_next_char(c);
			}
			buffer_back_char();
			return { NCC_OK, src_line_no, src_col_no };
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
