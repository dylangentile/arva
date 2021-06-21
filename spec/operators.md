# Operators

## `=` The Assignment Operator
The assignment operator takes a value from the right hand side and assigns it to the left hand side. If the types of both sides are not equivalent, the statment is invalid, with the following exceptions:
- If the type of the right hand side and the left hand side have the integer [trait](traits.md), and the bit width of the right hand side is less than the bit width of the right hand side, and both sides have the same signage, the value is implicitly upcasted.
- If the type of the right hand side and the left hand side have the floating [trait](traits.md), and the bit width of the right hand side is less than the bit width of the right hand side, the value is implicitly upcasted.
- If the type of the right hand side is a pointer, and the type of the left hand side is a refrence, and they both point/refrence the same type, then the left hand side refrences the value the right hand side points to.


## `:=`  The declaration assignment operator
This operator declares a new symbol in the current scope on its left hand side, and infers the type of the symbol from its right hand side. Then it assigns the value of the right hand side to the symbol according to the `=` operator's assignment rules. This operator follows a few rules for type infrence:
- If the right hand side is a [comptime expression](comptime.md), then the entire expression is evaluated at compile time.
- If the right hand side is a pointer to a type which has the function [trait](traits.md), then the infered type of the left hand side is a refrence to the type with the function trait.
- If the left hand side has and `&` after its name, than the left hand side will be assigned a reference to the rhs.
- If the left hand side has a `const` before its name, than the left hand side will be assigned a const version of the rhs type.
- If the right hand side doesn't fulfil any of the other type infrence rules, then the left hand side has the equivalent type of the right hand side.