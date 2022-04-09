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
	if(m_lookahead_queue.empty())
	{
		m_lexer->fetch_token(&c_tok);
	}
	else
	{
		c_tok = m_lookahead_queue.front();
		m_lookahead_queue.pop_front();
	}

	
	return &c_tok;
}

Token*
Parser::lookahead(uint8_t i)
{
	while(m_lookahead_queue.size() != i)
	{
		Token tok;
		m_lexer->fetch_token(&tok);
		m_lookahead_queue.push_back(tok);
	}


	return &m_lookahead_queue[i - 1];
}

void
Parser::push_scope(AIR_Scope* s)
{
	s->parent = c_scope;
	m_scope_stack.push(s);
	c_scope = s;
}

void
Parser::pop_scope()
{
	m_scope_stack.pop();
	c_scope = m_scope_stack.top();
}

Type
Parser::parse_type()
{
	Type ret;
	switch(c_tok.cat)
	{
		case TokenCat::Type:
		{
			ret.access = TypeAccess::Mutable; //there were no qualifiers!
			ret.storage = TypeStorage::Value; //default for now

			ret.cat = TypeCategory::Raw;
			ret.raw = token_type_to_raw_type(c_tok.type);

			fetch_token();
		}
		break;
		case TokenCat::Name:
		{
			ret.access = TypeAccess::Mutable; //there were no qualifiers!
			ret.storage = TypeStorage::Value; //default for now

			ret.cat = TypeCategory::Symbol;
			ret.symbol = c_tok.str;

			fetch_token();
		}
		break;
		case TokenCat::Keyword:
		{
			//todo
			log_token_error(c_tok, "type keywords unimplemented!");
		}
		//break;
		default:
		log_token_error(c_tok, "expected a type!");
		break;
	}

	return ret;
}


AIR_Struct*
Parser::parse_struct()
{
	AIR_Struct* st = new AIR_Struct;
	st->push_debug(c_tok.debug);
	fetch_token();

	if(c_tok.type != TokenType::LCURLY)
		log_token_error(c_tok, "expected '{' following struct keyword");

	st->push_debug(c_tok.debug);
	fetch_token();

	while(c_tok.type != TokenType::RCURLY)
	{
		AIR_Struct::Field field;
		field.type = parse_type(); //fetches past end of type

		if(c_tok.cat != TokenCat::Name)
			log_token_error(c_tok, "expected field name in struct!");

		field.name = c_tok.str;
		fetch_token();

		if(c_tok.type != TokenType::SEMICOLON)
			log_token_error(c_tok, "expected ';'");

		st->field_vec.push_back(field);

		fetch_token();
	}

	fetch_token();

	if(c_tok.type != TokenType::SEMICOLON)
		log_token_error(c_tok, "expected ';'");

	st->push_debug(c_tok.debug);

	return st;

}


AIR_Node*
Parser::parse_expression()
{
	AIR_Node* result = nullptr;
	switch(c_tok.cat)
	{
		case TokenCat::Keyword:
		{
			switch(c_tok.type)
			{
				case TokenType::FUNC:
				{

				}
				break;
				case TokenType::STRUCT:
				{
					result = static_cast<AIR_Node*>(parse_struct());
				}
				break;
				case TokenType::CLASS:
				{

				}
				break;
				case TokenType::ENUM:
				{

				}
				break;
				case TokenType::NAMESPACE:
				{

				}
				break;

				default:
				{
					log_token_error(c_tok, "unexpected keyword in expression!");
				}
				break;
			}
		}
		break;


		case TokenCat::Name:
		{
			if(lookahead()->type == TokenType::SEMICOLON)
			{
				
				AIR_SymbolRef* ref = new AIR_SymbolRef;
				ref->str = c_tok.str;

				ref->push_debug(c_tok.debug);
				fetch_token(); //consume current token, now on semicolon
				ref->push_debug(c_tok.debug);

				result = static_cast<AIR_Node*>(ref);
				break;
			}
		}
		break;

		case TokenCat::Immediate:
		{
			if(lookahead()->type == TokenType::SEMICOLON)
			{
				AIR_Immediate* immediate = new AIR_Immediate;
				immediate->str = c_tok.str;

				immediate->push_debug(c_tok.debug);
				fetch_token(); //consume current token, now on semicolon
				immediate->push_debug(c_tok.debug);

				result = static_cast<AIR_Node*>(immediate);
				break;
			}
		}
		break;

		default:
		{
			log_token_error(c_tok, "invalid token to begin expression!");
		}
		break;
	}


	if(result == nullptr)
		log_token_error(c_tok, "failed to resolve expression!");

	return result;
}


AIR_Scope* 
Parser::parse()
{
	bool working = true;

	AIR_Scope* global_scope = new AIR_Scope;
	push_scope(global_scope);

	while(working)
	{
		fetch_token();
		switch(c_tok.cat)
		{
			case TokenCat::Name:
			{
				AIR_SymbolDecl* decl = new AIR_SymbolDecl;
				decl->name = c_tok.str;
				
				decl->push_debug(c_tok.debug);
				fetch_token();

				if(c_tok.type != TokenType::DECL_EQUAL)
				{
					log_token_error(c_tok, "expected ':='");
					break;
				}

				decl->push_debug(c_tok.debug);
				fetch_token();

				decl->value = parse_expression();

				c_scope->add_decl(decl);



			}
			break;

			case TokenCat::FileEnd:
			{
				working = false;
			}
			break;

			default:
			{
				//log_token_error(c_tok, "bad token in global namespace!");
			}
			break;
		};
	}

	if(c_scope != global_scope)
		Error::str_error(ErrorType::Fatal, "bad scope stack management in parser!");

	return global_scope;
}