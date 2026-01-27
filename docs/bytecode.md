# Spade Bytecode Specification

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

### `const_true` instruction

### `const_false` instruction

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

Same as `const` but `index` is two bytes.

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

### `gstore` instruction

### `gfstore` instruction

### `pgstore` instruction

### `pgfstore` instruction

### `lload` instruction

### `lfload` instruction

### `lstore` instruction

### `lfstore` instruction

### `plstore` instruction

### `plfstore` instruction

### `aload` instruction

### `astore` instruction

### `pastore` instruction

### `mload` instruction

### `mfload` instruction

### `mstore` instruction

### `mfstore` instruction

### `pmstore` instruction

### `pmfstore` instruction

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

### `inv` instruction

### `neg` instruction

### `gettype` instruction

### `scast` instruction

### `ccast` instruction

### `concat` instruction

### `pow` instruction

### `mul` instruction

### `div` instruction

### `rem` instruction

### `add` instruction

### `sub` instruction

### `shl` instruction

### `shr` instruction

### `ushr` instruction

### `rol` instruction

### `ror` instruction

### `and` instruction

### `or` instruction

### `xor` instruction

### `lt` instruction

### `le` instruction

### `eq` instruction

### `ne` instruction

### `ge` instruction

### `gt` instruction

### `is` instruction

### `nis` instruction

### `isnull` instruction

### `nisnull` instruction

### `i2f` instruction

### `f2i` instruction

### `i2b` instruction

### `b2i` instruction

### `o2b` instruction

### `o2s` instruction

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

|         |     |                   |
| --:     | :-: | :--               |
| Initial | ... | _method_          |
| Final   | ... | _captured method_ |

### `objload` instruction

### `throw` instruction

### `ret` instruction

### `vret` instruction
