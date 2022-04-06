#include "air.h"



AIR_Node::AIR_Node(const AIR_Node_ID id_) : id(id_) {}
AIR_Node::~AIR_Node() {}


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



