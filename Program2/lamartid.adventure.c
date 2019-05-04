#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>

#define REQUIRED_ROOMS      7
#define MAX_CONNECTIONS     6
#define MAX_ROOM_DIGITS     1
#define DIR_NAME_FIRST      "lamartid.rooms."
#define ROOM_ENTRY_FIRST    "ROOM NAME"
#define CONN_ENTRY_FIRST    "CONNECTION"
#define TYPE_ENTRY_FIRST    "ROOM TYPE"

// Reuse code from lamartid.buildrooms to facilitate game
enum boolean { FALSE, TRUE };
enum room_type { START_ROOM, MID_ROOM, END_ROOM };

struct room{
    int id;
    char* name;
    int numConnections;
    struct room* connections[MAX_CONNECTIONS];
    enum room_type type;
};

// Add B to A's connections, increment A's numConnections
void ConnectRoom(struct room* roomA, struct room* roomB){
    roomA->connections[roomA->numConnections] = roomB;
    (roomA->numConnections)++;
}

// Return string representing most recently created rooms directory
char* MostRecentRooms(){
    int newestDirTime = -1;                             // timestamp of newest subdir
    char targetDirPrefix[100] = DIR_NAME_FIRST;         // prefix to search for
    char* newestDirName = calloc(100, sizeof(char));    // holds name of newest candidate
    memset(newestDirName, '\0', sizeof(newestDirName));

    DIR* dirToCheck;                            // holds start directory
    struct dirent* fileInDir;                   // holds current subdir of start dir
    struct stat dirAttributes;                  // holds info about subdir

    dirToCheck = opendir(".");                  // open program's root dir

    if (dirToCheck > 0){                        // make sur current dir opened
        // check each entry in directory
        while ((fileInDir = readdir(dirToCheck)) != NULL){
            // Only process entries with the right prefix
            if (strstr(fileInDir->d_name, targetDirPrefix) != NULL){
                // Get attributes of entry
                stat(fileInDir->d_name, &dirAttributes);
                // If this entry's time is more recent
                if ((int)dirAttributes.st_mtime > newestDirTime){
                    newestDirTime = (int)dirAttributes.st_mtime;
                    memset(newestDirName, '\0', sizeof(newestDirName));
                    strcpy(newestDirName, fileInDir->d_name);
                }
            }
        }
    }
    closedir(dirToCheck);
    return newestDirName;
}

// Separates text of interest from roomFile line
char* RoomFileEntry(char* line){
    char* first  = strtok(line, ":");
    char* second = strtok(NULL, " ");
    char* third  = strtok(second, "\n");
    char* final  = calloc(strlen(third) + 1, sizeof(char));
    strcpy(final, third);
    return final;
}

// Find room by name within rooms array
struct room* FindRoom(struct room* rooms[], int numRooms, char* nameToFind){
    int i;
    for (i = 0; i < numRooms; i++){
        if (strcmp(rooms[i]->name, nameToFind) == 0){
            return rooms[i];
        }
    }
    return NULL;
}

// Determine room type base on string
enum room_type RoomType(char* typeStr){
    if (strcmp(typeStr, "START_ROOM") == 0)
        return START_ROOM;
    else if (strcmp(typeStr, "END_ROOM") == 0)
        return END_ROOM;
    else
        return MID_ROOM;
}

// Allocate memory and init variables for room struct
void InitRoom(struct room** newRoom, int id, char* name){
    *newRoom = (struct room*)malloc(sizeof(struct room));
    (*newRoom)->id = id;
    (*newRoom)->name = name;
    (*newRoom)->numConnections = 0;
}

// Populate rooms struct for easy game manipulation
void ReadRooms(struct room* rooms[], int numRooms, char* roomsDir){
    struct dirent* roomFile;
    DIR* dirToCheck;
    int roomCount = 0;
    FILE* fp;
    char buf[100];
    char roomPath[100];

    // First readthrough - get names of all rooms so connections can be made
    dirToCheck = opendir(roomsDir);
    if (dirToCheck > 0){
        while ((roomFile = readdir(dirToCheck)) != NULL){
            // Ignore hidden files (Assume only files are rooms, ., and ..)
            if (roomFile->d_name[0] != '.'){
                strcpy(roomPath, roomsDir);
                strcat(roomPath, "/");
                strcat(roomPath, roomFile->d_name);
                
                fp = fopen(roomPath, "r");
                if (fp != NULL){
                    while (fgets(buf, sizeof(buf), fp)){
                        if (strstr(buf, ROOM_ENTRY_FIRST)){
                            InitRoom(&rooms[roomCount], roomCount, RoomFileEntry(buf));
                        }
                    }
                    fclose(fp);
                }  
                roomCount++;
            }
        }
        closedir(dirToCheck);
    }
    // Second readthrough - make connections
    roomCount = 0;
    struct room* curRoom;
    struct room* conRoom;
    char* entryStr;
    dirToCheck = opendir(roomsDir);
    if (dirToCheck > 0){
        while ((roomFile = readdir(dirToCheck)) != NULL){
            if (roomFile->d_name[0] != '.'){
                strcpy(roomPath, roomsDir);
                strcat(roomPath, "/");
                strcat(roomPath, roomFile->d_name);

                fp = fopen(roomPath, "r");
                if (fp != NULL){
                    curRoom = NULL; // keep null until assigned to check null
                    while (fgets(buf, sizeof(buf), fp)){
                        if (strstr(buf, ROOM_ENTRY_FIRST)){
                            entryStr = RoomFileEntry(buf);
                            curRoom = FindRoom(rooms, numRooms, entryStr);
                            free(entryStr);
                        }
                        else if (strstr(buf, CONN_ENTRY_FIRST) && curRoom != NULL){
                            entryStr = RoomFileEntry(buf);
                            conRoom = FindRoom(rooms, numRooms, entryStr);
                            ConnectRoom(curRoom, conRoom); 
                            free(entryStr);
                        }
                        else if (strstr(buf, TYPE_ENTRY_FIRST) && curRoom != NULL){
                            entryStr = RoomFileEntry(buf);
                            curRoom->type = RoomType(entryStr);
                            free(entryStr);
                        }
                    }
                    fclose(fp);
                }
            }
        }
        closedir(dirToCheck);
    }
}

