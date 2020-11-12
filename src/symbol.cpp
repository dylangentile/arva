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

	auto finder = s->m_reference_map.find(ref.m_tok->m_str);
	if(finder == s->m_reference_map.end())
		s->m_reference_map.insert(std::make_pair(ref.m_tok->m_str, std::vector<SymbolReference>({ref})));
	else
		finder->second.push_back(ref);
}

void
SymbolTable::add_declaration(SymbolDeclaration decl)
{
	assert(s != nullptr && "SymbolTable uninitialized!");
	
	auto finder = s->m_decl_map.find(decl.m_tok->m_str);
	if(finder == s->m_decl_map.end())
		token_error(ErrorType::Error, decl.m_tok, "Redeclaration of \033[0m\033[1;96m%s:%u\033[0m:", 
			finder->second.m_tok->m_file_path, finder->second.m_tok->m_line_num);
	else
		s->m_decl_map.insert(std::make_pair(decl.m_tok->m_str, decl));
	

}
