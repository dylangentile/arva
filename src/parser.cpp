#include "parser.h"
#include "symbol.h"
#include "error.h"

#include <cstdio>
#include <map>
#include <stack>

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
	
	if(c_tok.cat == TokenCat::FileEnd)
	{
		log_token_error(c_tok, "fetched token past end of file!");
		Error::report();
		exit(1);
	}
	

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


		st->add_field(field);

		if(c_tok.type != TokenType::SEMICOLON)
		{
			log_token_error(c_tok, "expected ';'");
			continue; //act as if they just forgot the semicolon
		}

		

		fetch_token();
	}

	if(lookahead()->type == TokenType::SEMICOLON) //these gymnastics are so we can continue parsing assuming they just forgot the semicolon
		fetch_token();
	else
		log_token_error(*lookahead(), "expected ';'");
		

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
	fetch_token(); //consumes func token


	if(c_tok.type != TokenType::LPAREN)
		log_token_error(c_tok, "expected '(' following 'func' keyword!");

	func->push_debug(c_tok.debug);
	fetch_token(); //consume lparen


	while(c_tok.type != TokenType::ARROW)
	{
		AIR_Func::Arg arg;
		arg.type = parse_type(); //fetches past end of type

		if(c_tok.cat != TokenCat::Name)
			log_token_error(c_tok, "expected argument name in func");

		arg.name = c_tok.str;

		fetch_token();//consume name

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

	

	if(lookahead()->type == TokenType::SEMICOLON)
		fetch_token(); //consume rcurly
	else
		log_token_error(*lookahead(), "expected ';'");
 

	return func;
}

