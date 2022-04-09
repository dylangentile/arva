#pragma once
#include <vector>
#include <unordered_map>
#include <string>

struct AIR_SymbolDecl;
struct AIR_Node;

class SymbolTable
{
public:
	SymbolTable();
	~SymbolTable();


	void add_decl(AIR_SymbolDecl* decl);

	std::unordered_map<std::string, AIR_Node*> m_name_map;

};