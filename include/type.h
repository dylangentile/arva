#pragma once
#include "token.h"

enum class TypeAccess : uint16_t
{
	Immutable,
	Mutable,
};

//needs to be in same order as token.h types
enum class RawTypeEnum : uint16_t
{
	Null,
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
};

enum class TypeCategory : uint16_t
{
	Raw,
	Symbol,
	Unresolved,
};

enum class TypeStorage : uint16_t
{
	Reference,
	Unsafe,
	Value,
};

struct Type
{
	TypeAccess access;
	TypeStorage storage;

	TypeCategory cat;

	RawTypeEnum raw;
	std::string symbol;
	


	std::string print() const;

	Type();
	~Type();

	Type(const Type& other);
	Type& operator=(const Type& other);


};


RawTypeEnum
token_type_to_raw_type(TokenType type);


