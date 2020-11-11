# Values


Values, or immediates, include numeric values, strings, and boolean values. A integer immediate is written like so: `12432`  for base ten. Arva also supports base 16 following the C notation: `0xFFFF`. 

Immediates of the integer are of the following two styles: `0x2000` (base 16) and `1000` (base 10). Implicitly typed variables will recieve a type of `i64` if the value of the immediate is less than `@max(i64)`, or a type of `u64`. Hexadecimal immediates always imply the `u64` type. In hex immediates the a,b,c,d,e,f digits can be capital or lowercase.