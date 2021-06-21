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

Type*
Parser::parse_type()
{
	Type* type = nullptr;

	TypeModifier mod = TypeModifier::Null;
	if(c_tok.m_type == TokenType::CONST)
	{
		mod = TypeModifier::CONST;
		fetch_token();
	}
	
	
	RawTypeEnum rt = token_type_to_raw_type(c_tok.m_type);
	if(rt != RawTypeEnum::Null)
	{
		Type_Raw* raw_type = new Type_Raw; //todo: Types should be allocated from pools/arenas
		raw_type->type.cat = TypeCategory::Raw;
		raw_type->type.mod = mod;
		raw_type->raw = rt;

		type = reinterpret_cast<Type*>(raw_type);
	}
	else if(c_tok.m_cat == TokenCat::Name)
	{
		Type_Unknown* u_type = new Type_Unknown; //todo
		u_type->type.cat = TypeCategory::Unknown;
		u_type->type.mod = mod;

		SymbolReference ref;
		ref.debug = c_tok.m_debug;
		ref.str = c_tok.m_str;
		ref.expected_type = SymbolType::Structure;
		ref.the_ref = u_type;

		SymbolTable::add_reference(ref);

		type = reinterpret_cast<Type*>(u_type);
	}
	


	return type;
}

//the outside must verify the sequence "name := struct", so this function assumes that
// call with the c_tok on "name"
void
Parser::parse_struct()
{
	Token name = c_tok; //name
	fetch_token(); // ":="
	fetch_token(); // "struct"
	fetch_token();

	if(c_tok.m_type != TokenType::LCURLY)
	{
		token_error(ErrorType::Error, &c_tok, "Expected '{'");
		return;
	}

	fetch_token();

	StructDecl decl;
	decl.s_name = c_tok.m_str;
	decl.debug = c_tok.m_debug;


	while(c_tok.m_type != TokenType::RCURLY)
	{
		Type* term_type = nullptr;
		if((term_type = parse_type()) == nullptr)
		{
			token_error(ErrorType::Error, &c_tok, "Invalid Type!");
			return;
		}


		fetch_token(); //parse_type() ends on last token of type

		if(c_tok.m_cat != TokenCat::Name)
		{
			token_error(ErrorType::Error, &c_tok, "Expected name!");
			return;
		}

		

		VarDecl v_decl;
		v_decl.type = term_type;
		v_decl.name = c_tok.m_str;

		decl.field_vec.push_back(v_decl);

		fetch_token();

		if(c_tok.m_type != TokenType::SEMICOLON)
		{
			token_error(ErrorType::Error, &c_tok, "Expected ';'");
			return;
		}


		fetch_token();
	}

	SymbolTable::add_struct_declaration(decl);

}


void
Parser::parse_function()
{
	FuncDecl decl;
	decl.debug = c_tok.m_debug;
	decl.f_name = c_tok.m_str;

	fetch_token(); // :=
	fetch_token(); // (
	fetch_token();

	while(1)
	{
		if(c_tok.m_type == TokenType::VOID)
		{
			fetch_token();
			if(c_tok.m_type != TokenType::ARROW)
			{
				token_error(ErrorType::Error, &c_tok, "Expected '->' after void!");
				return;
			}

			break;
		}

		Type* term_type = parse_type();
		if(term_type == nullptr)
		{
			token_error(ErrorType::Error, &c_tok, "Expected arguement type!");
			return;
		}

		fetch_token();

		if(c_tok.m_cat != TokenCat::Name)
		{
			token_error(ErrorType::Error, &c_tok, "Expected name!");
			return;
		}

		VarDecl v_decl;
		v_decl.type = term_type;
		v_decl.name = c_tok.m_str;
		decl.arg_vec.push_back(v_decl);

		fetch_token();

		if(c_tok.m_type == TokenType::ARROW)
			break;

		if(c_tok.m_type != TokenType::COMMA)
		{
			token_error(ErrorType::Error, &c_tok, "Expected comma!");
			continue; //can continue safely (assume they forgot a comma)
		}

		fetch_token();

	}

	fetch_token(); // ->

	Type* ret_type = parse_type();
	if(ret_type == nullptr)
	{
		token_error(ErrorType::Error, &c_tok, "Expected return type!");
		return;
	}

	decl.ret_type = ret_type;

	fetch_token();
	if(c_tok.m_type != TokenType::RPAREN)
	{
		token_error(ErrorType::Error, &c_tok, "Expected ')'");
		return;
	}

	fetch_token();

	if(c_tok.m_type != TokenType::LCURLY)
	{
		token_error(ErrorType::Error, &c_tok, "Expected '{'");
		return;
	}







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
			case TokenCat::Name: //these are not "symbols" but arbitrary 
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
						parse_struct();
					}
					break;
					case TokenType::LPAREN:
					{
						parse_function();
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