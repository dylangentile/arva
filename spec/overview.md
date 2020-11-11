# Overview

The Arva programming language serves as a programming language designed to make a compromise between the safety guarentees of rust, the explicit error handling of go and zig, the oop and templating system of C++, and the simplicity and eloquence of C and Lisp. It can be stylized as "Arva", "arva", or "ARVA". These are all interchangeable.

*Note: this guide assumes you are familiar with programming in a statically typed, low-level language such as Rust, Zig, Swift, C++, C, along with the C memory-model.*



## Basics
Here is hello world in Arva:

```arva
std := @import("std");
print := std.io.print;


main := (cstring[] args -> i32)
{
	print("Hello World!\n");
	return 0;
}

```
*hello.arva*

Lets go over this in depth.
The first line: `std := @import("std");`. First we take a symbol, `std`, and assign to it the result of the expression `@import("std");`. Because we used the `:=` operator, the type of std is inferred from the expression on the right hand side. An explicit way to write this would be `comptime const namespace& std = @import("std");`. (Import can only be run within a comptime block, hence it has special behavior where the comptime is also inferred from the `:=` operator. If a function is defined as comptime and then the `:=` operator always wraps the declaration in comptime).

Second line: `print := std.io.print;`. This expands to `comptime const (string msg -> void)* print = std.io.print;` This means that we have a compiletime function pointer to a function of signature `(string -> void)`. Note that const wraps everything after it, its a constant pointer to a constant function.

The fifth line: By taking a functions signature/type `(cstring[] args -> i32)`, and following with a function body `{...}`, we have formed a function "object". Function objects are `constant` and `exectuable` and have no other traits. Then the symbol main is assigned a pointer to the function object: `const (cstring[] args -> i32)* main = (cstring[] args -> i32){...}`.  The function signature is formed like so: `(T, T, ... -> T)`


### On `const`
Treat the `const` keyword like multiplication: it distributes (*to everything within the parentheses it is in*).   `const int***` means a constant pointer to a constant pointer to a constant pointer to a constant int. If this isn't the desired behaviour, you can utilize parantheses in your type formulations. `(const int)***` or `int (const *)`.

### On parentheses in types
Types and their "operators/attributes" have precedences and "arithmetic" rules in Arva. Parentheses can help specify the importance of a type or a composition. We can come up with a basic syntax: type `T` is a structure/class/basic type or `T` takes the form `(M T A...)`. The definition recurses.  `M` is a modifier (usually const), and `A` is an "attribute". `A` is defined as `(M A)` or `*`, `?*`, `&`. `M` and `A` are both optional in the `T` definition, and the parentheses are only necessary to specify modifier precedence. So we could write `int***` as `((int)(*)(*)(*))` if we were so inclined. This allows complicated type deinitions like so `((const int)(*)(*)(const *))`, which would be a constant pointer to a pointer to a pointer to a constant int. Parentheses can be eliminated if there is no modifier so the previous example can be written: `(const int)**(const*)`. Because `const`/modifiers in general, distribute to everything within their parentheses removing either of the two sets of parentheses in the previous example will make everything const, thus there is an exception to the rule. *Only a leading const can distribute to everything*. This helps prevent syntatical bugs by developers that may be hard to spot, like `(const int)**const*`. They probably weren't intending to make the whole thing const there, so we treat it as an error in violation of our own type system. It's more idomatic to lead with `const` anyway, too many headaches for everyone.





