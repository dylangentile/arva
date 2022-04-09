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
		log_token_error(c_tok, "expected '{' following 'struct' keyword");

	st->push_debug(c_tok.debug);
	fetch_token();

	while(c_tok.type != TokenType::RCURLY)
	{
		AIR_Struct::Field field;
		field.type = parse_type(); //fetches past end of type

		if(c_tok.cat != TokenCat::Name)
			log_token_error(c_tok, "expected field name in struct");

		field.name = c_tok.str;
		fetch_token();

		if(c_tok.type != TokenType::SEMICOLON)
			log_token_error(c_tok, "expected ';'");

		st->add_field(field);

		fetch_token();
	}

	fetch_token();

	if(c_tok.type != TokenType::SEMICOLON)
		log_token_error(c_tok, "expected ';'");

	st->push_debug(c_tok.debug);

	return st;

}

AIR_SymbolDecl* 
Parser::parse_decl()
{
	AIR_SymbolDecl* decl = new AIR_SymbolDecl;
	decl->name = c_tok.str;
	
	decl->push_debug(c_tok.debug);
	fetch_token();

	if(c_tok.type != TokenType::DECL_EQUAL)
		log_token_error(c_tok, "expected ':='");
	
	decl->push_debug(c_tok.debug);
	fetch_token();

	decl->value = parse_expression();

	return decl;
}

AIR_Func*
Parser::parse_function()
{
	AIR_Func* func = new AIR_Func;
	func->push_debug(c_tok.debug);
	fetch_token();


	if(c_tok.type != TokenType::LPAREN)
		log_token_error(c_tok, "expected '(' following 'func' keyword!");

	func->push_debug(c_tok.debug);
	fetch_token();


	while(c_tok.type != TokenType::ARROW)
	{
		AIR_Func::Arg arg;
		arg.type = parse_type(); //fetches past end of type

		if(c_tok.cat != TokenCat::Name)
			log_token_error(c_tok, "expected argument name in func");

		fetch_token();

		if(c_tok.type == TokenType::COMMA)
			fetch_token();

		func->arg_vec.push_back(arg);
	}
	fetch_token(); //consume arrow

	func->return_type = parse_type();//fetches past end of type

	if(c_tok.type != TokenType::RPAREN)
		log_token_error(c_tok, "expected ')'");

	fetch_token(); //consume rparen

	if(c_tok.type != TokenType::LCURLY)
		log_token_error(c_tok, "expected '{' to begin function body");
	
	fetch_token(); //consume lcurly

	func->scope = new AIR_Scope;
	push_scope(func->scope);
		while(c_tok.type != TokenType::RCURLY)
		{
			parse_statement();
			fetch_token();
		}
	pop_scope();

	fetch_token(); //consume rcurly

	if(c_tok.type != TokenType::SEMICOLON)
		log_token_error(c_tok, "expected ';'");


	return func;
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
					result = static_cast<AIR_Node*>(parse_function());
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
		log_token_error(c_tok, "failed to parse expression!");

	return result;
}


void
Parser::parse_statement()
{
	switch(c_tok.cat)
	{
		case TokenCat::Name:
		{

			//could be a decl
			if(lookahead()->type == TokenType::DECL_EQUAL)
			{
				c_scope->add_decl(parse_decl());
				break;
			}

			//could be a type + name
			//if(lookahead()-> == name) && lookahead(2) == assignment


			if(lookahead()->type == TokenType::ASSIGN_EQUAL)
			{
				AIR_Assign* assign = new AIR_Assign;
				assign->assignee.str = c_tok.str;
				
				assign->push_debug(c_tok.debug);
				fetch_token(); //consume name

				assign->push_debug(c_tok.debug);
				fetch_token(); //consume assign equal

				assign->value = parse_expression();

				c_scope->node_vec.push_back(static_cast<AIR_Node*>(assign));
			}	



		}
		break;
		case TokenCat::Keyword:
		{
			//if(c_tok.type == TokenType::RETURN)
			{

			}
		}
		break;
		case TokenCat::Type:
		{

		}
		break;


		default:
		log_token_error(c_tok, "bad token in current scope");
	}
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
				c_scope->add_decl(parse_decl());
			}
			break;

			case TokenCat::FileEnd:
			{
				working = false;
			}
			break;

			default:
			log_token_error(c_tok, "bad token in global namespace!");
		};
	}

	if(c_scope != global_scope)
		Error::str_error(ErrorType::Fatal, "bad scope stack management in parser!");

	return global_scope;
}