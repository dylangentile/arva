#include "lexer.h"
#include "error.h"

#define ARVA_INCLUDE_ENUM_STR_CRAP
#include "token.h"

#include <cstdio>
#include <cstring>
#include <string>


inline bool is_alpha(const char);
inline bool is_numeric(const char);
inline bool is_identifier_valid(const char);



Lexer::Lexer(){}
Lexer::~Lexer(){}


void
Lexer::read_next()
{
	memset(m_input_buffer, 0, sizeof(char)*m_input_buffer_size);

	if(m_reached_end)
		return;

	size_t result = fread(m_input_buffer, sizeof(char), m_input_buffer_size, m_fd);
	if(result != m_input_buffer_size && !feof(m_fd))
		Error::strf_error(ErrorType::Fatal, "Failed to read from file!");
	else
		m_reached_end = true;

	//todo: could ftell be negative?
	m_current_offset = (uint32_t)ftell(m_fd);	
	m_current_pointer = m_input_buffer;
}


void
Lexer::init_lexer(const char* path)
{
	m_fd = fopen(path, "r");
	if(m_fd == nullptr)
		Error::strf_error(ErrorType::Fatal, "The file at path: '%s', could not be opened!", path);

	m_input_buffer = (char*)calloc(m_input_buffer_size, sizeof(char));
	if(m_input_buffer == nullptr)
		Error::strf_error(ErrorType::Fatal, "Failed to allocate %X bytes of memory!", m_input_buffer_size);

	m_current_pointer = m_input_buffer;

}

void
Lexer::destroy()
{
	free(m_input_buffer);
	m_input_buffer = nullptr;
	fclose(m_fd);
	m_fd = nullptr;

}

char
Lexer::fetch_char()
{
	if(m_current_pointer == m_input_buffer + m_input_buffer_size)
		read_next();

	char val;
	val = *m_current_pointer;
	m_current_pointer++;
	
	if(val == '\n')
		m_current_line++;
	m_current_column++;
	m_current_offset++;

	return val;
}


void
Lexer::fetch_token(Token* tok)
{
	//memset(tok, 0, sizeof(*tok));

	char c = fetch_char();
	if(c == '\0')
	{
		tok->m_cat = TokenCat::EOF;
		return;
	}

	if(is_identifier_valid(c) && !is_numeric(c)) //cannot begin identifier with number
	{
		tok->m_line_num = m_current_line;
		tok->m_col_num = m_current_column;
		tok->m_foffset = m_current_offset;

		while(is_identifier_valid(c))
			tok->m_str.push_back(c);
		
		for(uint16_t i = TokenType::INT8; i != TokenType::DECL_EQUAL; i++)
		{
			if(strcmp(tok_enum_to_string[i], tok->m_str.c_str()))
			{
				tok->m_cat = TokenCat::Keyword;
				tok->m_type = (TokenType)i;
			}
		}

	}
	else if(is_numeric(c))
	{

	}



	
}

inline bool 
is_alpha(const char x)
{
	return ('a' <= x && x <= 'z') || ('A' <= x && x <= 'Z');
}

inline bool 
is_numeric(const char x)
{
	return '0' <= x && x <= '9';
}

inline bool 
is_identifier_valid(const char x)
{
	return is_numeric(x) || is_alpha(x) || x == '_';
}

