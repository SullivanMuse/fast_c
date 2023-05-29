# Fast

In order to hone my C skills, I am going to make an interpreter for the Fast language in C.

## Features

Fast is a simple imperative language, but it will have some advanced features.

- Let statements, for declaring and optionally defining variables
- Values
    - Integers `1234`
        - Arithmetic operators `+ - * /`
        - Bitwise operators `~ & | ^ << >>`
    - Immutable strings `"Hello, world!"`
        - Efficient formatting `"{name} is looking good today!"`
    - Objects `{x = 1, y = 2}`
        - Concatenation `{..xs, ..ys}`. `xs` and `ys` must have a disjoint set of keys.
    - Variants `:variant1`
    - Macro expansion and staging system
- Simple file-based module system
    - `use mod1` includes the code from `mod1.fast` under the `mod1` module
    - `use ..module` includes the code from `../module.fast`
    - `use mod1.mod2` includes the code from `./mod1/mod2.fast` as `mod2`
- Debugging
    - `breakpoint()` insert a breakpoint
    - `breakpoint(condition)` breakpoint if condition
- Compile-time reflection
- Macros and staging
- Bytecode compiler
- Optimizing transpiler (to C)

## Ecosystem

Any good language must have all of the following
- Standard library
    - Math stuff
        - NDArray implementation
    - Collections
        - Array
        - HashMap
        - BTreeMap
        - LinkedList
        - Deque
    - Sync
    - Parsers
        - Combinators
        - Generators
        - PEG
        - Languages
            - CSV
            - JSON
            - YAML
            - XML
    - File System
        - Path
        - exists(path)
    - Graphics
    - Science
- Docs generator
- Package manager
- Testing framework

## Command-Line Interface

- `fast FILENAME` Compile and run `FILENAME` and any implicit dependencies

## Tasks

- Write grammar
- Write parser
    - Strings
    - Integers
    - Variants
    - Objects
- Make GC example code
    - Mark variable not on stack when it is popped

## Done

## Grammar

The grammar is roughly C-like.

To be clear, this grammar may or may not match the current implementation of the parser.

```
statement = use | let | assign | fn_statement | expr
use = "use" simple_string
simple_string = '"' simple_string_item* '"'
simple_string_item = !'"' | '""' | '\"' | '\\'

string = '"' string_item* '"'
string_item = !('"' | '{' | '}') | '""' | '\"' | '\\' | '{{' | '}}' | '\{' | '\}' | escape_sequence | splice
escape_sequence = '\n' | '\b' | '\r' | '\t' | ansi_escape_sequence
splice = '{' expr '}'

inner = integer | struct | variant | string
pow = inner ('**' pow)?
mul = pow ('*' pow)*
add = mul ('+' mul)*
expr = add
```
