# CUI Interpreter

This directory contains CUI (Console User Interface) code for CLL.  
It allows user to input code 'on fly' to see its result or it can be used to execute scripts from console or terminal.

## Files

- main

Contains `Interpreter` object which executes scripts.  
If no arguments are passed it will act as real time interpeter,
otherwise it will execute script called in first argument.

- console

Contains additional functions that help with console/terminal manipulation e.g. allows to change font color.
