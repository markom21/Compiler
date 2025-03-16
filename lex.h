#ifndef  LEX_H
#define  LEX_H

#include "error.h"
#include "token.h"
#include "buffer.h"

#include <iostream>
#include <cctype>
#include <fstream>
#include <string>

Error lex_init(const char* src_code);

Error get_token(Token& t);

bool lex_eof(void);

void lex_cleanup();


#endif