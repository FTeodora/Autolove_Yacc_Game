%{
    #include <sys/mman.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "y.tab.h"
    #include "game_functions.h"
    
    int branch=0;
    char branchFile[255];
    int shouldDelete=1;
    extern void saveGame();
    extern int yylex();
    extern FILE* yyin;
    extern FILE* infoFile;
    FILE* ff;
    void yyerror(char* s);
    int main(int argc,char** argv);
    extern int yychar;
    extern void startNewGame();
    extern FILE* startDay();
    extern stats buffer;
    extern int maxStat();

    extern global_variables *metaInfo;
    extern dialogue currDialogue;
    
    extern void askQuestion(char*  questionAsked);
    extern void drawArt(char *src);
    extern void receiveInfo();
    extern void initMetaInfo();
    extern void goThroughSaves();
%}
%token<value> EMOTION 
%token<s> TEXT
%token<nr> INDEX
%token SPEAKER VAR_VALUE TEXT_INPUT VAR_SET MARK_INPUT VARI_SET MAX_STAT
%type<s> spokenText dialogueText processVariable
%union{
    char* s;
    char* value;
    int nr;
}
%%
file: statement 
    | file statement
    ;
statement: speak | specialCommand |togglePrint
speak: spokesman '<' spokenText '>' { 
    if(metaInfo->shouldPrint!=0){
    if(shouldDelete==1)
        system("clear");
    printf("%s: %s\n",currDialogue.speaker,$3); 
    char c='s';
    do{
        c=getchar();
        if(c=='s'){
                saveGame();
                getchar();
            }
    }while(c=='s');
    shouldDelete=1;
    }
};
specialCommand: MARK_INPUT processInput MARK_INPUT;
processInput: VAR_SET INDEX {
    if(metaInfo->shouldPrint!=0)
        {
            printf("Write your input: ");
    do { 
        fgets(metaInfo->globals[$2],255,stdin); 
    } while(strlen(metaInfo->globals[$2])<3); 
    metaInfo->globals[$2][strlen(metaInfo->globals[$2])-1]='\0';
        }
    }                                                 
    | TEXT_INPUT {
        if(metaInfo->shouldPrint!=0){
        char buffer2[255];
        int res=0;
        //ok deci o problema. pentru ca o sa chem alt compilator care o sa fie un proces separat cu memoria lui separata,
        //n-o sa aiba datele de aici (inclusiv numele lucrurilor). Si pentru ca trebuie sa schimb uneori metadate, solutia
        //cea mai la indemana a fost sa le scriu printr-un fisier binar si dupa sa prelucrez datele aici in functie de ce primesc
        // de la celalalt compilator (pentru ca mmap face ciudat/nu merge pe WSL)
        printf("Tip: if you forgot what you can do, type 'what options' to get a list of all actions\n");
        buffer.energy=0;
        buffer.affection=0;
        buffer.intellect=0;
        buffer.agility=0;
        while(metaInfo->globalsi[0]>0 && metaInfo->globalsi[1]>0){ 
            infoFile=fopen("./commands_compiler/metadata.bin","w+");
            fwrite(metaInfo->globalsi,sizeof(int),MAX_GLOBALS_INT,infoFile);
            printf("%s , day %d. energy:%d, time:%d minutes\n",\
            metaInfo->placesNames[metaInfo->globalsi[2]], metaInfo->globalsi[3], metaInfo->globalsi[0],metaInfo->globalsi[1]);
            fclose(infoFile);
            printf("Write your input: ");
            FILE* currFile=yyin;
            fgets(buffer2,255,stdin);
            char command[320];
            sprintf(command,"./commands_compiler/a.out \"%s\" './commands_compiler/metadata.bin'",buffer2);
            res=system(command);
            infoFile=fopen("./commands_compiler/metadata.bin","r");
            receiveInfo();
            fclose(infoFile);
            remove("./commands_compiler/metadata.bin");
       }
       printf("You can't do anything anymore except for pressing a key to continue\n");
       getchar();
        }
       };
    | VAR_SET INDEX ':' dialogueText{
        strcpy(metaInfo->globals[$2],$4);
    }
    |VARI_SET INDEX ':' INDEX{
        metaInfo->globalsi[$2]=$4;
    }
spokesman: speaking
        | speaking sprite;
sprite: '%' EMOTION ':' spokenText '%'{
    system("clear");
    shouldDelete=0;
    if(metaInfo->shouldPrint)
        drawArt($4);
}
togglePrint: MAX_STAT{
    branch=1;
    sprintf(branchFile,"./Dialogues/day_%d_%d.txt",metaInfo->globalsi[3],maxStat());
    };
speaking: SPEAKER INDEX{ strcpy(currDialogue.speaker,metaInfo->characters[$2].name);}
spokenText: dialogueText { strcpy(currDialogue.text,$1); }
            |  spokenText dialogueText{ strcat($1,$2);};
dialogueText: TEXT
            | processVariable;
processVariable:'%' VAR_VALUE INDEX '%' { strcpy($$,metaInfo->globals[$3]); };
%%
int main(int argc,char** argv){


    //printf("Hello, and welcome to Autolove\n");
    //printf("This is just a little fun relatable game\n");
    //printf("Play the game however you want and don't forget to have fun\n");
    /*
    printf("Pick the number of the game you would like to continue\n");
    goThroughSaves();
    int option;
    while(scanf("%d",&option)==0);
        getchar();
    getchar();
    */
    startNewGame();
    yyin=startDay();
    while(yyin!=NULL){
        yyparse();
        if(branch==0)
            yyin=startDay();
        else
            {
                yyin=fopen(branchFile,"r");
                branch=0;
            }
        
    }
        
    return 0;
}
void yyerror(char* s){
    fprintf(stderr,"%s\n",s);
}
