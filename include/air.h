#pragma once
//AIR, the first IR. (There are 3) (AIR, BIR, CIR)
// this IR has no types and is very barebones 
// to allow the parser to quickly create something
// that will then be iterated upon by the ir passes

#include "type.h"
#include "debug.h"


#include <vector>
#include <string>


enum class AIR_Node_ID
{
	Scope,
	BinaryExpr,
	SymbolDecl,
	SymbolRef,
	Immediate,
	FuncCall,
	Func,

};


struct AIR_Node
{
	const AIR_Node_ID id;
	DebugGroup debug;


	AIR_Node(const AIR_Node_ID id_);
	~AIR_Node();
};

struct AIR_Scope : public AIR_Node
{
	std::vector<AIR_Node*> node_vec;
	

	
	AIR_Scope();
	~AIR_Scope();	
};

struct AIR_BinaryExpr : public AIR_Node
{



	AIR_BinaryExpr();
	~AIR_BinaryExpr();
};

struct AIR_SymbolDecl : public AIR_Node
{
	std::string name;
	AIR_Node* value;
	Type type;

	AIR_SymbolDecl();
	~AIR_SymbolDecl();
};

struct AIR_SymbolRef : public AIR_Node
{
	std::string str;

	AIR_SymbolRef();
	~AIR_SymbolRef();
};


struct AIR_Immediate : public AIR_Node
{
	std::string str;

	AIR_Immediate();
	~AIR_Immediate();
};

struct AIR_FuncCall : public AIR_Node
{
	std::string name;
	std::vector<AIR_Node*> arg_vec;

	AIR_FuncCall();
	~AIR_FuncCall();
};

struct AIR_Func : public AIR_Node
{

	struct Arg
	{
		std::string name;
		Type type;
	};

	std::vector<Arg*> arg_vec;
	std::vector<AIR_Node*> node_vec;
	Type return_type;


	AIR_Func();
	~AIR_Func();
};




std::string print_air_node(const AIR_Node* n, bool print_ids);



