#include "game_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern FILE* yyin;
void addStats(stats* originals, stats modifier){
    originals->energy+=modifier.energy;
    originals->affection +=modifier.affection;
    originals->agility+=modifier.agility;
    originals->intellect+=modifier.intellect;
}
void initializeStats(stats* stat,int affection,int energy,int intellect,int agility){
    stat->affection=affection;
    stat->energy=energy;
    stat->intellect=intellect;
    stat->agility=agility;
}
int canDo(cost action){
    if(action.energy>metaInfo->globalsi[0])
        return 1;
    if(action.time>metaInfo->globalsi[1])
        return 2;
    return 0;
}
void initializeCost(cost* c,int energy,int time){
    c->time=time;
    c->energy=energy;
}
int maxStat(){
    stats s=metaInfo->characters[1].stats;
    if(s.agility==s.affection&&s.affection==s.intellect)
        return 0;
    if(s.agility>=s.affection&&s.agility>=s.intellect)
        return 1;
    if(s.affection>=s.agility&&s.affection>=s.intellect)
        return 2;
    if(s.intellect>=s.agility&&s.intellect>=s.affection)
        return 3;
    return 4;
}
/*
    Sends the info to the other compiler in the format:
    currPlace, time, energy, agility, affection, intellect, aftermath len+message
*/
void transmitInfo(int place,int consumedTime, stats gained,char* aftermathMessage){
    int len=strlen(aftermathMessage);
    int values[7]={ place,consumedTime, gained.energy, gained.agility, gained.affection, gained.intellect,len};
    //printf("len %d msg:%s\n",len,aftermathMessage);
    //for(int i=0;i<7;i++)
    //    printf("%d ",values[i]);
    //printf("\n\n");
    fwrite(values,sizeof(int),7,infoFile);
    if(values[6]>0)
        fwrite(aftermathMessage,sizeof(char),values[6],infoFile);
    //printf("\n%s\n",aftermathMessage);
    fseek(infoFile,0,SEEK_SET);

}
void receiveInfo(){
    int values[7];
    fread(values,sizeof(int),7,infoFile);
    metaInfo->globalsi[2]=values[0];
    metaInfo->globalsi[1]-=values[1];
    metaInfo->globalsi[0]+=values[2];
    stats modifiers;
    modifiers.energy=values[2];
    modifiers.agility=values[3];
    modifiers.affection=values[4];
    modifiers.intellect=values[5];
    addStats(&(metaInfo->characters[1].stats),modifiers);
    char message[10000];
    
   // for(int i=0;i<7;i++)
   //     printf("%d ",values[i]);
   //printf("Stats: %d %d %d\n",metaInfo->characters[1].stats.agility,metaInfo->characters[1].stats.affection,metaInfo->characters[1].stats.intellect);
    if(values[6]>0)
        {
        fread(message,sizeof(char),values[6],infoFile);
        message[values[6]]='\0';
        printf("\n%s\n",message);
        }
    fseek(infoFile,0,SEEK_SET);
}
void copyCost(cost* c,cost c2){
    c->time=c2.time;
    c->energy=c2.energy;
}
void initializeAction(action* a,char* name, int energy,int time,int affection,int agility,int intelligence){
    strcpy(a->name,name);
    initializeCost(&(a->energyCost),-energy,time);
    initializeStats(&(a->impact),affection,energy,intelligence,agility);
}
void addAction(int index,char* name, int energy,int time,int agility, int affection,int intelligence ){
    metaInfo->places[index].actionCount++;
    initializeAction(&(metaInfo->places[index].possibilities[metaInfo->places[index].actionCount-1]),name,energy,time,affection,agility,intelligence);
}
void initCharacter(int index, char* name,int affection,int energy,int intellect,int agility){
    strcpy(metaInfo->characters[index].name,name);
    initializeStats(&(metaInfo->characters[index].stats),energy,affection,intellect,agility);
}
void initPlace(int index,char name[20]){
    strcpy(metaInfo->places[index].name,name);
}
void initPlaces(){
    for(int i=0;i<MAX_PLACES;i++){
           metaInfo->places[i].actionCount=0;
        }
    //action energy, time, agility, affection, intelligence
    initPlace(0,"home");
    addAction(0,"rest",10,30,0,0,0);
    addAction(0,"use_the_internet",-10,30,0,0,40);
    addAction(0,"cook",-30,100,0,80,0);
    
    initPlace(1,"to_the_park");
    addAction(1,"sit_on_a_bench",10,30,0,0,0);
    addAction(1,"walk_together",-20,70,30,20,0);
    addAction(1,"run",-40,30,40,10,0);

    initPlace(2,"to_the_cinema");
    addAction(2,"watch_a_movie",10,120,0,60,30);
    addAction(2,"browse",-10,30,0,30,10);
    addAction(2,"sneak_into_a_movie",-20,10,20,10,0);

    initPlace(3,"to_the_library");
    addAction(3,"meditate",10,30,0,0,0);
    addAction(3,"browse",-10,30,0,30,10);
    addAction(3,"read",-10,60,0,10,50);
    addAction(3,"steal",-10,10,20,0,10);

    initPlace(4,"to_the_gym");
    addAction(4,"eat_protein",10,10,0,0,0);
    addAction(4,"lift",-40,20,50,0,0);
    addAction(4,"run",-30,20,20,20,0);

    initPlace(5,"to_the_shop");
    addAction(5,"get_food",10,0,0,0,0);
    addAction(5,"browse",-10,30,0,40,0);
    addAction(5,"steal",-10,10,20,0,10);

    initPlace(6,"to_the_tutorial");
    addAction(6,"begin",-10,30,0,0,0);

    initPlace(7,"??????");
    
}
void initDistances(){
    for(int i=0;i<MAX_PLACES;i++)
        {initializeCost(&(metaInfo->movingCost[i][i]),999999,999999);
        initializeCost(&(metaInfo->movingCost[7][i]),999999,999999);
        initializeCost(&(metaInfo->movingCost[i][7]),999999,999999);
        }
    initializeCost(&(metaInfo->movingCost[0][1]),20,15);
    initializeCost(&(metaInfo->movingCost[0][2]),10,5);
    initializeCost(&(metaInfo->movingCost[0][3]),15,15);
    initializeCost(&(metaInfo->movingCost[0][4]),5,10);
    initializeCost(&(metaInfo->movingCost[0][5]),5,5);
    initializeCost(&(metaInfo->movingCost[0][6]),999999,999999);

    initializeCost(&(metaInfo->movingCost[1][2]),30,25);
    initializeCost(&(metaInfo->movingCost[1][3]),5,5);
    initializeCost(&(metaInfo->movingCost[1][4]),10,15);
    initializeCost(&(metaInfo->movingCost[1][5]),20,20);
    initializeCost(&(metaInfo->movingCost[1][6]),999999,999999);

    initializeCost(&(metaInfo->movingCost[2][3]),15,20);
    initializeCost(&(metaInfo->movingCost[2][4]),10,10);
    initializeCost(&(metaInfo->movingCost[2][5]),5,5);
    initializeCost(&(metaInfo->movingCost[2][6]),999999,999999);

    initializeCost(&(metaInfo->movingCost[3][4]),10,15);
    initializeCost(&(metaInfo->movingCost[3][5]),10,15);
    initializeCost(&(metaInfo->movingCost[3][6]),999999,999999);

    initializeCost(&(metaInfo->movingCost[4][5]),5,5);
    initializeCost(&(metaInfo->movingCost[4][6]),999999,999999);
    
    initializeCost(&(metaInfo->movingCost[5][6]),999999,999999);

    

    for(int i=0;i<MAX_PLACES;i++){
        for(int j=i+1;j<MAX_PLACES;j++)
            copyCost(&(metaInfo->movingCost[j][i]),metaInfo->movingCost[i][j]);
    }
    initializeCost(&(metaInfo->movingCost[6][0]),10,30);
    /*for(int i=0;i<MAX_PLACES;i++){
        for(int j=0;j<MAX_PLACES;j++)
            printf("(%d,%d) ",metaInfo->movingCost[j][i].energy,metaInfo->movingCost[j][i].time);
            printf("\n");
    }*/
    
}
void initNewGameState(){
    metaInfo->shouldPrint=1;
    initNames();
    printf("1\n");
    initCharacter(0,"???\0",0,0,0,0);
    initCharacter(1,"Me\0",10,10,10,10);
    initCharacter(2,"Crush\0",20,10,50,20);
    initCharacter(3,"Narrator\0",30,100,100,50);
    initCharacter(4,"\0",0,0,0,0);
    printf("2asdsa\n");
    initPlaces();
    printf("3asdsa\n");
    strcpy(metaInfo->globals[0],"New game"); //character name
    strcpy(metaInfo->globals[1],"./Dialogues/intro.txt"); //currentDialogueFile
    metaInfo->globalsi[0]=10; //energy
    metaInfo->globalsi[1]=30; //time
    metaInfo->globalsi[2]=6; //currPlace
    metaInfo->globalsi[3]=-1; //current day;
    initDistances();
    printf("4asdsa\n");
}
void initNames(){
    metaInfo->placesNames[0] = "home";
    metaInfo->placesNames[1]="park";
    metaInfo->placesNames[2]= "cinema";
    metaInfo->placesNames[3]="library";
    metaInfo->placesNames[4]="gym";
    metaInfo->placesNames[5]="shop" ;
    metaInfo->placesNames[6]="tutorial" ;

    metaInfo->optionsNames[0] = "options";
    metaInfo->optionsNames[1] = "places";
    metaInfo->optionsNames[2] = "activities";
    metaInfo->optionsNames[3] = "topics" ;
    metaInfo->optionsNames[4] = "items" ;

    metaInfo->topicsNames[0] =  "family";
    metaInfo->topicsNames[1] = "hobbies";
    metaInfo->topicsNames[2] ="trends";
    metaInfo->topicsNames[3] ="school";
    metaInfo->topicsNames[4] ="health";
    metaInfo->topicsNames[5] ="food" ;
    metaInfo->topicsNames[6]= "game";
}
void initMetaInfo(){
    initNames();
    initPlaces();
    initDistances();
}
void startNewGame(){
    metaInfo=(global_variables*)malloc(sizeof(global_variables));
    initNewGameState();
}
/*
    What should be in a save file:

    - the global variables(both int and char)
    - the main character stats (and name)
*/
void saveGame(){
    printf("Saving. Do not close...");
    FILE* save=fopen("./Saves/save1.in","wb");
    if(save==NULL)
        {
            printf("Error: Could not save progress\n");
            return;
        }
    fwrite(&(metaInfo->globalsi),sizeof(int),MAX_GLOBALS_INT,save);
    for(int i=0;i<MAX_GLOBALS;i++){
        int len=strlen(metaInfo->globals[i]);
        fwrite(metaInfo->globals[i],sizeof(char),len,save);
    }
    fwrite(&(metaInfo->characters[1].stats.energy),sizeof(int),1,save);    
    fwrite(&(metaInfo->characters[1].stats.affection),sizeof(int),1,save);
    fwrite(&(metaInfo->characters[1].stats.intellect),sizeof(int),1,save);
    fwrite(&(metaInfo->characters[1].stats.agility),sizeof(int),1,save);
   // metaInfo->currDialogue.
    fclose(save);
    printf("Game saved!\n");
}
void obtainMetaInfo(FILE* save){
    fread(metaInfo->globalsi,sizeof(int),MAX_GLOBALS_INT,save);
    for(int i=0;i<MAX_GLOBALS;i++){
        int len=0;
        fread(&len,sizeof(int),1,save);
        fread(metaInfo->globals[i],sizeof(char),len,save);
    }
}
void readSaveFile(FILE* fd){
    obtainMetaInfo(fd);
    fread(&(metaInfo->characters[1].stats.energy),sizeof(int),1,fd);    
    fread(&(metaInfo->characters[1].stats.affection),sizeof(int),1,fd);
    fread(&(metaInfo->characters[1].stats.intellect),sizeof(int),1,fd);
    fread(&(metaInfo->characters[1].stats.agility),sizeof(int),1,fd);
    fclose(fd);
}
FILE* startDay(){
    system("clear");
    if(metaInfo->globalsi[3]>=0){
        metaInfo->globalsi[0]=100; //energy
        metaInfo->globalsi[1]=480; //time
        metaInfo->globalsi[2]=0;
    }
    metaInfo->globalsi[3]++; //current day;
    if(metaInfo->globalsi[3]==4){
        metaInfo->globalsi[0]=1; 
        metaInfo->globalsi[1]=1;
        metaInfo->globalsi[2]=7;
        int max=maxStat();
        switch(max){
            case 0:{ initializeCost(&(metaInfo->movingCost[0][7]),0,0); initializeCost(&(metaInfo->movingCost[7][0]),0,0); break;}
            case 1:{ addAction(7,"beat up",-1,1,0,0,0); break;}
            case 2:{ addAction(7,"kiss crush",-1,1,0,0,0); break;}
            case 3:{ addAction(7,"create a diversion",-1,1,0,0,0); break;}
        }
    }
    printf("You wake up, ready for a new day!\n");
    char file[255];
    sprintf(file,"./Dialogues/day_%d.txt",metaInfo->globalsi[3]);
    FILE* f=fopen(file,"r");
    if(f==NULL)
        printf("The END. Thank you for playing!\n");
    else 
        printf("================ Day %d ================ \n\n",metaInfo->globalsi[3]);
    char c='s';
    do{
        c=getchar();
        if(c=='s'){
                saveGame();
                getchar();
            }
    }while(c=='s');
    return f;
}
void goThroughSaves(){
    char path[101];
    for(int i=0;i<4;i++){
        sprintf(path,"./Saves/save%d.in",i+1);
        FILE* f=fopen(path,"r+");
        if(f!=NULL){
            obtainMetaInfo(f);
            printf("(%d) %s day %d, %s\n",i+1,metaInfo->globals[0],metaInfo->globalsi[3],metaInfo->placesNames[metaInfo->globalsi[3]]);
            fclose(f);
        }
        else printf("(%d) New game\n",i+1);
    }
}
FILE* selectSave(int id){
    char path[101];
    sprintf(path,"./Saves/save%d.in",id+1);
        FILE* f=fopen(path,"r+");
        if(f!=NULL){
            readSaveFile(f);
            fclose(f);
        }
        else startNewGame();
    return NULL;
}
void lowerString(char* src){
    int n=strlen(src);
    for(int i=0;i<n;i++){
        if(src[i]>='A'&&src[i]<='Z')
            src[i]=src[i]+('a'-'A');
    }
}
void drawArt(char *src){
    char file[255];
    sprintf(file,"./Sprites/%s.txt",src);
    FILE* save=fopen(file,"r");
    if(save==NULL)
        printf("couldn't open sprite %s\n",file);
    char str[1024];
    while(fgets(str,1024,save)){
        printf("%s",str);
    }
    printf("\n");
    fclose(save);
}
int identifyWord(char* word,const char** wordPool,int maxWords){
    for(int i=0;i<maxWords;i++){
        if(strcmp(wordPool[i],word)==0){
            return i;
        }
    }
    return -1;
}
void printOptions(int option){
    char message[32768];
    
    switch(option)
        {
        case OPTIONS:
            strcpy(message,"Here's everything you can do: \n \
            a) Actions you can choose:\n\
            1: go (to the )place_name - consume a certain amount of energy and time to go to the place you write. You also gain 5 agility per place\n\
            2: let's activity - consume a certain amount of energy and time to do the specific activity available at the current place\n\
            3: discuss topic_name: starts a conversation on the topic specified (always costs 30 minutes). Sometimes, discussing the right thing at the right place might unlock special things\n\
            b) Questions that you can ask (always free): \n\
            1: what places - shows the places where you can go and the costs to get there\n\
            2: what activities - shows what you can do\n\
            3: what topics - shows a list of things you can discuss\n");
            break;
        case PLACES:
            strcpy(message,"Here's the places you can visit, along with their costs:\n");
            
            for(int i=0;i<MAX_PLACES;i++){
                cost c=metaInfo->movingCost[metaInfo->globalsi[2]][i];
                if(canDo(c)==0)
                    {
                        char tmp[255];
                        sprintf(tmp,"%s energy:%d time:%d\n",metaInfo->placesNames[i],c.energy,c.time);
                        strcat(message,tmp);
                    }
                }
            break;
        case ACTIVITIES:
            strcpy(message,"Here's what you can do right now:\n");
            place location=metaInfo->places[metaInfo->globalsi[2]];
            for(int i=0;i<location.actionCount;i++)
                {
                    char act[255];
                    action action=location.possibilities[i];
                    sprintf(act,"%s - energy: %d, agility:%d, affection:%d, intellect:%d . Cost %d minutes\n", action.name,
                    action.impact.energy, action.impact.agility,action.impact.affection,action.impact.intellect,
                    action.energyCost.time);
                    strcat(message,act);
                }
            break;
        case TOPICS:
            strcpy(message,"Here's a list of things you can discuss(always costs 30 minutes)\n");
            if(metaInfo->globalsi[2]<MAX_PLACES-2){
                for(int i=0;i<MAX_PLACES-2;i++){
                    char tmp[255];
                    sprintf(tmp,"%s \n",metaInfo->topicsNames[i]);
                    strcat(message,tmp);}
            }else{
                if(metaInfo->globalsi[2]==6){
                    char tmp[255];
                sprintf(tmp,"%s \n",metaInfo->topicsNames[6]);
                strcat(message,tmp);
                }
            }
            break;
    }
    stats nullstats;
    nullstats.energy=0;
    nullstats.affection=0;
    nullstats.agility=0;
    nullstats.intellect=0;
    strcat(message,"\0");
    transmitInfo(metaInfo->globalsi[2],0, nullstats,message);
}
void discussTopic(char* topic){
    char message[255];
    stats altered;
    int time;
    time=0;
    altered.energy=0;
    altered.affection=0;
    altered.agility=0;
    altered.intellect=0;
    if(metaInfo->globalsi[1]<30)
        {
            strcpy(message,"You don't have enough energy to discuss anything right now\n");
        }
    else{
        time=30;
        int id=identifyWord(topic,metaInfo->topicsNames,MAX_PLACES);
        sprintf(message,"You tried to talk about %s. ",metaInfo->topicsNames[id]);
        if(id==metaInfo->globalsi[2])
            {
                strcat(message,"It seemed to go quite well\n");
                altered.affection=30;
            }
        else
            strcat(message,"But it doesn't seem like a meaningful conversation\n");
    }
    transmitInfo(metaInfo->globalsi[2],time,altered,message);
}
void executeAction(char* actionName){
    int u=isActionAvailable(actionName);
    stats altered;
    int time;
    time=0;
    altered.energy=0;
    altered.affection=0;
    altered.agility=0;
    altered.intellect=0;
    char message[255];
    if(u<0)
        {
            sprintf(message,"You can't %s here\n",actionName);
        }
    else{
        action act=metaInfo->places[metaInfo->globalsi[2]].possibilities[u];
    if(canDo(act.energyCost)==0){
        sprintf(message,"You %s\n",act.name);
        altered=act.impact;
        time=act.energyCost.time;
    }else{
        sprintf(message,"You don't have enough time or energy to %s\n",act.name);
    }
    }
    transmitInfo(metaInfo->globalsi[2],time,altered,message);
}
void goToPlace(char* placeName){
    int id=identifyWord(placeName,metaInfo->placesNames,MAX_PLACES);
    if(id<0)
        return;
    char message[255];
    stats altered;
    int time;
    place wantedPlace=metaInfo->places[id];
    time=0;
    altered.energy=0;
    altered.affection=0;
    altered.agility=0;
    altered.intellect=0;
    if(canDo(metaInfo->movingCost[metaInfo->globalsi[2]][id])!=0){
        //printf("%d %d\n",metaInfo->movingCost[metaInfo->globalsi[2]][id].energy,metaInfo->movingCost[metaInfo->globalsi[2]][id].time);
            sprintf(message,"You don't have enough time or energy to go %s\n",wantedPlace.name);
        }
    else{
        altered.energy=-metaInfo->movingCost[metaInfo->globalsi[2]][id].energy;
        altered.agility=5;
        time=metaInfo->movingCost[metaInfo->globalsi[2]][id].time;
        sprintf(message,"You decide to go %s\n",wantedPlace.name);
    }
    transmitInfo(id,time,altered,message);
}
int isActionAvailable(char* action){
    place location=metaInfo->places[metaInfo->globalsi[2]];
    for(int i=0;i<location.actionCount;i++){
        if(strcmp(action,location.possibilities[i].name)==0)
        return i;
    }
    return -1;
}