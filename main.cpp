#include "buffer.h"
#include "error.h"
#include "token.h"
#include "lex.h"
#include "c_tree.h"
#include <fstream>
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
	const char* filename = argv[1];
	Error e = lex_init(filename);
	parse parser;
    vector<instruction> machine_code;

	while (true)
	{
		node* root = parser.parse_expression();

		if (root != nullptr)
		{
			cout << "Code tree: " << endl;
			print_tree(root);

            double result = root->evaluate();

            size_t code_size = 0;
            machine_code.clear();
            generate_code(root, machine_code);

            cout << "Code size: " << code_size << " bytes." << endl;
            cout << "Code execution: " << endl << result << endl << "------------------------" << endl;

            cout << "Generated machine code:" << endl;
            for (const auto& instr : machine_code)
            {
                instr.print();
            }
            cout << "------------------------" << endl;
        }
        delete root;
    }

    lex_cleanup();
    return 0;
}