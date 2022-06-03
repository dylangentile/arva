#pragma once
#include "token.h"
#include "lexer.h"
#include "type.h"
#include "air.h"

#include <deque>
#include <stack>

class Parser
{
	Parser(const Parser&) = delete;
	Parser& operator=(const Parser&) = delete;
public:
	Parser();
	~Parser();

	void initialize(const char*);
	void terminate();
	
	AIR_Scope* parse();

private:
	Token* fetch_token();
	Token* lookahead(uint8_t i = 1);

	void push_scope(AIR_Scope*);
	void pop_scope();

	Type parse_type();

	AIR_Struct* parse_struct();
	AIR_SymbolDecl* parse_decl();
	AIR_Func* parse_function();
	AIR_Node* parse_expression();
	AIR_Node* parse_bin_expr();

	void parse_statement();

	
	
private:
	Lexer* m_lexer;

	Token c_tok; //current token, used so much you want the shorthand
	AIR_Scope* c_scope; //current scope

	std::deque<Token> m_lookahead_queue;
	std::stack<AIR_Scope*> m_scope_stack;
};