// Free rooms and allocated names
void FreeRooms(struct room* rooms[], int numRooms){
    int i;
    for (i = 0; i < numRooms; i++){
        free(rooms[i]->name);
        free(rooms[i]);
    }
}

// Diagnostic print function
void printRooms(struct room* rooms[], int numRooms){
    int i, j;
    for (i = 0; i < numRooms; i++){
        printf("Room %d: \n\t%d\n\t%s\n\t%d\n\t%d\n\t", i,
                rooms[i]->id, rooms[i]->name, rooms[i]->numConnections, rooms[i]->type);
        printf("Connections: ");
        for (j = 0; j < rooms[i]->numConnections; j++){
            printf("%s ", rooms[i]->connections[j]->name);
        }
        printf("\n");
    }
}

// Get starting position from rooms array
struct room* GetStart(struct room* rooms[], int numRooms){
    int i;
    for (i = 0; i < numRooms; i++){
        if (rooms[i]->type == START_ROOM){
            return rooms[i];
        }
    }
}

// Validate next room choice, return room pointer or null
struct room* NextRoom(struct room* curPos, char* nameToFind){
    int i;
    for (i = 0; i < curPos->numConnections; i++){
        if (strcmp(curPos->connections[i]->name, nameToFind) == 0){
            return curPos->connections[i];
        }
    }
    return NULL;
}

// Display game state
void PrintGameState(struct room* curPos){
    printf("CURRENT LOCATION: %s\n", curPos->name);
    printf("POSSIBLE CONNECTIONS: ");
    int i;
    for (i = 0; i < curPos->numConnections; i++){
        if (i < curPos->numConnections - 1){
            printf("%s, ", curPos->connections[i]->name);
        }else{
            printf("%s.\n", curPos->connections[i]->name);
        }
    }
}

// Bad room entry errors
void PrintRoomError(){
    printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
}
void PrintCurError(){
    printf("YOU'RE ALREADY THERE! TRY AGAIN.\n");
}
void PrintNoConnectionError(){
    printf("YOU CAN'T GET TO THAT ROOM FROM YOUR CURRENT POSITION!\n");
}

// Display ending message
void PrintEndMessage(struct room* path[], int pathLength){
    printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS\n");
    printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", pathLength);

    int i;
    for (i = 0; i < pathLength; i++){
        printf("%s\n", path[i]->name);
    }
}


// Prompt user for next room (or time)
// Return string allocated by gettime - must be freed
char* GetEntry(){
    printf("WHERE TO? >");

    char* lineEntered = NULL;
    size_t bufferSize = 0;
    int numChars = getline(&lineEntered, &bufferSize, stdin);
    lineEntered[numChars - 1] = '\0';       // Set null terminator to make string 'usable'

    // printf("\n");       // separator for readiability
    return lineEntered;
}

// Write & Display time
void Time(){


}


// Driver
int main(int argc, char* argv[]){
    // Thread setup

    // Game Setup
    char* roomsDir = MostRecentRooms();         // Get name of most recent rooms dir
    struct room* rooms[REQUIRED_ROOMS];         // array of rooms to be used in game
    ReadRooms(rooms, REQUIRED_ROOMS, roomsDir); // Read in rooms from files and create structs
    
    // Game
    struct room* curPos = GetStart(rooms, REQUIRED_ROOMS);  // Set start room
    struct room* path[1000];        // array of room pointers to keep track of path taken
    int nextPath = 0;               // counter for entering rooms into path
    struct room* nextRoom = NULL;   // point to next room user has chosen (null if invalid)
    char* entryStr;                 // user room entry (separate variable for deallocation)

    // Outer loop - keep getting next room until user arrives at end
    do{
        // Inner loop - keep printing state and requesting next room until valid entry
        do{
            PrintGameState(curPos);
            entryStr = GetEntry();
            printf("\n");       // separate for readability

            // Print time if user entered time
            if (strcmp(entryStr, "time") == 0){
                printf("TIME\n");

                nextRoom = NULL;
            }
            // Else process entry as room string
            else{
                nextRoom = NextRoom(curPos, entryStr);
                if (nextRoom == NULL){
                    if (strcmp(entryStr, curPos->name) == 0){
                        PrintCurError();
                    }else if (FindRoom(rooms, REQUIRED_ROOMS, entryStr) != NULL){
                        PrintNoConnectionError();
                    }else{
                        PrintRoomError();
                    }
                    printf("\n");       // separate for readability
                }else{
                    curPos = nextRoom;
                    path[nextPath] = nextRoom;
                    nextPath++;
                }
            }
            free(entryStr);     // mem allocated by getline - needs to be freed
        }while (nextRoom == NULL);
    }while (curPos->type != END_ROOM);

    // Once user has found end room, print message
    PrintEndMessage(path, nextPath);

    // Finish / free memory
    free(roomsDir);
    FreeRooms(rooms, REQUIRED_ROOMS);

    return 0;
}
