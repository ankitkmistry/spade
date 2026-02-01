# Spade Bytecode Specification

[ ] - What if int overflows during add, sub, mul and casting operations?
[ ] - Create a separate section for truth values
[ ] - Show what castable means
[ ] - Elaborate about erroneous behaviour

This document provides the specification of all the available instructions provided by the Spade Virtual Machine

## Instructions

### `nop` instruction

`nop` is a no-operation instruction. It does nothing.

#### Instruction layout

```text
nop
```

#### Stack layout

|         |     |
| --:     | :-: |
| Initial | ... |
| Final   | ... |

### `const_null` instruction

`const_null` pushes `null` onto the stack.

#### Instruction layout

```text
const_null
```

#### Stack layout

|         |     | 0      |
| --:     | :-: | :--    |
| Initial | ... |        |
| Final   | ... | _null_ |

### `const_true` instruction

`const_true` pushes `true` onto the stack.

#### Instruction layout

```text
const_true
```

#### Stack layout

|         |     | 0      |
| --:     | :-: | :--    |
| Initial | ... |        |
| Final   | ... | _true_ |

### `const_false` instruction

`const_false` pushes `false` onto the stack.

#### Instruction layout

```text
const_false
```

#### Stack layout

|         |     | 0       |
| --:     | :-: | :--     |
| Initial | ... |         |
| Final   | ... | _false_ |

### `const` instruction

`const` loads a constant from the constant pool of the current module and pushes
onto the stack.

#### Instruction layout

```text
const index:u8
```

The first byte is the opcode, then the next one byte denotes the index of
the constant in the current constant pool.

#### Stack layout

|         |     | 0          |
| --:     | :-: | :--        |
| Initial | ... |            |
| Final   | ... | _constant_ |

### `constl` instruction

