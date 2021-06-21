#pragma once
#include "type.h"

enum class NodeType : uint16_t
{
	Null,
	Expression,
	BinaryOp,
	UnaryOp,
	FunctionCall,
	Immediate

};

struct AST_Node
{
	NodeType type;
};


struct AST_BinaryOp
{
	AST_Node node;

	AST_Node* left;
	AST_Node* right;
};

struct AST_UnaryOp
{
	AST_Node node;

	AST_Node* operand; 
};

struct AST_FunctionCall
{
	AST_Node node;
};

struct AST_Immediate
{
	AST_Node node;

	Type* type;
};

struct AST_VariableDecl
{
	AST_Node node;

	Type* type;
	std::string name;
	AST_Node* value;


};

struct AST_VariableRef
{
	AST_Node node;


};
