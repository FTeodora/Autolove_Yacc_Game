%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "y.tab.h"
    #include "../game_functions.h"
    
    stats nullstats;
    extern int yylex();
    extern FILE* yyin;
    extern FILE* infoFile;
    void yyerror(char* s);
    int wasQuestion;
    int main(int argc,char** argv);
    extern int yychar;
    extern void initMetaInfo();
    extern int canDo(cost action,cost currEnergy);

    extern global_variables *metaInfo;
    extern void printOptions(int option);
    extern character characters[4];
    extern dialogue currDialogue;
    extern int identifyWord(char* word,const char** wordPool,int maxWords);
    extern int isActionAvailable(char* action);
    extern void goToPlace(char* placeName);
    extern void executeAction(char* action);
    extern void discussTopic(char* topic);
    extern void transmitInfo(int place,int consumedTime, stats gained,char* aftermathMessage);

    void lowerString(char* src);
    typedef struct yy_buffer_state * YY_BUFFER_STATE;
    extern YY_BUFFER_STATE yy_scan_string(char * str);
    extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
%}
%token<s> TEXT OPTION  PLACE TOPIC
%token ASK CHECK RELOCATE DISCUSS DO GAME
%type<s> randomText
%union{
    char* s;
    char* value;
    int nr;
}
%%
command:
        | action
        | question {wasQuestion=0;};
action: RELOCATE PLACE
    {
        wasQuestion=1;
        goToPlace($2);}       
    | DISCUSS TOPIC
    {
        wasQuestion=2;
        discussTopic($2);}
    | DISCUSS GAME
    {
        wasQuestion=2;
        discussTopic("game");}
    | DO randomText
    {
        wasQuestion=3;
        executeAction($2);   
    };
question: ASK OPTION { int u=identifyWord($2,metaInfo->optionsNames,MAX_OPTIONS); printOptions(u); }
        | CHECK TEXT{ };
randomText: TEXT{ strcpy($$,$1); }
        | randomText TEXT{ strcat($1,$2);};
%%

int main(int argc,char** argv){
    
    if(argc<2){
        printf("Please input a string to parse\n");
        return -1;
    }
    char path[255];
    if(argc>=3){
        infoFile=fopen(argv[2],"r+");
    }
    else
        infoFile=fopen("metadata.bin","r+");
    if(infoFile==NULL){
        printf("Error: could not open metadata from other file:  \n");
        return -1;
    }
    else{
        metaInfo=(global_variables*)malloc(sizeof(global_variables));
        initMetaInfo();
        int ty=-1;
        fread(metaInfo->globalsi,sizeof(int),MAX_GLOBALS_INT,infoFile);
        fseek(infoFile,0,SEEK_SET);
    }
    
    nullstats.energy=0;
    nullstats.affection=0;
    nullstats.agility=0;
    nullstats.intellect=0;
    transmitInfo(metaInfo->globalsi[2],0, nullstats,"");
    lowerString(argv[1]);
    YY_BUFFER_STATE buffer = yy_scan_string(argv[1]);
    yyparse();
    yy_delete_buffer(buffer);
    fclose(infoFile);
    //printf("wasQ %d\n",wasQuestion);
    return wasQuestion;
}
void yyerror(char* s){
    transmitInfo(metaInfo->globalsi[2],0, nullstats,"I'm afraid this isn't an option\n");
    wasQuestion=-1;
}