Same as [`const`](#const-instruction) but `index` is two bytes.

#### Instruction layout

```text
constl index:u16
```

#### Stack layout

|         |     | 0          |
| --:     | :-: | :--        |
| Initial | ... |            |
| Final   | ... | _constant_ |

### `pop` instruction

`pop` pops one value from the stack

#### Instruction layout

```text
pop
```

#### Stack layout

|         |     | 0       |
| --:     | :-: | :--     |
| Initial | ... | _value_ |
| Final   | ... |         |

### `npop` instruction

`npop` pops multiple values from the stack

#### Instruction layout

```text
npop count:u8
```

The first byte is the opcode, then the next one byte denotes the number of values
to be popped from the stack.

#### Stack layout

|         |     | 0        | ... | N - 1    |
| --:     | :-: | :--      | :-- | :--      |
| Initial | ... | _value1_ | ... | _valueN_ |
| Final   | ... |          |     |          |

### `dup` instruction

`dup` pushes the topmost value on the stack. Hence, duplicating the stack top.

#### Instruction layout

```text
dup
```

#### Stack layout

|         |     | 0       | 1       |
| --:     | :-: | :--     | :--     |
| Initial | ... | _value_ |         |
| Final   | ... | _value_ | _value_ |

### `ndup` instruction

`ndup` duplicates the top of stack multiple times

#### Instruction layout

```text
ndup count:u8
```

The first byte is the opcode, then the next one byte denotes the number of times
the value will be duplicated.

#### Stack layout

|         |     | 0       | 1       | ... | N - 1   | N       |
| --:     | :-: | :--     | :--     | :-- | :--     | :--     |
| Initial | ... | _value_ |         | ... |         |         |
| Final   | ... | _value_ | _value_ | ... | _value_ | _value_ |

### `gload` instruction

### `gfload` instruction

Same as [`gload`](#gload-instruction) but `index` is one byte.

#### Instruction layout

```text
gfload index:u8
```

#### Stack layout

|         |     | 0        |
| --:     | :-: | :--      |
| Initial | ... |          |
| Final   | ... | _result_ |

### `gstore` instruction

### `gfstore` instruction

Same as [`gstore`](#gstore-instruction) but `index` is one byte.

#### Instruction layout

```text
gfstore index:u8
```

#### Stack layout

|         |     | 0       |
| --:     | :-: | :--     |
| Initial | ... | _value_ |
| Final   | ... | _value_ |

### `pgstore` instruction

### `pgfstore` instruction

Same as [`pgstore`](#pgstore-instruction) but `index` is one byte.

#### Instruction layout

```text
pgfstore index:u8
```

#### Stack layout

|         |     | 0       |
| --:     | :-: | :--     |
| Initial | ... | _value_ |
| Final   | ... |         |

### `lload` instruction

### `lfload` instruction

Same as [`lload`](#lload-instruction) but `index` is one byte.

#### Instruction layout

```text
lfload index:u8
```

#### Stack layout

|         |     | 0        |
| --:     | :-: | :--      |
| Initial | ... |          |
| Final   | ... | _result_ |

### `lstore` instruction

### `lfstore` instruction

Same as [`lstore`](#lstore-instruction) but `index` is one byte.

#### Instruction layout

```text
lfstore index:u8
```

#### Stack layout

|         |     | 0       |
| --:     | :-: | :--     |
| Initial | ... | _value_ |
| Final   | ... | _value_ |

### `plstore` instruction

### `plfstore` instruction

Same as [`plstore`](#plstore-instruction) but `index` is one byte.

#### Instruction layout

```text
plfstore index:u8
```

#### Stack layout

|         |     | 0       |
| --:     | :-: | :--     |
| Initial | ... | _value_ |
| Final   | ... |         |

### `aload` instruction

### `astore` instruction

### `pastore` instruction

### `mload` instruction

### `mfload` instruction

Same as [`mload`](#mload-instruction) but `index` is one byte.

#### Instruction layout

```text
mfload index:u8
```

#### Stack layout

|         |     | 0        |
| --:     | :-: | :--      |
| Initial | ... | _object_ |
| Final   | ... | _result_ |

### `mstore` instruction

### `mfstore` instruction

Same as [`mstore`](#mstore-instruction) but `index` is one byte.

#### Instruction layout

```text
mfstore index:u8
```

#### Stack layout

|         |     | 0       | 0        |
| --:     | :-: | :--     | :--      |
| Initial | ... | _value_ | _object_ |
| Final   | ... | _value_ |          |

### `pmstore` instruction

### `pmfstore` instruction

Same as [`pmstore`](#pmstore-instruction) but `index` is one byte.

#### Instruction layout

```text
pmfstore index:u8
```

#### Stack layout

|         |     | 0       | 0        |
| --:     | :-: | :--     | :--      |
| Initial | ... | _value_ | _object_ |
| Final   | ... |         |          |

### `spload` instruction

### `spfload` instruction

### `arrpack` instruction

### `arrunpack` instruction

### `arrbuild` instruction

### `arrfbuild` instruction

### `iload` instruction

### `istore` instruction

### `pistore` instruction

### `arrlen` instruction

### `invoke` instruction

### `vinvoke` instruction

### `spinvoke` instruction

### `linvoke` instruction

### `ginvoke` instruction

### `ainvoke` instruction

### `vfinvoke` instruction

### `spfinvoke` instruction

### `lfinvoke` instruction

### `gfinvoke` instruction

### `callsub` instruction

### `retsub` instruction

### `jmp` instruction

### `jt` instruction

### `jf` instruction

### `jlt` instruction

### `jle` instruction

### `jeq` instruction

### `jne` instruction

### `jge` instruction

### `jgt` instruction

### `not` instruction

`not` pops the stack to get a object, then it pushes the opposite truth value of the object.


Truth value of an object is defined as follows:

| Popped value     | Truth value                        |
| ---              | ---                                |
| `null`           | `false`                            |
| `true`           | `true`                             |
| `false`          | `false`                            |
| `int`            | `if num is 0 then false else true` |
| `float`          | `if num is 0 then false else true` |
| any other object | `true`                             |

#### Instruction layout

```text
not
```

#### Stack layout

|         |     | 0        |
| --:     | :-: | :--      |
| Initial | ... | _value_  |
| Final   | ... | _result_ |

#### Pseudocode

```c
result = !value.truth(); // result is bool
```

### `inv` instruction

`inv` pops the stack to get an integer, then it flips all the bits of the integer and pushes the value.

#### Instruction layout

```text
inv
```

#### Stack layout

|         |     | 0        |
| --:     | :-: | :--      |
| Initial | ... | _value_  |
| Final   | ... | _result_ |

#### Pseudocode

```c
if (value.is_int())
    result = ~value.as_int();
else if (value.is_uint())
    result = ~value.as_uint();
else
    trigger_erroneous_behaviour();
```

### `neg` instruction

`neg` pops the stack to get a number, then it negates the number and pushes the value.

#### Instruction layout

```text
neg
```

#### Stack layout

|         |     | 0        |
| --:     | :-: | :--      |
| Initial | ... | _value_  |
| Final   | ... | _result_ |

#### Pseudocode

```c
if (value.is_int())
    result = -value.as_int();       // result is int // Basic negation
else if (value.is_uint())
    result = -value.as_uint();      // result is uint // Get two's complement
else if (value.is_float())
    result = -value.as_float();     // result is float // Basic negation
else
    trigger_erroneous_behaviour();
```

### `gettype` instruction

`gettype` pops the stack to get an object, then it pushes the type of the object.

#### Instruction layout

```text
gettype
```

#### Stack layout

|         |     | 0        |
| --:     | :-: | :--      |
| Initial | ... | _value_  |
| Final   | ... | _result_ |

#### Pseudocode

```c
result = value.get_type(); // result is type
```

### `scast` instruction

`scast` pops the stack to get an type and an object, then it checks whether the object
can be cast to the type or not. If the object is castable, then it pushes the object.
If the object is not castable then it pushes `null`.

#### Instruction layout

```text
scast
```

#### Stack layout

|         |     | 0        | 1        |
| --:     | :-: | :--      | :--      |
| Initial | ... | _value_  | _type_   |
| Final   | ... | _result_ |          |

#### Pseudocode

```c
if (value can be cast to type)
    result = value;
else
    result = null;
```

### `ccast` instruction

`ccast` pops the stack to get an type and an object, then it checks whether the object
can be cast to the type or not. If the object is castable, then it pushes the object.
If the object is not castable then it throws a runtime error.

#### Instruction layout

```text
ccast
```

#### Stack layout

|         |     | 0        | 1      |
| --:     | :-: | :--      | :--    |
| Initial | ... | _value_  | _type_ |
| Final   | ... | _result_ |        |

#### Pseudocode

```c
if (value can be cast to type)
    result = value;
else
    runtime_error();
```

### `concat` instruction

`concat` pops the stack to get two strings and pushes the concatenated result of the two strings.

#### Instruction layout

```text
concat
```

#### Stack layout

|         |     | 0        | 1        |
| --:     | :-: | :--      | :--      |
| Initial | ... | _value1_ | _value2_ |
| Final   | ... | _result_ |          |

#### Pseudocode

```c
if (value1.is_string() && value2.is_string())
    result = value1.as_string().concat(value2.as_string()); // result is string
else
    trigger_erroneous_behaviour();
```

### `pow` instruction

`pow` pops the stack to get two numbers and pushes their exponent result.

#### Instruction layout

```text
pow
```

#### Stack layout

|         |     | 0        | 1        |
| --:     | :-: | :--      | :--      |
| Initial | ... | _value1_ | _value2_ |
| Final   | ... | _result_ |          |

#### Pseudocode

```c
if (value1.is_int() && value2.is_int())
    result = value1.as_int().power(value2.as_int()); // result is float
else if (value1.is_uint() && value2.is_uint())
    result = value1.as_uint().power(value2.as_uint()); // result is float
else if (value1.is_float() && value2.is_float())
    result = value1.as_float().power(value2.as_float()); // result is float
else
    trigger_erroneous_behaviour();
```

### `mul` instruction

`mul` pops the stack to get two numbers and pushes their product.

#### Instruction layout

```text
mul
```

#### Stack layout

|         |     | 0        | 1        |
| --:     | :-: | :--      | :--      |
| Initial | ... | _value1_ | _value2_ |
| Final   | ... | _result_ |          |

#### Pseudocode

```c
if (value1.is_int() && value2.is_int())
    result = value1.as_int() * value2.as_int(); // result is int
else if (value1.is_uint() && value2.is_uint())
    result = value1.as_uint() * value2.as_uint(); // result is uint
else if (value1.is_float() && value2.is_float())
    result = value1.as_float() * value2.as_float(); // result is float
else
    trigger_erroneous_behaviour();
```

### `div` instruction

`div` pops the stack to get two numbers and pushes their division result.

#### Instruction layout

```text
div
```

#### Stack layout

|         |     | 0        | 1        |
| --:     | :-: | :--      | :--      |
| Initial | ... | _value1_ | _value2_ |
| Final   | ... | _result_ |          |

#### Pseudocode

```c
if (value1.is_int() && value2.is_int())
    result = value1.as_int() / value2.as_int(); // result is int
else if (value1.is_uint() && value2.is_uint())
    result = value1.as_uint() / value2.as_uint(); // result is uint
else if (value1.is_float() && value2.is_float())
    result = value1.as_float() / value2.as_float(); // result is float
else
    trigger_erroneous_behaviour();
```

### `rem` instruction

`rem` pops the stack to get two numbers and pushes their remainder.

#### Instruction layout

```text
rem
```

#### Stack layout

|         |     | 0        | 1        |
| --:     | :-: | :--      | :--      |
| Initial | ... | _value1_ | _value2_ |
| Final   | ... | _result_ |          |

#### Pseudocode

```c
if (value1.is_int() && value2.is_int())
    result = value1.as_int() % value2.as_int(); // result is int
else if (value1.is_uint() && value2.is_uint())
    result = value1.as_uint() % value2.as_uint(); // result is uint
else
    trigger_erroneous_behaviour();
```

### `add` instruction

`add` pops the stack to get two numbers and pushes their sum.

#### Instruction layout

```text
add
```

#### Stack layout

|         |     | 0        | 1        |
| --:     | :-: | :--      | :--      |
| Initial | ... | _value1_ | _value2_ |
| Final   | ... | _result_ |          |

#### Pseudocode

```c
if (value1.is_int() && value2.is_int())
    result = value1.as_int() + value2.as_int(); // result is int
else if (value1.is_uint() && value2.is_uint())
    result = value1.as_uint() + value2.as_uint(); // result is uint
else if (value1.is_float() && value2.is_float())
    result = value1.as_float() + value2.as_float(); // result is float
else
    trigger_erroneous_behaviour();
```

### `sub` instruction

`sub` pops the stack to get two numbers and pushes their subtraction result.

#### Instruction layout

```text
sub
```

#### Stack layout

|         |     | 0        | 1        |
| --:     | :-: | :--      | :--      |
| Initial | ... | _value1_ | _value2_ |
| Final   | ... | _result_ |          |

#### Pseudocode

```c
if (value1.is_int() && value2.is_int())
    result = value1.as_int() - value2.as_int(); // result is int
else if (value1.is_uint() && value2.is_uint())
    result = value1.as_uint() - value2.as_uint(); // result is uint
else if (value1.is_float() && value2.is_float())
    result = value1.as_float() - value2.as_float(); // result is float
else
    trigger_erroneous_behaviour();
```

### `shl` instruction

`shl` pops the stack to get two numbers, performs bitwise shift left operation and pushes the result.

#### Instruction layout

```text
shl
```

#### Stack layout

|         |     | 0        | 1        |
| --:     | :-: | :--      | :--      |
| Initial | ... | _value1_ | _value2_ |
| Final   | ... | _result_ |          |

#### Pseudocode

```c
if (value1.is_int() && value2.is_int())
    result = value1.as_int() << value2.as_int(); // result is int
else if (value1.is_uint() && value2.is_uint())
    result = value1.as_uint() << value2.as_uint(); // result is uint
else
    trigger_erroneous_behaviour();
```

### `shr` instruction

`shr` pops the stack to get two numbers, performs bitwise shift right operation and pushes the result.

#### Instruction layout

```text
shr
```

#### Stack layout

|         |     | 0        | 1        |
| --:     | :-: | :--      | :--      |
| Initial | ... | _value1_ | _value2_ |
| Final   | ... | _result_ |          |

#### Pseudocode

```c
if (value1.is_int() && value2.is_int())
    // sign extension happens here
    result = value1.as_int() >> value2.as_int(); // result is int
else if (value1.is_uint() && value2.is_uint())
    result = value1.as_uint() >> value2.as_uint(); // result is uint
else
    trigger_erroneous_behaviour();
```

### `ushr` instruction

`ushr` pops the stack to get two numbers, performs bitwise unsigned shift right operation and pushes the result.

#### Instruction layout

```text
ushr
```

#### Stack layout

|         |     | 0        | 1        |
| --:     | :-: | :--      | :--      |
| Initial | ... | _value1_ | _value2_ |
| Final   | ... | _result_ |          |

#### Pseudocode

```c
if (value1.is_int() && value2.is_int())
    // no sign extension happens here
    result = (uint64_t)value1.as_int() >> value2.as_int(); // result is int
else if (value1.is_uint() && value2.is_uint())
    result = value1.as_uint() >> value2.as_uint(); // result is uint
else
    trigger_erroneous_behaviour();
```

### `rol` instruction

`rol` pops the stack to get two numbers, performs bitwise left rotation and pushes the result.

#### Instruction layout

```text
rol
```

#### Stack layout

|         |     | 0        | 1        |
| --:     | :-: | :--      | :--      |
| Initial | ... | _value1_ | _value2_ |
| Final   | ... | _result_ |          |

#### Pseudocode

```c
if (value1.is_int() && value2.is_int())
    result = value1.as_int().rotate_left(value2.as_int()); // result is int
else if (value1.is_uint() && value2.is_uint())
    result = value1.as_uint().rotate_left(value2.as_uint()); // result is uint
else
    trigger_erroneous_behaviour();
```

### `ror` instruction

`rol` pops the stack to get two numbers, performs bitwise right rotation and pushes the result.

#### Instruction layout

```text
rol
```

#### Stack layout

|         |     | 0        | 1        |
| --:     | :-: | :--      | :--      |
| Initial | ... | _value1_ | _value2_ |
| Final   | ... | _result_ |          |

#### Pseudocode

```c
if (value1.is_int() && value2.is_int())
    result = value1.as_int().rotate_right(value2.as_int()); // result is int
else if (value1.is_uint() && value2.is_uint())
    result = value1.as_uint().rotate_right(value2.as_uint()); // result is uint
else
    trigger_erroneous_behaviour();
```

### `and` instruction

`and` pops the stack to get two numbers, performs bitwise and operation and pushes the result.

#### Instruction layout

```text
and
```

#### Stack layout

|         |     | 0        | 1        |
| --:     | :-: | :--      | :--      |
| Initial | ... | _value1_ | _value2_ |
| Final   | ... | _result_ |          |

#### Pseudocode

```c
if (value1.is_int() && value2.is_int())
    result = value1.as_int() & value2.as_int(); // result is int
else if (value1.is_uint() && value2.is_uint())
    result = value1.as_uint() & value2.as_uint(); // result is uint
else
    trigger_erroneous_behaviour();
```

### `or` instruction

`or` pops the stack to get two numbers, performs bitwise or operation and pushes the result.

#### Instruction layout

```text
or
```

#### Stack layout

|         |     | 0        | 1        |
| --:     | :-: | :--      | :--      |
| Initial | ... | _value1_ | _value2_ |
| Final   | ... | _result_ |          |

#### Pseudocode

```c
if (value1.is_int() && value2.is_int())
    result = value1.as_int() | value2.as_int(); // result is int
else if (value1.is_uint() && value2.is_uint())
    result = value1.as_uint() | value2.as_uint(); // result is uint
else
    trigger_erroneous_behaviour();
```

### `xor` instruction

`xor` pops the stack to get two numbers, performs bitwise xor operation and pushes the result.

#### Instruction layout

```text
xor
```

#### Stack layout

|         |     | 0        | 1        |
| --:     | :-: | :--      | :--      |
| Initial | ... | _value1_ | _value2_ |
| Final   | ... | _result_ |          |

#### Pseudocode

```c
if (value1.is_int() && value2.is_int())
    result = value1.as_int() ^ value2.as_int(); // result is int
else if (value1.is_uint() && value2.is_uint())
    result = value1.as_uint() ^ value2.as_uint(); // result is uint
else
    trigger_erroneous_behaviour();
```

### `lt` instruction

### `le` instruction

### `eq` instruction

### `ne` instruction

### `ge` instruction

### `gt` instruction

### `is` instruction

`is` pops two objects from the stack and checks whether both the objects 
are located at the same memory location. It pushes `true` if the objects
are located at the same memory location and `false` otherwise.

#### Instruction layout

```text
is
```

#### Stack layout

|         |     | 0        | 1        |
| --:     | :-: | :--      | :--      |
| Initial | ... | _value1_ | _value2_ |
| Final   | ... | _result_ |          |

#### Pseudocode

```c
if (value1.is_obj() && value2.is_obj())
    result = value1.as_obj() == value2.as_obj(); // result is bool
else
    trigger_erroneous_behaviour();
```

### `nis` instruction

`nis` pops two objects from the stack and checks whether both the objects 
are located at the same memory location. It pushes `false` if the objects
are located at the same memory location and `true` otherwise.

#### Instruction layout

```text
nis
```

#### Stack layout

|         |     | 0        | 1        |
| --:     | :-: | :--      | :--      |
| Initial | ... | _value1_ | _value2_ |
| Final   | ... | _result_ |          |

#### Pseudocode

```c
if (value1.is_obj() && value2.is_obj())
    result = value1.as_obj() != value2.as_obj(); // result is bool
else
    trigger_erroneous_behaviour();
```

### `isnull` instruction

`isnull` pops a object from the stack and checks whether the object is null
It pushes `true` if the object is null, and `false` otherwise. 

#### Instruction layout

```text
isnull
```

#### Stack layout

|         |     | 0        |
| --:     | :-: | :--      |
| Initial | ... | _value_  |
| Final   | ... | _result_ |

#### Pseudocode

```c
result = value.is_null(); // result is bool
```

### `nisnull` instruction

`nisnull` pops a object from the stack and checks whether the object is null
It pushes `false` if the object is null, and `true` otherwise. 

#### Instruction layout

```text
nisnull
```

#### Stack layout

|         |     | 0        |
| --:     | :-: | :--      |
| Initial | ... | _value_  |
| Final   | ... | _result_ |

#### Pseudocode

```c
result = !value.is_null(); // result is bool
```

### `i2u` instruction

`i2u` pops an integer, converts it into an unsigned integer, and pushes the converted
value onto the stack. The conversion is just bitwise casting and no operations are
done other than changing the value's type.

#### Instruction layout

```text
i2u
```

#### Stack layout

|         |     | 0        |
| --:     | :-: | :--      |
| Initial | ... | _value_  |
| Final   | ... | _result_ |

#### Pseudocode

```c
if (value.is_int())
    result = convert_int_to_uint(value); // result is uint
else 
    trigger_erroneous_behaviour();
```

### `u2i` instruction

`u2i` pops an unsigned integer, converts it into an integer, and pushes the converted
value onto the stack. The conversion is just bitwise casting and no operations are
done other than changing the value's type.

#### Instruction layout

```text
u2i
```

#### Stack layout

|         |     | 0        |
| --:     | :-: | :--      |
| Initial | ... | _value_  |
| Final   | ... | _result_ |

#### Pseudocode

```c
if (value.is_uint())
    result = convert_uint_to_int(value); // result is int
else 
    trigger_erroneous_behaviour();
```

### `u2f` instruction

`u2f` pops an unsigned integer, converts it into a float, and pushes the converted
value onto the stack.

#### Instruction layout

```text
u2f
```

#### Stack layout

|         |     | 0        |
| --:     | :-: | :--      |
| Initial | ... | _value_  |
| Final   | ... | _result_ |

#### Pseudocode

```c
if (value.is_uint())
    result = convert_uint_to_float(value); // result is float
else 
    trigger_erroneous_behaviour();
```

### `i2f` instruction

`i2f` pops an integer, converts it into a float, and pushes the converted
value onto the stack.

#### Instruction layout

```text
i2f
```

#### Stack layout

|         |     | 0        |
| --:     | :-: | :--      |
| Initial | ... | _value_  |
| Final   | ... | _result_ |

#### Pseudocode

```c
if (value.is_int())
    result = convert_int_to_float(value); // result is float
else 
    trigger_erroneous_behaviour();
```

### `f2i` instruction

`f2i` pops a float, converts it into an integer, and pushes the converted
value onto the stack. Conversion happens by truncating all the digits after the
decimal point.

#### Instruction layout

```text
f2i
```

#### Stack layout

|         |     | 0        |
| --:     | :-: | :--      |
| Initial | ... | _value_  |
| Final   | ... | _result_ |

#### Pseudocode

```c
if (value.is_int())
    result = convert_float_to_int(value); // result is int
else 
    trigger_erroneous_behaviour();
```

### `i2b` instruction

`i2b` pops an integer, converts it into a bool, and pushes the converted
value onto the stack. Conversion happens by pushing the truth value of the integer.

#### Instruction layout

```text
i2b
```

#### Stack layout

|         |     | 0        |
| --:     | :-: | :--      |
| Initial | ... | _value_  |
| Final   | ... | _result_ |

#### Pseudocode

```c
if (value.is_int())
    result = value.truth(); // result is bool
else 
    trigger_erroneous_behaviour();
```

### `b2i` instruction

`b2i` pops a bool, converts it into an integer, and pushes the converted
value onto the stack. Conversion happens by pushing 1 if bool is true and pushing
0 if bool is false.

#### Instruction layout

```text
b2i
```

#### Stack layout

|         |     | 0        |
| --:     | :-: | :--      |
| Initial | ... | _value_  |
| Final   | ... | _result_ |

#### Pseudocode

```c
if (value.is_bool())
    result = value.as_bool() ? 1 : 0; // result is int
else 
    trigger_erroneous_behaviour();
```

### `o2b` instruction

`o2b` pops an object, converts it into a bool, and pushes the converted
value onto the stack. Conversion happens by pushing the truth value of the object.

#### Instruction layout

```text
o2b
```

#### Stack layout

|         |     | 0        |
| --:     | :-: | :--      |
| Initial | ... | _value_  |
| Final   | ... | _result_ |

#### Pseudocode

```c
result = value.truth(); // result is bool
```

### `o2s` instruction

`o2s` pops an object, converts it into a string, and pushes the converted
value onto the stack. Conversion happens by pushing the vm-specific string 
representation of the object.

#### Instruction layout

```text
o2s
```

#### Stack layout

|         |     | 0        |
| --:     | :-: | :--      |
| Initial | ... | _value_  |
| Final   | ... | _result_ |

#### Pseudocode

```c
result = value.to_string(); // result is string
```

### `entermonitor` instruction

### `exitmonitor` instruction

### `mtperf` instruction

### `mtfperf` instruction

### `closureload` instruction

`closureload` pops the stack to get a method. The method is loaded with
closed variables from the current method and pushed onto the stack. This
is a variable-length instruction.

#### Instruction layout

```text
closureload capture_count:u8
    ( capture_dest:u16 capture_type:u8 capture_from:u8|u16 )*
```

The first byte is the opcode, then the next byte denotes the number of
variables to be closed (say `N`). Then, `N` entries of capture information
is encoded from the byte after `capture_count` as follows:

- The first two bytes denotes the local index of the destination method
    where the closed variables can be accessed
- The next byte denotes the kind of the variable to be captured:
  - If `capture_type` is 0x00, then the kind of the variable to be captured
      is a argument of the current method
  - If `capture_type` is 0x01, then the kind of the variable to be captured
      is a local of the current method
- The next one or two bytes (depending upon `capture_type`) denotes the location
    of the variable to be captured from the current method.
  - If `capture_type` is 0x00, then `capture_from` is one byte and it denotes
      the arg index of the argument to be captured
  - If `capture_type` is 0x01, then `capture_from` is two bytes and it denotes
      the local index of the local to be captured

#### Stack layout

|         |     | 0                 |
| --:     | :-: | :--               |
| Initial | ... | _method_          |
| Final   | ... | _captured method_ |

### `objload` instruction

### `throw` instruction

### `ret` instruction

### `vret` instruction
