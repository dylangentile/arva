#include "symbol.h"
#include "air.h"
#include "error.h"

SymbolTable::SymbolTable() {}
SymbolTable::~SymbolTable(){}




void
SymbolTable::add_decl(AIR_SymbolDecl* decl)
{
	if(m_name_map.find(decl->name) != m_name_map.end())
	{
		log_air_error(decl, "redeclaration of symbol already declared in current scope!");
		return;
	}

	m_name_map.insert(std::make_pair(decl->name, decl->value));
}