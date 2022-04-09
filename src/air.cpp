#include "air.h"



AIR_Node::AIR_Node(const AIR_Node_ID id_) : id(id_) {}
AIR_Node::~AIR_Node() {}


void
AIR_Node::push_debug(const DebugInfo& d)
{
	debug.info_vec.push_back(d);
}


AIR_BinaryExpr::AIR_BinaryExpr() : AIR_Node(AIR_Node_ID::BinaryExpr) {}
AIR_BinaryExpr::~AIR_BinaryExpr() {}


AIR_SymbolDecl::AIR_SymbolDecl() : AIR_Node(AIR_Node_ID::SymbolDecl) {}
AIR_SymbolDecl::~AIR_SymbolDecl() {}


AIR_SymbolRef::AIR_SymbolRef() : AIR_Node(AIR_Node_ID::SymbolRef) {}
AIR_SymbolRef::~AIR_SymbolRef() {}


AIR_Immediate::AIR_Immediate() : AIR_Node(AIR_Node_ID::Immediate) {}
AIR_Immediate::~AIR_Immediate() {}


AIR_FuncCall::AIR_FuncCall() : AIR_Node(AIR_Node_ID::FuncCall) {}
AIR_FuncCall::~AIR_FuncCall() {}


AIR_Func::AIR_Func() : AIR_Node(AIR_Node_ID::Func) {}
AIR_Func::~AIR_Func() {}


AIR_Scope::AIR_Scope() : AIR_Node(AIR_Node_ID::Scope) {}
AIR_Scope::~AIR_Scope() {}


void
AIR_Scope::add_decl(AIR_SymbolDecl* decl)
{
	node_vec.push_back(static_cast<AIR_Node*>(decl));
	symbol_table.add_decl(decl);
}


std::string
print_air_node(const AIR_Node* n, bool print_ids)
{
	std::string str = "";

	if(!n)
		return "!NullNode!";
	

	switch(n->id)
	{
		case AIR_Node_ID::BinaryExpr:
		{
			if(print_ids)
				str += "#BinaryExpr ";
		}
		break;
		case AIR_Node_ID::SymbolDecl:
		{
			const AIR_SymbolDecl* decl = static_cast<const AIR_SymbolDecl*>(n);
			if(print_ids)
				str += "#SymbolDecl ";
			str += decl->name + " = " + print_air_node(decl->value, print_ids);
		}
		break;
		case AIR_Node_ID::SymbolRef:
		{
			const AIR_SymbolRef* ref = static_cast<const AIR_SymbolRef*>(n);
			if(print_ids)
				str += "#SymbolRef ";
			str += ref->str;
		}
		break;
		case AIR_Node_ID::Immediate:
		{
			const AIR_Immediate* imm = static_cast<const AIR_Immediate*>(n);
			if(print_ids)
				str += "#Immediate ";
			str += imm->str;
		}
		break;
		case AIR_Node_ID::FuncCall:
		{
			const AIR_FuncCall* fc = static_cast<const AIR_FuncCall*>(n);
			if(print_ids)
				str += "#FuncCall ";
			str += fc->name + "(";

			const AIR_Node* const * arg = fc->arg_vec.data();
			for(; arg != fc->arg_vec.data() + fc->arg_vec.size() - 1; arg++)
				str += print_air_node(*arg, print_ids) + ", ";
			str += print_air_node(*arg, print_ids) + ")";
		}
		break;



		default:
		{
			str += "Unknown";
		}
		break;
	}

	return str;
}