AIR_Node* 
Parser::parse_bin_expr()
{
	std::map<AIR_BinaryExpr::OperatorID, int32_t> precedence_map = 
	{
		{AIR_BinaryExpr::OperatorID::PLUS, 11},
		{AIR_BinaryExpr::OperatorID::MINUS, 11},
		{AIR_BinaryExpr::OperatorID::MULTIPLY, 12},
		{AIR_BinaryExpr::OperatorID::DIVIDE, 12},
		{AIR_BinaryExpr::OperatorID::MODULO, 12},
		
		{AIR_BinaryExpr::OperatorID::EQUALITY, 8},
		{AIR_BinaryExpr::OperatorID::NOT_EQUALITY, 8},
		{AIR_BinaryExpr::OperatorID::LOGICAL_AND, 4},
		{AIR_BinaryExpr::OperatorID::LOGICAL_OR, 3},
		{AIR_BinaryExpr::OperatorID::LOGICAL_NOT, 13},
		{AIR_BinaryExpr::OperatorID::LESS_THAN, 9},
		{AIR_BinaryExpr::OperatorID::GREATER_THAN, 9},
		{AIR_BinaryExpr::OperatorID::LESS_EQUAL, 9},
		{AIR_BinaryExpr::OperatorID::GREATER_EQUAL, 9},

		{AIR_BinaryExpr::OperatorID::BIT_AND, 7},
		{AIR_BinaryExpr::OperatorID::BIT_OR, 5},
		{AIR_BinaryExpr::OperatorID::BIT_COMPL, 13},

		{AIR_BinaryExpr::OperatorID::DOT, 14},

		{AIR_BinaryExpr::OperatorID::UNARY_NEGATIVE, 13},
		{AIR_BinaryExpr::OperatorID::UNARY_ADDR, 13},

		{AIR_BinaryExpr::OperatorID::LPAREN, 14},
		{AIR_BinaryExpr::OperatorID::RPAREN, 14},
	};
	

	std::stack<AIR_BinaryExpr::OperatorID> op_stack;
	std::stack<AIR_Node*> output_stack;


	int32_t paren_count = 0;
	TokenCat prev_cat = TokenCat::NULLCAT;

	while(c_tok.type != TokenType::COMMA && c_tok.type != TokenType::SEMICOLON && c_tok.type != TokenType::ASSIGN_EQUAL)
	{
		//messy/weird control flow -- might be better to put it into the switch/operators section....
		if(c_tok.type == TokenType::LPAREN)
		{
			
			op_stack.push(AIR_BinaryExpr::OperatorID::LPAREN);

			fetch_token();
			paren_count++;
			prev_cat = c_tok.cat;
			continue;
		}

		if(c_tok.type == TokenType::RPAREN)
		{
			if(paren_count == 0) //stop
				break;

			if(op_stack.empty())
					log_token_fatal(c_tok, "mismatched parentheses!");
			AIR_BinaryExpr::OperatorID op = op_stack.top();
			op_stack.pop();
			
			while(op != AIR_BinaryExpr::OperatorID::LPAREN)
			{
				//code duplication!!!! 			
				if(output_stack.size() < 2)
					log_token_fatal(c_tok, "invalid pairing of operators and symbols!" /*this is a horrible error message*/);

				AIR_Node* a = output_stack.top();
				output_stack.pop();
				AIR_Node* b = output_stack.top();
				output_stack.pop();

				AIR_BinaryExpr* bin_expr = new AIR_BinaryExpr();
				bin_expr->op = op;
				bin_expr->lhs = b;
				bin_expr->rhs = a;

				output_stack.push(bin_expr);

				if(op_stack.empty())
					log_token_fatal(c_tok, "mismatched parentheses!");

				op = op_stack.top();
				op_stack.pop();
			};
			

			fetch_token();
			paren_count--;
			prev_cat = TokenCat::Name;
		}

		switch(c_tok.cat)
		{

			case TokenCat::Operator:
			{
				AIR_BinaryExpr::OperatorID op = (AIR_BinaryExpr::OperatorID)c_tok.type;
				if(prev_cat == TokenCat::Operator)
				{
					if(c_tok.type == TokenType::MINUS)
						op = AIR_BinaryExpr::OperatorID::UNARY_NEGATIVE;
					else if(c_tok.type == TokenType::BIT_AND)
						op = AIR_BinaryExpr::OperatorID::UNARY_ADDR;
				}

				while(	!op_stack.empty() 
						&& op_stack.top() != AIR_BinaryExpr::OperatorID::LPAREN 
						&& precedence_map[op_stack.top()] > precedence_map[op]) //todo account for associativity
				{
					AIR_BinaryExpr::OperatorID op_other = op_stack.top();
					op_stack.pop();

					if(output_stack.size() < 2)
						log_token_fatal(c_tok, "invalid pairing of operators and symbols!" /*this is a horrible error message*/);

					AIR_Node* a = output_stack.top();
					output_stack.pop();
					AIR_Node* b = output_stack.top();
					output_stack.pop();

					AIR_BinaryExpr* bin_expr = new AIR_BinaryExpr();
					bin_expr->op = op_other;
					bin_expr->lhs = b;
					bin_expr->rhs = a;

					output_stack.push(bin_expr);
				}

				op_stack.push(op);
				prev_cat = c_tok.cat;
			}
			break;
			case TokenCat::Immediate:
			{
				AIR_Immediate* immediate = new AIR_Immediate();
				immediate->str = c_tok.str;
				output_stack.push(static_cast<AIR_Node*>(immediate));

				prev_cat = c_tok.cat;
			}
			break;
			case TokenCat::Name:
			{
				if(lookahead()->type == TokenType::LPAREN)//function call
				{
					AIR_FuncCall* fcall = new AIR_FuncCall();
					fcall->name = c_tok.str;

					fetch_token(); //consume (
					fetch_token();

					while(c_tok.type != TokenType::RPAREN)
					{
						fcall->arg_vec.push_back(
							static_cast<AIR_Node*>(parse_bin_expr(/*stop_on_rparen = true*/))
							//stop on rparen?
						);

						if(c_tok.type != TokenType::COMMA && c_tok.type != TokenType::RPAREN)
							log_token_error(c_tok, "expected ',' or ')'");

						if(c_tok.type == TokenType::COMMA)
							fetch_token();


					}

					output_stack.push(static_cast<AIR_Node*>(fcall));

					prev_cat = TokenCat::Name;
				}
				else //symbolref
				{
					AIR_SymbolRef* symbol = new AIR_SymbolRef();
					symbol->str = c_tok.str;
					output_stack.push(static_cast<AIR_Node*>(symbol));

					prev_cat = c_tok.cat;
				}
			}
			break;
			default:
			log_token_error(c_tok, "unexpected token in binary expression!");
		}



		fetch_token();
	}

	//more code dupe, perhaps a lambda?
	while(!op_stack.empty())
	{
		AIR_BinaryExpr::OperatorID op_other = op_stack.top();
		op_stack.pop();

		if(output_stack.size() < 2)
			log_token_fatal(c_tok, "invalid pairing of operators and symbols!" /*this is a horrible error message*/);

		AIR_Node* a = output_stack.top();
		output_stack.pop();
		AIR_Node* b = output_stack.top();
		output_stack.pop();

		AIR_BinaryExpr* bin_expr = new AIR_BinaryExpr();
		bin_expr->op = op_other;
		bin_expr->lhs = b;
		bin_expr->rhs = a;

		output_stack.push(bin_expr);
	}

	if(output_stack.empty())
		log_token_fatal(c_tok, "bad binary expression parse!");

	return output_stack.top();
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
		case TokenCat::Immediate:
		case TokenCat::Operator:
		{

			result = parse_bin_expr();
		}
		break;
		/*{
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
		break;*/

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

			if(lookahead()->cat == TokenCat::Name)
			{
				AIR_SymbolDecl* decl = new AIR_SymbolDecl();
				decl->push_debug(c_tok.debug);
				decl->type = parse_type();
				
				decl->name = c_tok.str;
				
				decl->push_debug(c_tok.debug);
				fetch_token();

				if(c_tok.type == TokenType::SEMICOLON)
				{
					decl->value = nullptr;
					log_token_warn(c_tok, "expected designated initializer for variable '%s'", decl->name.c_str());
				}
				//else if(c_tok->type == {}, initializers)
				else if(c_tok.type == TokenType::ASSIGN_EQUAL)
				{
					decl->push_debug(c_tok.debug);
					fetch_token();

					decl->value = parse_expression();
				}
				else
					log_token_error(c_tok, "invalid tokens after symbol declaration!");
				
				c_scope->add_decl(decl);

				break;

			}

			if(lookahead()->type == TokenType::ASSIGN_EQUAL)
			{
				AIR_Assign* assign = new AIR_Assign();
				AIR_SymbolRef* ref = new AIR_SymbolRef();
				assign->assignee = static_cast<AIR_Node*>(ref);
				ref->str = c_tok.str;
				
				assign->push_debug(c_tok.debug);
				fetch_token(); //consume name

				assign->push_debug(c_tok.debug);
				fetch_token(); //consume assign equal

				assign->value = parse_expression();

				c_scope->node_vec.push_back(static_cast<AIR_Node*>(assign));

				break;
			}


			if(lookahead()->type == TokenType::DOT)
			{
				AIR_Assign* assign = new AIR_Assign();
				assign->assignee = parse_bin_expr();

				if(c_tok.type != TokenType::ASSIGN_EQUAL)
					log_token_error(c_tok, "expected '='");

				fetch_token();

				assign->value = parse_expression();

				c_scope->node_vec.push_back(static_cast<AIR_Node*>(assign));

				break;
			}	



		}
		break;
		case TokenCat::Keyword:
		{
			if(c_tok.type == TokenType::RETURN)
			{
				AIR_Return* ret = new AIR_Return();
				fetch_token(); //consume return
				ret->return_expr = parse_expression();
				c_scope->node_vec.push_back(static_cast<AIR_Node*>(ret));
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