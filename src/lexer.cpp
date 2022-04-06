#define ARVA_INCLUDE_ENUM_STR //must be first line
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
Lexer::initialize(const char* path)
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
Lexer::terminate()
{
	free(m_input_buffer);
	m_input_buffer = nullptr;
	fclose(m_fd);
	m_fd = nullptr;

	m_current_offset = 0;
	m_line_offset = 0;
	m_current_fd_offset = 0;
	m_reached_end = false;

	m_current_line = 1;
	m_current_column = 1;
	m_current_pointer = nullptr;
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
	{
		m_line_offset = m_current_offset + 1;
		m_current_line++; //todo: line number counting broken
	}
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
	tok->type = TokenType::NULLTYPE;
	tok->cat =  TokenCat::NULLCAT;
	tok->debug.file_path = this->m_file_path;
	tok->str.clear();

	struct DeferThing
	{
		Token* t;

		DeferThing(Token* t_) : t(t_){}
		~DeferThing()
		{
			t->debug.char_count = t->str.size();
		}
	} set_debug(tok);


	char c = prev_char;
	
	skip_whitespace(&c);

	if(c == '/' && peek() == '/')
	{
		while(c != '\n')
		{
			if((c = fetch_char()) == '\0')
			{
				tok->cat = TokenCat::FileEnd;
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
				tok->cat = TokenCat::FileEnd;
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
		tok->cat = TokenCat::FileEnd;
		return;
	}


	tok->debug.line_num = m_current_line;
	//tok->debug.col_num = m_current_column;
	tok->debug.token_offset = m_current_offset - m_line_offset;
	tok->debug.line_offset = m_line_offset;

	


	if(is_symbol_valid(c) && !is_numeric(c)) //cannot begin symbol with number
	{
		tok->cat = TokenCat::Name; 
		
		while(is_symbol_valid(c))
		{
			tok->str.push_back(c);
			c = fetch_char();
		}
		


		for(uint16_t i = (uint16_t)TokenType::COMPTIME; i != (uint16_t)TokenType::DECL_EQUAL; i++)
		{
			if(strcmp(tok_enum_to_string[i], tok->str.c_str()) == 0)
			{
				tok->cat = TokenCat::Keyword;
				tok->type = (TokenType)i;
				break;
			}
		}

		if(tok->cat == TokenCat::Name)
		{
			for(uint16_t i = (uint16_t)TokenType::INT8; i != (uint16_t)TokenType::COMPTIME; i++)
			{
				if(strcmp(tok_enum_to_string[i], tok->str.c_str()) == 0)
				{
					tok->cat = TokenCat::Type;
					tok->type = (TokenType)i;
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
				tok->str.push_back(c);
				c = fetch_char();
			}

			tok->type = TokenType::UINT64;
		}
		else
		{
			tok->str.push_back(prev);
			while(is_numeric(c))
			{
				tok->str.push_back(c);
				c = fetch_char();
			}

			//todo: determine if it actually fits, if not make UINT64, or raise error!
			tok->type = TokenType::INT64;
		}

		tok->cat = TokenCat::Immediate;
		
	}
	/*else if(c == '@')
	{
		tok->cat = TokenCat::BuiltIn;
		c = fetch_char();

		while(is_symbol_valid(c) && !is_numeric(c))
		{
			tok->str.push_back(c);
			c = fetch_char();
		}

		if(tok->str.size() == 0)
			Error::strf_error(ErrorType::Error, "Line:%u Invalid character after @: '%c'",  tok->debug.line_num, c);
	}*/
	else if(c == '\"')
	{
		c = fetch_char();
		while(c != '\"')
		{
			tok->str.push_back(c);
			if((c = fetch_char()) == '\0')
			{
				log_token_error(*tok, "Readed EOF before end of terminating quote!");
			}
		}

		c = fetch_char(); //for prev_char to be set properly
		//todo: implement escape parsing "\n \t \0" etc.
	}
	else
	{
		
		tok->str.push_back(c);
		tok->str.push_back(peek());
		for(uint16_t i = (uint16_t)TokenType::DECL_EQUAL; i != (uint16_t)TokenType::AT + 1; i++)
		{
			if(strcmp((const char*)tok->str.c_str(), tok_enum_to_string[i]) == 0)
			{
				tok->type = (TokenType)i;
				tok->cat = TokenCat::Operator;

				fetch_char(); //peek
				c = fetch_char(); //for prev_char
				break;
			}
		}

		if(tok->type == TokenType::NULLTYPE)
		{
			tok->str.clear();
			tok->str.push_back(c);
			for(uint16_t i = (uint16_t)TokenType::DECL_EQUAL; i != (uint16_t)TokenType::AT + 1; i++)
			{
				if(strcmp((const char*)tok->str.c_str(), tok_enum_to_string[i]) == 0)
				{
					tok->type = (TokenType)i;
					tok->cat = TokenCat::Operator;

					c = fetch_char(); //for prev_char
					break;
				}
			}
		}
		
		if(tok->type == TokenType::NULLTYPE)
		{
			tok->str.push_back(c);
			log_token_fatal(*tok, "Unknown character!");
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

