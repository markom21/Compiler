#ifndef BUFFER_H
#define BUFFER_H

#include <string>
#include <vector>
using std::string;
using namespace std;

extern int src_line_no;
extern int src_col_no;
extern vector<char> buffer;

int buffer_init(const char* filename);

int buffer_get_cur_char(char& c);

int buffer_next_char(void);

int buffer_get_next_char(char& c);

bool buffer_eof(void);

int buffer_back_char(void);

int buffer_cleanup(void);

int get_src_line(int line_no, string& line);

#endif