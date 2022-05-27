#include <stdlib.h>
#include <stdio.h>

#define MAX_OPTIONS 4
#define MAX_ACTIONS 6
#define INVENTORY_CAPACITY 6
#define MAX_PLACES 8
#define MAX_CHARACTERS 4
#define MAX_ENERGY 100
#define MAX_GLOBALS 2
#define MAX_GLOBALS_INT 4
static enum options{ OPTIONS, PLACES, ACTIVITIES, TOPICS} option;
typedef struct _cost{
        int time;
        int energy;
}cost;
//energy: hp
//agility: attack
//affection: resistance
//intellect: speed
typedef struct _stats{
    int affection , energy, intellect, agility;
}stats;

typedef struct _character{
    char name[20];
    stats stats;
 }character;
typedef  struct _action{
    char name[32];
    cost energyCost;
    stats impact;
} action;
typedef struct _place{
    char name[20];
    action possibilities[MAX_ACTIONS];
    int actionCount;
}place;
typedef struct _dialogue{
    char text[255];
    char speaker[20];
}dialogue;
typedef struct _topic{
    char name[10];
    int rightPlace;
}topic;
typedef struct _shared{
    const char* placesNames[MAX_PLACES];
    const char* optionsNames[MAX_OPTIONS];
    const char* topicsNames[MAX_PLACES];
    cost movingCost[MAX_PLACES][MAX_PLACES];
    char globals[MAX_GLOBALS][255];
    /*
    0 - protag energy
    1 - time for the day
    2 - current place
    3 - current day
    */
    int globalsi[MAX_GLOBALS_INT];
    place places[MAX_PLACES];
    character characters[MAX_CHARACTERS];
    
    long int returnFromFileAddress;
    int shouldPrint;
}global_variables;
global_variables *metaInfo;
char currFile[255];
dialogue currDialogue;
stats buffer;
FILE* infoFile;
unsigned int generateIndex(const char* word,int maxIndex);
enum options toOptions(char* src);
void addStats(stats* originals, stats modifier);
FILE* startDay();
void startNewGame();
void readSaveFile(FILE* fd);
void saveGame();
void lowerString(char* src);
void initNames();
void initPlaces();
void initDistances();
void goThroughSaves();
void drawArt(char *src);
int identifyWord(char* word,const char** wordPool,int maxWords);
int isActionAvailable(char* action);
void printOptions(int option);
void initMetaInfo();
void transmitInfo(int place,int consumedTime, stats gained,char* aftermathMessage);