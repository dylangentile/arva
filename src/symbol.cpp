#include "symbol.h"
#include "error.h"
#include "lexer.h"
#include <cassert>

SymbolTable* SymbolTable::s = nullptr;

SymbolTable::SymbolTable() {}
SymbolTable::~SymbolTable() {}


void
SymbolTable::init()
{
	s = new SymbolTable();
}

void
SymbolTable::destroy()
{
	delete s;
}


void
SymbolTable::add_reference(SymbolReference ref)
{
	assert(s != nullptr && "SymbolTable uninitialized!");

	auto finder = s->reference_map.find(ref.str);
	if(finder == s->reference_map.end())
		s->reference_map.insert(std::make_pair(ref.str, std::vector<SymbolReference>({ref})));
	else
		finder->second.push_back(ref);
}

void
SymbolTable::add_struct_declaration(StructDecl decl)
{
	assert(s != nullptr && "SymbolTable uninitialized!");
	
	auto finder = s->decl_map.find(decl.s_name);
	if(finder != s->decl_map.end())
		dinfo_error(ErrorType::Error, decl.debug, "Symbol is already in use!");
	else
	{
		s->struct_vec.push_back(decl);
		StructDecl* ptr = s->struct_vec.data() + (s->struct_vec.size() - 1);

		SymbolDeclaration sym_decl;
		sym_decl.type = SymbolType::Structure;
		sym_decl.ptr = ptr;

		s->decl_map.insert(std::make_pair(decl.s_name, sym_decl));
	}
	

}

void
SymbolTable::resolve_references()
{
	
}
