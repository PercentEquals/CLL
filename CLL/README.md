# CLL Interpreter

This directory contains CLL Interpreter class that allows execution of CLL scripts.  
This directory should be built as static library.

## Files

- lexer

Contains function that interpretes line as tokens - or in this case as variables.

- static

Contains literals that are used by Interpreter class methods and lexer such as:  
list of barewords, restricted variable names, multiple character operators,  
symbols that are illegal in variable name and symbols.

- var

Contains `var` struct that acts as a dynamic variable.  
Every object of it contains information such as: name, value, type (as enum).

- functions and functions directory

Contains `function` struct that holds pointers to all basic functions,  
which are contained in functions directory.  
It allows for function execution by its name.

- defined

Contains `defined` struct that holds vector of lines to execute when function is called.  
It allows for creation of functions in CLL language.

- interpreter

Contains `Interpreter` class which executes a script.  
First it calls lexer to read tokens from executed line.  
Next it checks for line breaks and scopes.  
If line is broke it will first execute tokens before line break.  
Next it parses tokens to check for errors.  
Then it applies math to tokens, after which it checks for errors one more time.  
Finally it checks for barewords which tell interpreter what to do.

- utils directory

Contains usefull algorithms used by other translation units.
