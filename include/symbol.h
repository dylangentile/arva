#pragma once
#include "token.h"
#include <unordered_map>
#include <vector>

enum class SymbolType
{
	NULLTYPE = 0,
	Function,
	Structure,
	Class,
	Enum,
	Namespace,
	Variable,
};


struct SymbolReference
{
	Token* m_tok = nullptr;
	//in this case Structure = Class = Enum = Namespace 
	// since there is little differentiation till reference resolution
	SymbolType expected_type = SymbolType::NULLTYPE;
	void** the_ref = nullptr;

};

struct SymbolDeclaration
{
	Token* m_tok = nullptr;
	SymbolType m_type = SymbolType::NULLTYPE;
	void* ptr = nullptr;
};

class SymbolTable
{
	SymbolTable();
	~SymbolTable();
public:
	static void init();
	static void destroy();

	static void add_reference(SymbolReference);
	static void add_declaration(SymbolDeclaration);

	static void resolve_references();


private:
	static SymbolTable* s;

	std::unordered_map<std::string, std::vector<SymbolReference> > m_reference_map;
	//for now  arva doesn't support symbol overloading of any kind
	std::unordered_map<std::string, SymbolDeclaration> m_decl_map; 
};