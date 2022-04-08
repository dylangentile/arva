#include "type.h"

RawTypeEnum
token_type_to_raw_type(TokenType type)
{
	if((uint16_t)type > (uint16_t)TokenType::VOID)
		return RawTypeEnum::Null;

	return (RawTypeEnum)((uint16_t)type);
}

Type::Type() : cat(TypeCategory::Unresolved) {}
Type::~Type(){}