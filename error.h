#ifndef ERROR_H
#define ERROR_H

#include <string>
using namespace std;

enum error_codes
{
	NCC_OK,
	NCC_EOF,
	NCC_UNEXPECTED_EOF,
	NCC_FILE_NOT_FOUND,
	NCC_INVALID_CHAR,
	NCC_INVALID_TOKEN
};


struct Error
{
	error_codes error;
	int line, col, int_val;
	char char_val;
	string string_val;
};

void print_error(const Error& e);

#endif
