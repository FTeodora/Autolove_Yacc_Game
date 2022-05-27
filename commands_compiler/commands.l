%{
    #include <stdio.h>
    #include "y.tab.h"
    #include <string.h>
    #include <stdlib.h>
    char* val;
%}
%option noyywrap
%%
what                      			                    { return ASK;}
"options"|"places"|"activities"|"items"|"topics"        { yylval.s=strdup(yytext);return OPTION;}
check            				                        { return CHECK;}
"let's"                                                 { return DO; }
"go to the"|"go"                                        { return RELOCATE;}
discuss                                                 { return DISCUSS;}
"home"|"park"|"cinema"|"library"|"gym"|"shop"           { yylval.s=strdup(yytext); return PLACE;}
"family"|"hobbies"|"trends"|"school"|"health"|"food"    { yylval.s=strdup(yytext); return TOPIC;}
game                                                    { return GAME;}
[\t\n ]                                                 ;
.                                                       { yylval.s=strdup(yytext); return TEXT;}
%%