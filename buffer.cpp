#include "buffer.h"
#include <fstream>
#include <iostream>
using namespace std;

int src_line_no = 1;
int src_col_no = 0;
vector<char> buffer;
size_t current_pos = 0;

int buffer_init(const char* filename)
{
	ifstream file(filename, ios::binary);
	if (!file.is_open())
	{
		cerr << "Error: File " << filename << " could not be opened" << endl;
		return -1;
	}
	
	buffer = vector<char>((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

	if (buffer.empty())
	{
		cerr << "Error: File " << filename << " is empty" << endl;
		return -2;
	}



	file.close();
	return 0;
}

int buffer_get_cur_char(char& c)
{
	if (buffer_eof())
	{
		c = '\0';
		return -1;
	}
	c = buffer[current_pos];
	return 0;
}

int buffer_next_char(void)
{
	if (buffer_eof())
	{
		return -1;
	}
	char c = buffer[current_pos];
	if (c == '\n')
	{
		src_line_no++;
		src_col_no = 1;
	}
	else
	{
		src_col_no++;
	}
	current_pos++;
	return 0;
}

int buffer_get_next_char(char& c)
{
	if (buffer_eof())
	{
		c = '\0';
		return -1;
	}
	c = buffer[current_pos];
	return buffer_next_char();
}


bool buffer_eof(void)
{
	return current_pos >= buffer.size();
}

int buffer_back_char(void)
{
	if (current_pos == 0)
	{
		return -1;
	}
	current_pos--;
	char c = buffer[current_pos];
	if (c == '\n')
	{
		src_line_no--;
		src_col_no = 1;
	}
	else
	{
		src_col_no--;
	}
	return 0;
}

int buffer_peek_next_char(char& c)
{
	if (current_pos >= buffer.size())
	{
		c = '\0';
		return -1;
	}
	c = buffer[current_pos];
	return 0;
}

int buffer_cleanup(void)
{
	buffer.clear();
	return 0;
}

int get_src_line(int line_no, string& line)
{
	int start_pos = -1;
	int end_pos = -1;
	int current_line = 1;

	for (int i = 0; i < buffer.size(); i++)
	{
		if (current_line == line_no && start_pos == -1)
		{
			start_pos = i;
		}
		if (current_line == line_no && (buffer[i] == '\n' || i == buffer.size() - 1))
		{
			end_pos = i;
			break;
		}
		if (buffer[i] == '\n')
		{
			current_line++;
		}
	}

	if (start_pos == -1 || end_pos == -1)
	{
		return -2;
	}

	line.assign(buffer.begin() + start_pos, buffer.begin() + end_pos);
	return 0;
}
