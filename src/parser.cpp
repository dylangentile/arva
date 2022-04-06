#include "parser.h"
#include "symbol.h"
#include "error.h"
#include <cstdio>

Parser::Parser() 
{
	m_lexer = new Lexer();
}

Parser::~Parser() 
{
	delete m_lexer;
}

void
Parser::initialize(const char* path)
{
	m_lexer->initialize(path);
}

void
Parser::terminate()
{
	m_lexer->terminate();
}


Token*
Parser::fetch_token()
{	
	if(lookahead_queue.empty())
	{
		m_lexer->fetch_token(&c_tok);
	}
	else
	{
		c_tok = lookahead_queue.front();
		lookahead_queue.pop_front();
	}

	
	return &c_tok;
}

Token*
Parser::lookahead(uint8_t i)
{
	while(lookahead_queue.size() != i)
	{
		Token tok;
		m_lexer->fetch_token(&tok);
		lookahead_queue.push_back(tok);
	}


	return &lookahead_queue[i - 1];
}

void 
Parser::parse()
{
	fetch_token();
	
}