#pragma once
#include "token.h"

enum class TypeModifier : uint16_t
{
	Null,
	CONST,
};

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
	STRING,
	CSTRING,
	BOOL,
	VOID,
};

enum class TypeCategory : uint16_t
{
	Null,
	Class,
	Namespace,
	Struct,
	Pointer,
	Raw,
	Unknown,
};

enum class PointerCategory : uint16_t
{
	Null,
	Regular,
	Unique,
	Shared,
};

struct Type
{
	TypeCategory cat;
	TypeModifier mod;

};

struct Type_Unknown
{
	Type type;
	Type* ref = nullptr;
};

struct Type_Raw
{
	Type type;
	RawTypeEnum raw;
};

struct Type_Ptr
{
	Type type;
	PointerCategory pcat;
	Type* pointee;
};


RawTypeEnum
token_type_to_raw_type(TokenType type);


