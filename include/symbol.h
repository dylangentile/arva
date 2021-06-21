#pragma once
#include "token.h"
#include "type.h"
#include "ast.h"
#include <string>
#include <unordered_map>
#include <vector>

struct VarDecl
{
	Type* type;
	std::string name;
};

struct StructDecl
{
	DebugInfo debug;
	std::string s_name;
	//fields
	std::vector<VarDecl> field_vec;

};


struct FuncDecl
{
	DebugInfo debug;
	std::string f_name;

	std::vector<VarDecl> arg_vec;
	Type* ret_type;

	std::vector<AST_Node*> node_vec;


};

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
	DebugInfo debug;
	std::string str;
	//in this case Structure = Class = Enum = Namespace 
	// since there is little differentiation till reference resolution
	SymbolType expected_type = SymbolType::NULLTYPE;
	void* the_ref = nullptr;

};

struct SymbolDeclaration
{
	SymbolType type = SymbolType::NULLTYPE;
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
	static void add_struct_declaration(StructDecl);

	static void resolve_references();


private:
	static SymbolTable* s;

	std::unordered_map<std::string, std::vector<SymbolReference> > reference_map;
	//for now  arva doesn't support symbol overloading of any kind
	std::unordered_map<std::string, SymbolDeclaration> decl_map; 

	std::vector<StructDecl> struct_vec;
};