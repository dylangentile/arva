#include "type.h"

RawTypeEnum
token_type_to_raw_type(TokenType type)
{
	if((uint16_t)type > (uint16_t)TokenType::VOID)
		return RawTypeEnum::Null;

	return (RawTypeEnum)((uint16_t)type);
}



std::string 
Type::print() const
{
	std::string str;

	if(cat == TypeCategory::Unresolved)
		return "Unresolved";

	if(access == TypeAccess::Immutable)
		str += "immut ";
	else
		str += "mut ";

	if(cat == TypeCategory::Symbol)
		str += symbol + " ";
	else
		str += tok_enum_to_string[(uint16_t)raw] + std::string(" ");

	switch(storage)
	{
		case TypeStorage::Reference:
			str += "ref";
		break;
		case TypeStorage::Unsafe:
			str += "unsafe";
		break;
		case TypeStorage::Value:
			str += "value";
		break;
	}

	return str;
};


Type::Type() : cat(TypeCategory::Unresolved) {}
Type::~Type(){}


Type::Type(const Type& other)
{
	this->access = other.access;
	this->storage = other.storage;
	this->cat = other.cat;
	if(other.cat == TypeCategory::Symbol)
		this->symbol = other.symbol;
	else
		this->raw = other.raw;
}


Type& 
Type::operator=(const Type& other)
{
	this->access = other.access;
	this->storage = other.storage;
	this->cat = other.cat;
	if(other.cat == TypeCategory::Symbol)
		this->symbol = other.symbol;
	else
		this->raw = other.raw;

	return *this;
}



