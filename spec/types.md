# Types
All types have [traits](traits.md). These traits determine how each type is used.


## Integer Types
**Traits**: `integer`

- `i8`
- `i16`
- `i32`
- `i64`

- `u8`
- `u16`
- `u32`
- `u64`

- `size_t` equal to the address width of the system. It is the maximum pointer value and maximum amount of memory that can theoretically be allocated upon the system. It is unsigned.


## Floating Types
**Traits**: `floating`

- `float`  32 bit floating point
- `double` 64 bit floating point
- `double_ext` widest floating point type

## String Types
### `utf_char`
**Traits**: `compareable`, `inequality`
An u32 value representing a unicode character.

### `cstring`
**Traits**: `basic_container`, `random_access`, `compareable`, `hashable`
A `cstring` is static array of `u8` values. It stores its data in UTF-8 format.

### `string`
**string**: `dynamic_container`, `random_access` `compareable`, `hashable`
A `string` is a dynamic array of `u8` values. It stores its data in UTF-8 format.

## Boolean type
### `bool`
**Traits**: `compareable`, `copyable`, `movable`, `refrenceable`
A boolean is either `true` or `false`.

## Void type
### `void`
**Traits**: `unusable`



## Type Compositions

### Pointers
A pointer is formed by taking a type `T` and adding a `*` after it. `T*` is a type that represents a pointer to a value of `T` type `T`. Special pointers can be formed by prefixing the type with `unique`, or `shared`, which form their own types.

#### Basic/Raw Pointer
A basic pointer, or raw pointer, is an address that when "derefrenced", points to a value of type `T`. For instance, a pointer to a constant utf_char would be formed like so: `(const utf_char)* ptr`. A constant pointer to a utf_char would be formed like so: `utf_char(const *) ptr`, and a constant pointer to a constant utf_char* would look like so: `const utf_char* ptr`. The data of the pointer type is **usually** an address that points to data. To safely work with pointers it is reccommended to use the optional pointer  `?*` type, however the [compiler_constant](compiler_constants.md) `nullptr` is supplied to assist with low level memory work where the optional pointer isn't applicable.
#### Optional Pointer
The optional pointer `?*` works just like the raw pointer, with the exception that it must be checked to not point to unallocated or freed data. If one takes the pointer `utf_char?* ptr` that has just been allocated, and we would like to derefrence it, first we must `try ptr.check();`, then within the scope we are operating and any nested scopes, we can treat the optional pointer as a normal pointer. If we want to get the raw pointer from the optional pointer we `utf_char* x = @unwrap(ptr);`.

#### Shared/Unique Pointer
Whenever a shared pointer is copied, its internal ref-count increments. If the ref-count == 0, it frees the data. `shared T* ptr` denotes a shared ptr, `shared T* ptr2 = ptr` denotes a copy. Unique pointer uses move semantics.


#### Arrays
An array is formed by taking a type `T` and writing a `[S]` after, where S is a comptime u64 or an immediate. An array is laid out in memory with 8 bytes representing its size(bytes) followed by the contigous chunk of memory. An array that is declared without a rhs will run the "constructor" on each element, or zero them, depending on if its a class, or a (struct/enum). `T[5] array1; array2 := array1;` represents a copy. `array2` is a copy of `array1`, this is the same with passing an array in a function. 


### Functions



