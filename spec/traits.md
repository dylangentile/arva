# Traits

## On `const`

The `const` key word wraps the entire type after it, and adds the `constant` keyword to the type's traits.


## All of the traits
Note that some traits cannot be implemented, such as the `arithmetic` trait. That is because the language does not support general operator overloading.

### `copyable`

### `movable`

### `refrenceable`
Refers to if the address of the types data can be read. (`int* y = &x;`)

### `arithmetic`
Do the standard `+` `-` `*` and `/` arithmetic operators apply?

### `inequality`
Do the standard `<` and `>` inequality operators apply?

### `compareable`
Can this type's data be compared?

### `iterable`
Can this type's data be iterated?

### `random_access`
Can this type's data be randomly accessed?

### `sized`
Does this type have an internal data size?

### `constant`
Is this type's data be not modifiable or "constant"?

### `hashable`
Does this type support hashing other than raw byte hashing?

### `executable`
Can this type be executed?


### Trait Compositions
These are traits that also bring other traits with them, but are their own unique traits. (ie, a trait with `compareable` `arithmetic` and `inequality` is **NOT** equivalent to the `integer` trait. Instead the integer trait is its own trait, that just brings other traits with it.)

#### `integer`
`integer` brings: `arithmetic`, `inequality`, `comparable`, `copyable`, `movable`, `refrenceable`

#### `floating`
`floating` brings: `arithmetic`, `inequality`, `comparable`, `copyable`, `movable`, `refrenceable`

#### `function`
`function` brings: `movable`, `refrenceable`, `constant`

### `unusable`
This is a trait for the void type. It isn't really a composition, but instead a lack thereof.

#### `basic_container`
`static_container` brings: `iterable`, `sized`, `constant`

#### `dynamic_container`
`dynamic_container` brings: `iterable`, `sized`





