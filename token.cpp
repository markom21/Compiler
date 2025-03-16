#include <iostream>
#include "token.h"

void print_token(const Token& t) 
{
    switch (t.id) 
    {
        case TOKEN_EOF: cout << "TOKEN_EOF"; break;
        case TOKEN_PLUS: cout << "TOKEN_PLUS"; break;
        case TOKEN_MINUS: cout << "TOKEN_MINUS"; break;
        case TOKEN_MULT: cout << "TOKEN_MULT"; break;
        case TOKEN_DIV: cout << "TOKEN_DIV"; break;
        case TOKEN_MOD: cout << "TOKEN_MOD"; break;
        case TOKEN_EXP: cout << "TOKEN_EXP"; break;
        case TOKEN_IDENT:
            cout << "TOKEN_IDENTIFIER: " << t.val;
            break;
        case TOKEN_LESS: cout << "TOKEN_LESS"; break;
        case TOKEN_LESS_EQ: cout << "TOKEN_LESS_EQ"; break;
        case TOKEN_GREATER: cout << "TOKEN_GREATER"; break;
        case TOKEN_GREATER_EQ: cout << "TOKEN_GREATER_EQ"; break;
        case TOKEN_EQUAL: cout << "TOKEN_EQUAL"; break;
        case TOKEN_NOT_EQUAL: cout << "TOKEN_NOT_EQUAL"; break;
        case TOKEN_ASSIGN: cout << "TOKEN_ASSIGN"; break;
        case TOKEN_NOT:  cout << "TOKEN_NOT"; break;
        case TOKEN_LPAREN: cout << "TOKEN_LPAREN"; break;
        case TOKEN_RPAREN: cout << "TOKEN_RPAREN"; break;
        case TOKEN_LBRACE: cout << "TOKEN_LBRACE"; break;
        case TOKEN_RBRACE: cout << "TOKEN_RBRACE"; break;
        case TOKEN_LBRACKET: cout << "TOKEN_LBRACKET"; break;
        case TOKEN_RBRACKET: cout << "TOKEN_RBRACKET"; break;
        case TOKEN_AND: cout << "TOKEN_AND"; break;
        case TOKEN_OR: cout << "TOKEN_OR"; break;
        case TOKEN_DOT: cout << "TOKEN_DOT"; break;
        case TOKEN_AT: cout << "TOKEN_AT"; break;
        case TOKEN_INTEGER:
            cout << "TOKEN_INTEGER: " << t.val;
            break;
        case TOKEN_STRING:
            cout << "TOKEN_STRING: " << t.val;
            break;
        case TOKEN_COLON: cout << "TOKEN_COLON"; break;
        case TOKEN_SEMICOLON: cout << "TOKEN_SEMICOLON"; break;
        case TOKEN_COMMA: cout << "TOKEN_COMMA"; break;
        case TOKEN_REAL: cout << "TOKEN_REAL"; break;
        case TOKEN_COMMENT: break;
        default: cout << "UNKNOWN_TOKEN"; break;
    }
    if (t.id == TOKEN_COMMENT)
    {
    }
    else
    {
        cout << " at " << t.line << ":" << t.col << endl;
    }
}
