#pragma once
#include <cstdint>
#include <string>

enum class TokenType : uint16_t;
enum class TokenCat  : uint16_t;

struct Token
{
	TokenType m_type;
	TokenCat  m_cat;
	std::string m_str;

	uint32_t m_line_num;
	uint32_t m_col_num;
	uint32_t m_foffset;
};



enum class TokenCat : uint16_t
{
	NULLCAT = 0,
	Keyword,
	Immediate,
	Symbol,
	Operator,
	FileEnd, //stdio.h defines EOF sometimes
	BuiltIn, //any @blah function
};

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
	STRING,
	CSTRING,
	BOOL,
	VOID,

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
	COMMA,
	DOT,
	ARROW,

	//keywords

	COMPTIME,
	CONST




};



#ifndef ARVA_INCLUDE_ENUM_STR_CRAP
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
	"string",
	"cstring",
	"bool",
	"void",


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
	",",
	".",
	"->",

};

const size_t tok_enum_to_string_size = sizeof(tok_enum_to_string);

#endif


