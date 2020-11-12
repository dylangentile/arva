#define ARVA_INCLUDE_ENUM_STR_CRAP //must be first line
#include "lexer.h"
#include "error.h"
#include "token.h"

#include <cstdio>
#include <cstring>
#include <string>


inline bool is_alpha(const char);
inline bool is_numeric(const char);
inline bool is_symbol_valid(const char);
inline bool is_whitespace(const char);



Lexer::Lexer(){}
Lexer::~Lexer(){}


//this is probably the worst written code in this file.
// my apologies but for error printing this will probably get replaced in the future anyway
void
token_error(ErrorType type, Token* tok, const char* msg, ...)
{

	va_list args1, args2;
	va_start(args1, msg);
	va_copy(args2, args1);

	size_t s = vsnprintf(nullptr, 0, msg, args1) + 1; //include null terminator
	char* buf = (char*)calloc(s, sizeof(char));
	vsnprintf(buf, s, msg, args2);
	
	va_end(args2);

	const char* fmt_str = "\033[1m%s:%u:\033[91m %s\033[0m\n\t\t%s\n\t\t";

	s = snprintf(nullptr, 0, fmt_str, 
		tok->m_file_path, tok->m_line_num, (const char*)buf, tok->m_str.c_str()) + 1;
	char* final_msg = (char*)calloc(s + tok->m_str.size() + 1, sizeof(char));
	snprintf(final_msg, s, fmt_str, 
		tok->m_file_path, tok->m_line_num, (const char*)buf, tok->m_str.c_str());
	s--;
	final_msg[s] = '^';
	for(size_t i = 1; i < tok->m_str.size(); i++)
		final_msg[s + i] = '~';
	final_msg[s + tok->m_str.size()] = '\n';
	final_msg[s + tok->m_str.size() + 1] = '\0';
	
	Error::strf_error(type, (const char*)final_msg);
	
	free(final_msg);
	free(buf);

}

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

	m_file_path = path;
	prev_char = fetch_char();
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
Lexer::skip_whitespace(char* c)
{
	while(is_whitespace(*c))
	{
		*c = fetch_char();
	}
}

void
Lexer::fetch_token(Token* tok)
{
	tok->m_type = TokenType::NULLTYPE;
	tok->m_cat =  TokenCat::NULLCAT;
	tok->m_file_path = this->m_file_path;
	tok->m_str.clear();

	char c = prev_char;
	
	skip_whitespace(&c);

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

	while(c == '/' && peek() == '*')
	{
		while(!(c == '*' && peek() == '/'))
		{
			if((c = fetch_char()) == '\0')
			{
				Error::strf_error(ErrorType::Error, "Unterminated comment at EOF!");
				tok->m_cat = TokenCat::FileEnd;
				return;
			}
		}

		c = fetch_char();
		c = fetch_char(); //for prev_char
		skip_whitespace(&c);
	}

	skip_whitespace(&c);
	

	if(c == '\0')
	{
		tok->m_cat = TokenCat::FileEnd;
		return;
	}


	tok->m_line_num = m_current_line;
	tok->m_col_num = m_current_column;
	tok->m_foffset = m_current_offset;


	


	if(is_symbol_valid(c) && !is_numeric(c)) //cannot begin symbol with number
	{
		tok->m_cat = TokenCat::Symbol; 
		
		while(is_symbol_valid(c))
		{
			tok->m_str.push_back(c);
			c = fetch_char();
		}
		


		for(uint16_t i = (uint16_t)TokenType::COMPTIME; i != (uint16_t)TokenType::DECL_EQUAL; i++)
		{
			if(strcmp(tok_enum_to_string[i], tok->m_str.c_str()) == 0)
			{
				tok->m_cat = TokenCat::Keyword;
				tok->m_type = (TokenType)i;
				break;
			}
		}

		if(tok->m_cat == TokenCat::Symbol)
		{
			for(uint16_t i = (uint16_t)TokenType::INT8; i != (uint16_t)TokenType::COMPTIME; i++)
			{
				if(strcmp(tok_enum_to_string[i], tok->m_str.c_str()) == 0)
				{
					tok->m_cat = TokenCat::Type;
					tok->m_type = (TokenType)i;
					break;
				}
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
	else if(c == '@')
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
	else if(c == '\"')
	{
		c = fetch_char();
		while(c != '\"')
		{
			tok->m_str.push_back(c);
			if((c = fetch_char()) == '\0')
			{
				token_error(ErrorType::Fatal, tok, "Readed EOF before end of terminating quote!");
			}
		}

		c = fetch_char(); //for prev_char to be set properly
		//todo: implement escape parsing "\n \t \0" etc.
	}
	else
	{
		
		tok->m_str.push_back(c);
		tok->m_str.push_back(peek());
		for(uint16_t i = (uint16_t)TokenType::DECL_EQUAL; i != (uint16_t)TokenType::ARROW + 1; i++)
		{
			if(strcmp((const char*)tok->m_str.c_str(), tok_enum_to_string[i]) == 0)
			{
				tok->m_type = (TokenType)i;
				tok->m_cat = TokenCat::Operator;

				fetch_char(); //peek
				c = fetch_char(); //for prev_char
				break;
			}
		}

		if(tok->m_type == TokenType::NULLTYPE)
		{
			tok->m_str.clear();
			tok->m_str.push_back(c);
			for(uint16_t i = (uint16_t)TokenType::DECL_EQUAL; i != (uint16_t)TokenType::ARROW + 1; i++)
			{
				if(strcmp((const char*)tok->m_str.c_str(), tok_enum_to_string[i]) == 0)
				{
					tok->m_type = (TokenType)i;
					tok->m_cat = TokenCat::Operator;

					c = fetch_char(); //for prev_char
					break;
				}
			}
		}
		
		if(tok->m_type == TokenType::NULLTYPE)
		{
			tok->m_str.push_back(c);
			token_error(ErrorType::Fatal, tok, "Unknown character!");
		}
	}
	


	prev_char = c;


	


	
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

inline bool
is_whitespace(const char x)
{
	return x == ' ' || x == '\t' || x == '\n' || x == '\r';
}

