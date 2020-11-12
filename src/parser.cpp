#include "parser.h"
#include "error.h"
#include <cstdio>

Parser::Parser() 
{
	m_lexer = new Lexer();
}

Parser::~Parser() 
{
	m_lexer->destroy(); //yes I know that my design pattern isn't that consistent
	delete m_lexer;
}

void
Parser::init(const char* path)
{
	m_lexer->init_lexer(path);
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
	while(c_tok.m_cat != TokenCat::FileEnd)
	{
		switch(c_tok.m_cat)
		{
			case TokenCat::NULLCAT: token_error(ErrorType::Fatal, &c_tok, "Uncategorized token in parser!");
			break;
			case TokenCat::Keyword:
			break;
			case TokenCat::Type:
			{

			}
			break;
			case TokenCat::Immediate:
			break;
			case TokenCat::Symbol:
			{
				if(lookahead()->m_type != TokenType::DECL_EQUAL)
				{
					fetch_token();
					token_error(ErrorType::Error, &c_tok, "Expected ':='");
					break;
				}

				TokenType l_type = lookahead(2)->m_type;
				switch(l_type)
				{
					case TokenType::STRUCT:
					{

					}
					break;
					case TokenType::ENUM:
					break;
					case TokenType::CLASS:
					break;
					case TokenType::NAMESPACE:
					break;
					default: ; //expect an expression.
				}



			}
			break;
			case TokenCat::Operator:
			break;
			case TokenCat::FileEnd: //cant happen but include for compiler warnings' sake
			break;
			case TokenCat::BuiltIn:
			break;

		}
		fetch_token();
	}
}