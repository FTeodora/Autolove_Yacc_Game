#!/bin/bash 
lex commands.l; yacc -d commands.y; gcc lex.yy.c y.tab.c ../game_functions.c
