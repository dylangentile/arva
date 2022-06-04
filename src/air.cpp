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


AIR_Assign::AIR_Assign() : AIR_Node(AIR_Node_ID::Assign) {}
AIR_Assign::~AIR_Assign() {}


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


AIR_Struct::AIR_Struct() : AIR_Node(AIR_Node_ID::Struct) {}
AIR_Struct::~AIR_Struct() {}


AIR_Return::AIR_Return() : AIR_Node(AIR_Node_ID::Return) {}
AIR_Return::~AIR_Return() {}

void
AIR_Struct::add_field(const Field& f)
{
	const size_t idx = field_vec.size();
	field_vec.push_back(f);
	field_map.insert({f.name, idx});
}



std::string
print_air_node(const AIR_Node* n, bool print_ids, std::string prepend)
{
	std::string str = prepend;

	if(!n)
		return "!NullNode!";
	

	switch(n->id)
	{
		case AIR_Node_ID::BinaryExpr:
		{
			const AIR_BinaryExpr* bin_expr = static_cast<const AIR_BinaryExpr*>(n);
			if(print_ids)
				str += "#BinaryExpr ";
			str += print_air_node(bin_expr->rhs, false) + " " + print_air_node(bin_expr->lhs, false) + " ";
			str += tok_enum_to_string[(uint16_t)bin_expr->op]; 
			//we don't need to worry about handling all of the enum, 
			//since the unary ops are a unary expr, and the others are helpers for parsing, but dont produce valid air

		}
		break;
		case AIR_Node_ID::SymbolDecl:
		{
			const AIR_SymbolDecl* decl = static_cast<const AIR_SymbolDecl*>(n);
			if(print_ids)
				str += "#SymbolDecl ";
			str += decl->name + " = \t" + print_air_node(decl->value, print_ids);
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
		case AIR_Node_ID::Assign:
		{
			const AIR_Assign* assign = static_cast<const AIR_Assign*>(n);
			if(print_ids)
				str += "#Assign ";
			str += print_air_node(assign->assignee, print_ids) + " = \t" + print_air_node(assign->value, print_ids);
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

			if(fc->arg_vec.empty())
			{
				str += ")";
				break;
			}

			const AIR_Node* const * arg = fc->arg_vec.data();
			for(; arg != fc->arg_vec.data() + fc->arg_vec.size() - 1; arg++)
				str += print_air_node(*arg, print_ids) + ", ";
			str += print_air_node(*arg, print_ids) + ")";
		}
		break;
		case AIR_Node_ID::Func:
		{
			const AIR_Func* fn = static_cast<const AIR_Func*>(n);
			if(print_ids)
				str += "#Func ";

			str += "(";

			if(!fn->arg_vec.empty())
			{
				const AIR_Func::Arg* arg = fn->arg_vec.data();

				for(; arg != fn->arg_vec.data() + fn->arg_vec.size() - 1; arg++)
					str += arg->type.print() + " " + arg->name + ", ";
				str += arg->type.print() + " " + arg->name + " ";
			}

			str += "-> "  + fn->return_type.print() + ")";
			str += "\n" + print_air_node(fn->scope, print_ids, prepend + "\t");
		}
		break;
		case AIR_Node_ID::Scope:
		{
			const AIR_Scope* scope = static_cast<const AIR_Scope*>(n);
			if(print_ids)
				str += "#Scope Start\n";

			if(str == prepend)
				str.clear();

			for(const AIR_Node* node : scope->node_vec)
				str += prepend + print_air_node(node, print_ids) + "\n";

			if(print_ids)
				str += prepend + "#Scope End\n";
		}
		break;
		case AIR_Node_ID::Struct:
		{
			const AIR_Struct* st = static_cast<const AIR_Struct*>(n);
			if(print_ids)
				str += "#Struct ";

			str += "{";

			if(st->field_vec.empty())
			{
				str += "}";
				break;
			}

			const AIR_Struct::Field* f = st->field_vec.data();
			for(; f != st->field_vec.data() + st->field_vec.size() - 1; f++)
				str += f->type.print() + " " + f->name + ", ";
			str += f->type.print() + " " + f->name + "}";
		}
		break;
		case AIR_Node_ID::Return:
		{
			const AIR_Return* ret = static_cast<const AIR_Return*>(n);
			str += "return " + print_air_node(ret->return_expr, print_ids);
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
