#include <iostream>
#include "error.h"

void print_error(const Error& e)
{
	switch (e.error)
	{
		case NCC_OK: cout << "No error"; break;
		case NCC_EOF: cout << "Error: End of file"; break;
		case NCC_UNEXPECTED_EOF: cout << "Error: Unexpected end of file"; break;
		case NCC_FILE_NOT_FOUND: cout << "Error: File not found"; break;
		case NCC_INVALID_CHAR: cout << "Error: Invalid character"; break;
	}
	if (e.error == NCC_UNEXPECTED_EOF)
	{
		cout << '\n';
	}
	else
	{
		cout << " at " << e.line << ":" << e.col << endl;
	}
}