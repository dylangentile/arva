# Overview

The Arva programming language serves as a programming language designed to make a compromise between the safety guarentees of rust, the explicit error handling of go and zig, the oop and templating system of C++, and the simplicity and eloquence of C and Lisp. It can be stylized as "Arva", "arva", or "ARVA". These are all interchangeable.

*Note: this guide assumes you are familiar with programming in a statically typed, low-level language such as Rust, Zig, Swift, C++, C, along with the C memory-model.*



## Basics
Here is hello world in Arva:

```arva
std := import("std");
print := std.io.print;


main := (string[] args -> i32)
{
	print("Hello World!\n");
	return 0;
}

```
*hello.arva*

Lets go over this in depth.
The first line: `std := import("std");`. First we take a symbol, `std`, and assign to it the result of the expression `import("std");`. Because we used the `:=` operator, the type of std is inferred from the expression on the right hand side. An explicit way to write this would be `comptime const namespace std = import("std");`. (Import can only be run within a comptime block, hence it has special behavior where the comptime is also inferred from the `:=` operator. If a function is defined as comptime and then the `:=` operator always wraps the declaration in comptime).

Second line: `print := std.io.print;`. This expands to `comptime const (string msg -> void)& print = std.io.print;` This means that we have a compiletime function refrence to a function of signature `(string -> void)`. Note that const wraps everything after it, its a constant refrence to a constant function.


