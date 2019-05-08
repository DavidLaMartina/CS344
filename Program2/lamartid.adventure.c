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
#define TIME_FILE_PATH      "currentTime.txt"
#define ROOM_ENTRY_FIRST    "ROOM NAME"
#define CONN_ENTRY_FIRST    "CONNECTION"
#define TYPE_ENTRY_FIRST    "ROOM TYPE"

enum boolean { FALSE, TRUE };
enum room_type { START_ROOM, MID_ROOM, END_ROOM };

// Use room struct from buildrooms to give structure to room data
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
    struct dirent* roomFile;    // Pointer to specific room file
    DIR* dirToCheck;            // Directory to check for room files
    int roomCount = 0;          // Keep track of number of rooms checked
    FILE* fp;                   // File pointer for open, close, error check
    char buf[100];              // buffer for entries
    char roomPath[100];         // buffer for room file paths

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
    roomCount = 0;          // Reset room counter so correct room referenced
    struct room* curRoom;   // Pointer to room whose connections being made
    struct room* conRoom;   // Pointer to room to be connected to current
    char* entryStr;         // Pointer to current line / string read
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

// Thread safety globals
pthread_t timeThread;                       // Second thread for writing to date time file
pthread_mutex_t timeLock;                   // Lock for thread-safe write to date time file
enum boolean InPlay;                        // Flag for time-keeping thread

// Write time in second thread
// Assistance: https://www.tutorialspoint.com/c_standard_library/c_function_strftime.htm
void* WriteTime(){
    // Return immediately if flag is false
    if (InPlay == FALSE){
        return NULL;
    }

    pthread_mutex_lock(&timeLock);  // Attempt lock - execution upon unlock from main

    // Attempt file open for writing - exit if fail
    int timeFile = open(TIME_FILE_PATH, O_WRONLY | O_CREAT, 0660);
    if (timeFile < 0){
        printf("Could not create and open %s\n", TIME_FILE_PATH);
        exit(1);
    }
    // Get and format time
    char timeBuf[100];                      // Buffer for time string
    memset(timeBuf, '\0', sizeof(timeBuf)); // Ensure string will end in null terminator
    time_t curTime;                         // Struct for getting current time
    struct tm* timeComponents;              // Struct for separating time into parts
    time(&curTime);                         // Put current time into storage
    timeComponents = localtime(&curTime);   // Separate time into printable parts
    strftime(timeBuf, 100, "%I:%M%P, %A, %B %d, %Y", timeComponents);   // Save in buf

    // Write formatted time to file
    int bytesToWrite = strlen(timeBuf) * sizeof(char);      // Space requirement
    int written = write(timeFile, timeBuf, bytesToWrite);   // Attempt write to file
    if (written != bytesToWrite){
        printf("Could not complete writing time to %s\n", TIME_FILE_PATH);
        exit(1);
    }
    close(timeFile);

    pthread_mutex_unlock(&timeLock);    // Unlock to allow main thread to regain control
    return NULL;
}

// Display time as read from currentTime.txt - file created by WriteTime
void ReadTime(){
    char buf[100];  // Create buffer to read in time

    FILE* timeFile = fopen(TIME_FILE_PATH, "r");    // open for read
    if (timeFile == NULL){
        printf("Time file could not be opened\n");
        exit(1);
    }
    fgets(buf, sizeof(buf), timeFile);  // Just one line in file to print
    printf("\t%s\n\n", buf); 
    fclose(timeFile);
}

// Lock / unlock methods to reduce code in main
void LockMutex(){
    if (pthread_mutex_lock(&timeLock) != 0){
        printf("Lock not created...exiting\n");
        exit(1);
    }
}
void CreateTimeThread(){
    if (pthread_create(&timeThread, NULL, &WriteTime, NULL) != 0){
        printf("Time thread could not be created...exiting\n");
        exit(1);
    }
}

// Driver
int main(int argc, char* argv[]){
    // Game Setup
    char* roomsDir = MostRecentRooms();         // Get name of most recent rooms dir
    struct room* rooms[REQUIRED_ROOMS];         // Array of rooms to be used in game
    ReadRooms(rooms, REQUIRED_ROOMS, roomsDir); // Read in rooms from files and create structs

    LockMutex();        // Lock main thread until user enters "time"
    CreateTimeThread(); // Spawn time-writing thread
    InPlay = TRUE;
    
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

            // If entry "time", write to time file in separate thread, then read in main
            if (strcmp(entryStr, "time") == 0){
                pthread_mutex_unlock(&timeLock);    // Unlock secondary thread for writing
                pthread_join(timeThread, NULL);     // Barrier - Ensure write finishes
                ReadTime();

                LockMutex();        // Re-lock until next "time" entry
                CreateTimeThread(); // Create new thread for next "time" entry

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

    // Free malloc / calloc memory
    free(roomsDir);
    FreeRooms(rooms, REQUIRED_ROOMS);
     
    // Finish / free thread memory
    InPlay = FALSE;                     // Avoid unwanted write to time file
    pthread_mutex_unlock(&timeLock);    // Unlock lock now that flag has been set
    pthread_join(timeThread, NULL);
    pthread_exit(&timeThread);
    pthread_mutex_destroy(&timeLock);

    
    return 0;
}
