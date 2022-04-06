#pragma once

#include <cstdint>
#include <cstddef>
#include <cstdio>

struct Token;

class Lexer
{
public:
	Lexer();
	~Lexer();

	void initialize(const char* path);
	void terminate();
	
	void fetch_token(Token* tok);
	
private:
	void read_next();
	char fetch_char();
	char peek();
	void skip_whitespace(char*);

private:
	const size_t m_input_buffer_size = 0x200; //page size
	FILE*		m_fd			 = nullptr;
	char*		m_input_buffer   = nullptr;
	uint32_t	m_current_offset = 0;
	uint32_t	m_current_fd_offset = 0;
	bool		m_reached_end	 = false;

	uint32_t	m_current_line   = 1;
	uint32_t	m_current_column = 1;
	const char* m_current_pointer  = nullptr;

	const char* m_file_path;
	char prev_char;


};
