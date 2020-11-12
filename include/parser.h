#pragma once
#include "token.h"
#include "lexer.h"
#include <deque>
class Parser
{
	Parser(const Parser&) = delete;
	Parser& operator=(const Parser&) = delete;
public:
	Parser();
	~Parser();

	void init(const char*);
	void parse();

private:
	Token* fetch_token();
	Token* lookahead(uint8_t i = 1);
	
private:
	Lexer* m_lexer;

	Token c_tok; //current token, used so much you want the shorthand
	std::deque<Token> lookahead_queue;
};