#!/bin/bash 
lex compiler.l; yacc -d compiler.y; gcc lex.yy.c y.tab.c game_functions.c
