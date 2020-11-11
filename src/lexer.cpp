#include "lexer.h"
#include "error.h"

#define ARVA_INCLUDE_ENUM_STR_CRAP
#include "token.h"

#include <cstdio>
#include <cstring>
#include <string>


inline bool is_alpha(const char);
inline bool is_numeric(const char);
inline bool is_symbol_valid(const char);



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
	m_current_fd_offset = (uint32_t)ftell(m_fd);	
	m_current_pointer = m_input_buffer;
}

//this function introduces overhead by using fread, but I didn't want to overcomplicate
// the code for the peek function, so this was the most eloquent solution.
char
Lexer::peek()
{
	char x = 0;
	
	fseek(m_fd, m_current_offset, 0);
	fread(&x, sizeof(x), 1, m_fd);
	fseek(m_fd, m_current_fd_offset, 0);

	return x;
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

	m_current_pointer = m_input_buffer + m_input_buffer_size; //primes fetch_char to request read_next

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
		m_current_line++; //todo: line number counting broken
	m_current_column++;
	m_current_offset++;

	return val;
}


void
Lexer::fetch_token(Token* tok)
{
	tok->m_type = TokenType::NULLTYPE;
	tok->m_cat =  TokenCat::NULLCAT;
	tok->m_str.clear();

	char c = fetch_char();
	
	if(c == '/' && peek() == '/')
	{
		while(c != '\n')
		{
			if((c = fetch_char()) == '\0')
			{
				tok->m_cat = TokenCat::FileEnd;
				return;
			}
		}

		c = fetch_char();
	}

	if(c == '/' && peek() == '*')
	{
		while(!(c == '*' && peek() == '/'))
		{
			if((c = fetch_char()) == '\0')
			{
				tok->m_cat = TokenCat::FileEnd;
				return;
			}
		}

		c = fetch_char();
	}

	if(c == '\0')
	{
		tok->m_cat = TokenCat::FileEnd;
		return;
	}



	if(is_symbol_valid(c) && !is_numeric(c)) //cannot begin symbol with number
	{
		tok->m_line_num = m_current_line;
		tok->m_col_num = m_current_column;
		tok->m_foffset = m_current_offset;
		tok->m_cat = TokenCat::Symbol; 
		
		while(is_symbol_valid(c))
		{
			tok->m_str.push_back(c);
			c = fetch_char();
		}
		
		for(uint16_t i = (uint16_t)TokenType::INT8; i != (uint16_t)TokenType::DECL_EQUAL; i++)
		{
			if(strcmp(tok_enum_to_string[i], tok->m_str.c_str()) == 0)
			{
				tok->m_cat = TokenCat::Keyword;
				tok->m_type = (TokenType)i;
			}
		}

	}
	else if(is_numeric(c))
	{
		char prev = c;
		c = fetch_char();
		
		if(prev == '0' && c == 'x')
		{
			fetch_char();
			while(is_numeric(c) || c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F'
				|| c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f')
			{
				tok->m_str.push_back(c);
				c = fetch_char();
			}

			tok->m_type = TokenType::UINT64;
		}
		else
		{
			tok->m_str.push_back(prev);
			while(is_numeric(c))
			{
				tok->m_str.push_back(c);
				c = fetch_char();
			}

			//todo: determine if it actually fits, if not make UINT64, or raise error!
			tok->m_type = TokenType::INT64;
		}

		tok->m_cat = TokenCat::Immediate;
		
	}

	if(c == '@')
	{
		tok->m_cat = TokenCat::BuiltIn;
		c = fetch_char();

		while(is_symbol_valid(c) && !is_numeric(c))
		{
			tok->m_str.push_back(c);
			c = fetch_char();
		}

		if(tok->m_str.size() == 0)
			Error::strf_error(ErrorType::Error, "Line:%u Invalid character after @: '%c'",  tok->m_line_num, c);
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
is_symbol_valid(const char x)
{
	return is_numeric(x) || is_alpha(x) || x == '_';
}

