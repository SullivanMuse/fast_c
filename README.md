# Toy Lang

In order to hone my C skills, I am going to make an interpreter for a toy language in C.

## Features

ToyLang is a simple imperative language, but it will have some advanced features.

- Let statements, for declaring and optionally defining variables
- Values
    - Integers `1234`
        - Arithmetic operators `+ - * /`
        - Bitwise operators `~ & | ^ << >>`
    - Immutable strings `"Hello, world!"`
        - Efficient formatting `"{name} is looking good today!"`
    - Structs `{x = 1, y = 2}`
        - Concatenation `{..xs, ..ys}`. `xs` and `ys` must have a disjoint set of keys.
    - Variants `:variant1`
    - Macro expansion and staging system
- Simple file-based module system
    - `use mod1` includes the code from `mod1.fast` under the `mod1` module
    - `use ..module` includes the code from `../module.fast`
    - `use mod1.mod2` includes the code from `./mod1/mod2.fast` as `mod2`

## Tasks

- Write grammar

## Done

## Grammar
