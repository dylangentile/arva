#pragma once
#include "debug.h"

#include <cstdint>
#include <string>


enum class TokenType : uint16_t;
enum class TokenCat  : uint16_t;


struct Token
{
	TokenType type;
	TokenCat  cat;
	std::string str;

	DebugInfo debug;
};



enum class TokenCat : uint16_t
{
	NULLCAT = 0,
	Keyword,
	Type,
	Immediate,
	Name,
	Operator,
	FileEnd,
};


//update type.h and lexer.cpp whenever you update this
enum class TokenType : uint16_t
{
	NULLTYPE = 0,

	//types
	INT8,
	INT16,
	INT32,
	INT64,
	UINT8,
	UINT16,
	UINT32,
	UINT64,

	FLOAT,
	DOUBLE,
	CHAR,
	BOOL,
	VOID,

	//keywords

	COMPTIME,
	IF,
	SWITCH,
	WHILE,
	FOR,
	RETURN,
	FUNC,
	STRUCT,
	CLASS,
	ENUM,
	NAMESPACE,
	
	//operators
	DECL_EQUAL,
	ASSIGN_EQUAL,

	PLUS,
	MINUS,
	MULTIPLY,
	DIVIDE,
	MODULO,

	PLUS_EQUAL,
	MINUS_EQUAL,
	MULTIPLY_EQUAL,
	DIVIDE_EQUAL,
	MODULO_EQUAL,

	//logic
	EQUALITY,
	NOT_EQUALITY,
	LOGICAL_AND,
	LOGICAL_OR,
	LOGIC_NOT,

	LESS_THAN,
	GREATER_THAN,
	LESS_EQUAL,
	GRATER_EQUAL,

	//bitwise
	BIT_AND,
	BIT_OR,
	BIT_COMPL,

	//delimiters
	LCURLY,
	RCURLY,
	LPAREN,
	RPAREN,
	LBRACKET,
	RBRACKET,

	//misc
	SEMICOLON,
	COLON,
	COMMA,
	DOT,
	ARROW,
	AT,
};



#ifndef ARVA_INCLUDE_ENUM_STR
extern const char* tok_enum_to_string[];
extern const size_t tok_enum_to_string_size;
#else

const char* tok_enum_to_string[] = 
{
	nullptr,
	"i8",
	"i16",
	"i32",
	"i64",
	"u8",
	"u16",
	"u32",
	"u64",

	"float",
	"double",
	"char",
	"bool",
	"void",

	"comptime",
	"if",
	"switch",
	"while",
	"for",
	"return",
	"func",
	"struct",
	"class",
	"enum",
	"namespace",


	":=",
	"=",

	"+",
	"-",
	"*",
	"/",
	"%",

	"+=",
	"-=",
	"*=",
	"/=",
	"%=",

	"==",
	"!=",
	"&&",
	"||",
	"!",

	"<",
	">",
	"<=",
	">=",

	"&",
	"|",
	"~",

	"{",
	"}",
	"(",
	")",
	"[",
	"]",

	";",
	":",
	",",
	".",
	"->",
	"@",

};

const size_t tok_enum_to_string_size = sizeof(tok_enum_to_string);

#endif


