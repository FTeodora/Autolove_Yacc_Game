%{
    #include <stdio.h>
    #include "y.tab.h"
    #include <string.h>
    #include <stdlib.h>
    char* val;
%}
%option noyywrap
%Start PLAYING INPUT SKIP
%%
^Car                                                      { BEGIN PLAYING; return SPEAKER;}
_[0-9]+_                                                  { val=yytext+1; val[strlen(val)-1]='\0'; yylval.nr=atoi(val); return INDEX;}
<PLAYING>\%|:|<|>                                         { return yytext[0];}
<PLAYING>getVar                                           { return VAR_VALUE;}
<PLAYING>drawSprite                                       { return EMOTION;}
<PLAYING>"~~"                                             { BEGIN INPUT; return MARK_INPUT;}
<INPUT>"~~"                                               { BEGIN PLAYING; return MARK_INPUT;}
<INPUT>"input"                                            { return TEXT_INPUT;}
<INPUT>setVar                                             { return VAR_SET;}
<INPUT>setVari                                            { return VARI_SET;}
<PLAYING>maxStat                                          { return MAX_STAT;}
[\n\r]                                                    ;
.                                                         { yylval.s=strdup(yytext); return TEXT;}
%%