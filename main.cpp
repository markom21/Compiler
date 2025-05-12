#include "buffer.h"
#include "error.h"
#include "token.h"
#include "lex.h"
#include "c_tree.h"
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char* argv[])
{
    const char* filename = argv[1];
    Error e = lex_init(filename);
    if (e.error != NCC_OK)
    {
        cerr << "Error initializing lexer for file: " << filename << endl;
        return 1;
    }

    parse parser;
    vector<node*> program_statements;

    while (parser.get_current_token().id != TOKEN_EOF)
    {
        node* statement_root = parser.parse_statement();

        if (statement_root != nullptr)
        {
            program_statements.push_back(statement_root);
        }
        else
        {
            if (parser.get_current_token().id != TOKEN_EOF)
            {
                parser.error("Expected a valid statement or EOF", parser.get_current_token().line, parser.get_current_token().col);
                break;
            }
        }
    }

    if (!sym_table.empty())
    {
        variable_values.resize(sym_table.size(), 0);
    }

    if (!program_statements.empty())
    {
        cout << "Code Tree:" << endl;
        cout << "statement block" << endl;

        for (node* statement_root : program_statements)
        {
            print_tree(statement_root, 2);
        }
        cout << "Code execution:" << endl;
        for (node* statement : program_statements)
        {
            statement->evaluate();
        }
    }
    else {
        cout << "No valid statements found in the input." << endl;
    }
    for (node* statement_root : program_statements)
    {
        delete statement_root;
    }
    program_statements.clear();

    lex_cleanup();
    return 0;
}