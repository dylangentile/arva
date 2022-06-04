#pragma once
//AIR, the first IR. (There are 3) (AIR, BIR, CIR)
// this IR has no types and is very barebones 
// to allow the parser to quickly create something
// that will then be iterated upon by the ir passes

#include "type.h"
#include "debug.h"
#include "symbol.h"

#include <vector>
#include <string>


enum class AIR_Node_ID
{
	BinaryExpr,
	SymbolDecl,
	SymbolRef,
	Assign,
	Immediate,
	FuncCall,
	Func,
	Scope,
	Struct,
	Return,

};


struct AIR_Node
{
	const AIR_Node_ID id;
	DebugGroup debug;


	void push_debug(const DebugInfo&);

	AIR_Node(const AIR_Node_ID id_);
	~AIR_Node();
};


struct AIR_BinaryExpr : public AIR_Node
{
	enum class OperatorID : uint16_t
	{
		PLUS = (uint16_t)TokenType::PLUS,
		MINUS,
		MULTIPLY,
		DIVIDE,
		MODULO,

		//logic
		EQUALITY = (uint16_t)TokenType::EQUALITY - (uint16_t)TokenType::PLUS,
		NOT_EQUALITY,
		LOGICAL_AND,
		LOGICAL_OR,
		LOGICAL_NOT,

		LESS_THAN,
		GREATER_THAN,
		LESS_EQUAL,
		GREATER_EQUAL,

		//bitwise
		BIT_AND,
		BIT_OR,
		BIT_COMPL,

		DOT = (uint16_t)TokenType::DOT,

		
		//these don't match w/tokentypes
		UNARY_NEGATIVE,
		UNARY_ADDR,

		//these are just for the op stack, and for precedence
		LPAREN,
		RPAREN,

	};

	OperatorID op;
	AIR_Node* lhs;
	AIR_Node* rhs;


	AIR_BinaryExpr();
	~AIR_BinaryExpr();
};


struct AIR_SymbolDecl : public AIR_Node
{
	std::string name;
	AIR_Node* value; //this can be null. all null vars are zeroed by default
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

struct AIR_Assign : public AIR_Node
{
	AIR_Node* assignee;
	AIR_Node* value;


	AIR_Assign();
	~AIR_Assign();
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

struct AIR_Scope;
struct AIR_Func : public AIR_Node
{

	struct Arg
	{
		Type type;
		std::string name;
	};

	std::vector<Arg> arg_vec;
	Type return_type;
	
	AIR_Scope* scope;


	AIR_Func();
	~AIR_Func();
};


struct AIR_Scope : public AIR_Node
{
	std::vector<AIR_Node*> node_vec;
	SymbolTable symbol_table;
	AIR_Scope* parent;

	void add_decl(AIR_SymbolDecl* decl);

	
	
	AIR_Scope();
	~AIR_Scope();	
};


struct AIR_Struct : public AIR_Node
{
	struct Field
	{
		Type type;
		std::string name;
	};

	void add_field(const Field&);


	std::vector<Field> field_vec;
	std::unordered_map<std::string, size_t> field_map; //indexes into field vec 


	AIR_Struct();
	~AIR_Struct();

};

struct AIR_Return : public AIR_Node
{
	AIR_Node* return_expr;

	AIR_Return();
	~AIR_Return();
};





std::string print_air_node(const AIR_Node* n, bool print_ids, std::string prepend = "");



