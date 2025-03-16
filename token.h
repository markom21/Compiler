#ifndef TOKEN_H
#define TOKEN_H

#include <string>
using namespace std;

enum token_id
{
    TOKEN_EOF,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULT,
    TOKEN_DIV,
    TOKEN_MOD,
    TOKEN_EXP,
    TOKEN_IDENT,
    TOKEN_LESS,
    TOKEN_LESS_EQ,
    TOKEN_GREATER,
    TOKEN_GREATER_EQ,
    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,
    TOKEN_ASSIGN,
    TOKEN_NOT,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_DOT,
    TOKEN_AT,
    TOKEN_INTEGER,
    TOKEN_NEGATE,
    TOKEN_STRING,
    TOKEN_COLON,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    TOKEN_REAL,
    TOKEN_COMMENT,
    TOKEN_NULL
};

struct Token
{
    token_id id;
	int line, col;
	string val;
};

void print_token(const Token& t);

#endif
