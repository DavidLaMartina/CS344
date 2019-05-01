#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

#define REQUIRED_ROOMS      7
#define TOTAL_ROOMS         10
#define MAX_CONNECTIONS     6
#define MAX_ROOM_DIGITS     1
#define DIR_NAME_FIRST      "lamartid.rooms."

enum boolean { FALSE, TRUE };
enum room_type { START_ROOM, MID_ROOM, END_ROOM };

struct room{
    int id;
    char* name;
    int numConnections;
    struct room* connections[MAX_CONNECTIONS];
    enum room_type type;
};

char* roomNames[10] = {
    "Atrium",
    "Main_Chamber",
    "Boss_Lair",
    "Secondary_Chamber",
    "Stairwell",
    "Hall_of_Mirrors",
    "Armory",
    "Bottomless_Pit",
    "Secret_Chamber",
    "Healing_Well"
};

// Build name string for directory holding room files
char* BuildDirName(){
    char* first = DIR_NAME_FIRST;
    
    const size_t max_pid_len = 6;       // Max 5 digits on os1 + null terminator
    int pid = getpid();
    char* mypid = malloc(max_pid_len + 1);
    snprintf(mypid, max_pid_len, "%d", pid);

    // https://stackoverflow.com/questions/53230155/converting-pid-t-to-string?noredirect=1&lq=1
    char* dirName = malloc((strlen(first) + strlen(mypid) + 1) * sizeof(char));
    strcpy(dirName, first);
    strcat(dirName, mypid);
    free(mypid);
    return dirName;
}
// Randomly select room names from hard-coded list
void ChooseRoomNames(char* chosenRooms[], int numRooms){
    int i, choice;
    for (i = 0; i < numRooms; i++){
        do{
            choice = rand() % TOTAL_ROOMS;
        }while (roomNames[choice] == NULL);

        chosenRooms[i] = calloc(strlen(roomNames[choice]) + 1, sizeof(char));
        strcpy(chosenRooms[i], roomNames[choice]);
        roomNames[choice] = NULL;
    }
}
// Allocate memory to room structs and init variables
void CreateRooms(struct room* rooms[], int numRooms, char* roomNames[]){
    int i, j;
    for (i = 0; i < numRooms; i++){
        rooms[i] = (struct room*)malloc(sizeof(struct room));
        rooms[i]->id = i;
        rooms[i]->numConnections = 0;
        rooms[i]->name = roomNames[i];

        if (i == 0){
            rooms[i]->type = START_ROOM;
        }else if (i == numRooms - 1){
            rooms[i]->type = END_ROOM;
        }else{
            rooms[i]->type = MID_ROOM;
        }
        // Init all connections to null
        for (j = 0; j < MAX_CONNECTIONS; j++){
            rooms[i]->connections[j] = NULL;
        }
    }
}
// Determine whether graph has met min / max connection requirements
enum boolean IsGraphFull(struct room* rooms[], int numRooms){
    int i;
    for (i = 0; i < numRooms; i++){
        if (rooms[i]->numConnections < 3 || rooms[i]->numConnections > 6){
            return FALSE;
        }
    }
    return TRUE;
}
// Get random room ptr from set of room structs
struct room* GetRandomRoom(struct room* rooms[], int numRooms){
    int i, choice;
    for (i = 0; i < numRooms; i++){
        choice = rand() % numRooms;
    }
    return rooms[choice];
}
// Determine whether connection can be added (fewer than max already)
enum boolean CanAddConnectionFrom(struct room* potentialRoom){
    if (potentialRoom->numConnections < 6){
        return TRUE;
    }
    return FALSE;
}
// Determine whether given connection already exists
enum boolean ConnectionAlreadyExists(struct room* roomA, struct room* roomB){
    int i;
    for (i = 0; i < MAX_CONNECTIONS; i++){
        if (roomA->connections[i] == roomB){
            return TRUE;
        }
    }
    return FALSE;
}
// Connect rooms via pointers
void ConnectRoom(struct room* roomA, struct room* roomB){
    roomA->connections[roomA->numConnections] = roomB;
    (roomA->numConnections)++;
}
// Check room identity
enum boolean IsSameRoom(struct room* roomA, struct room* roomB){
    if (roomA == roomB){
        return TRUE;
    }else{
        return FALSE;
    }
}
// Randomly make double-sided connections between two compatible rooms in set
void AddRandomConnection(struct room* rooms[], int numRooms){
    struct room* roomA;
    struct room* roomB;

    while(1){
        roomA = GetRandomRoom(rooms, numRooms);
        if (CanAddConnectionFrom(roomA) == TRUE){
            break;
        }
    }
    do{
        roomB = GetRandomRoom(rooms, numRooms);
    } while(CanAddConnectionFrom(roomB) == FALSE ||
            IsSameRoom(roomA, roomB) == TRUE ||
            ConnectionAlreadyExists(roomA, roomB) == TRUE);

    ConnectRoom(roomA, roomB);
    ConnectRoom(roomB, roomA);
}
// Build formatted string for room file
char* BuildRoomString(struct room* newRoom){
    char* roomStr = calloc(1000, sizeof(char));

    // Set up 'intros' to each section
    char* nameIntro = "ROOM NAME: ";
    char* connIntro = "CONNECTION ";
    char* typeIntro = "ROOM TYPE: ";
    char* newLine   = "\n";
    // Copy & concat name line
    strcpy(roomStr, nameIntro);
    strcat(roomStr, newRoom->name);
    strcat(roomStr, newLine);
    // Create & concat line for each connection
    int i;
    char* mid = ": ";
    char* name;
    char* connStr;
    for (i = 0; i < newRoom->numConnections; i++){
        mid = ": ";
        name = newRoom->connections[i]->name;
        char* connStr = calloc(strlen(connIntro) +
                               MAX_ROOM_DIGITS +
                               strlen(mid) +
                               strlen(name) + 1, sizeof(char));
        sprintf(connStr, "%s%d%s%s", connIntro, i + 1, mid, name);
        strcat(roomStr, connStr);
        strcat(roomStr, newLine);
        free(connStr);
    }
    char* type;
    if (newRoom->type == START_ROOM){
        type = "START_ROOM";
    }else if (newRoom->type == MID_ROOM){
        type = "MID_ROOM";
    }else{
        type = "END_ROOM";
    }
    strcat(roomStr, typeIntro);
    strcat(roomStr, type);
    strcat(roomStr, newLine);

    return roomStr;
}
// Create all necessary files - 1 per room, formatted with BuildRoomString
void CreateRoomFiles(struct room* rooms[], int numRooms, char* targetDir){
    int i, roomFile, written, bytesToWrite;
    char* slash = "/";
    char* roomStr;
    for (i = 0; i < numRooms; i++){
        char* roomPath = calloc(strlen(targetDir) +
                                strlen(rooms[i]->name) + 
                                strlen(slash) + 1, sizeof(char));
        strcpy(roomPath, targetDir);
        strcat(roomPath, slash);
        strcat(roomPath, rooms[i]->name);

        roomFile = open(roomPath, O_WRONLY | O_CREAT, 0660);
        if (roomFile < 0){
            printf("Could not create and open %s\n", roomPath);
            exit(1);
        }
        roomStr = BuildRoomString(rooms[i]);
        bytesToWrite = strlen(roomStr) * sizeof(char);
        written = write(roomFile, roomStr, bytesToWrite);
        if (written != bytesToWrite){
            printf("Could not complete writing to %s\n", roomPath);
            exit(1);
        }

        free(roomPath);
        free(roomStr);
        close(roomFile);
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
// Driver
int main(int argc, char* argv[]){
    srand(time(0));
    // Create directory to be filled with room files
    char* dirName = BuildDirName();
    mkdir(dirName, S_IRWXU | S_IRWXG);
    // Randomly pick names from list
    char* chosenRoomNames[REQUIRED_ROOMS];
    ChooseRoomNames(chosenRoomNames, REQUIRED_ROOMS);
    // Create room structs
    struct room* rooms[REQUIRED_ROOMS];
    CreateRooms(rooms, REQUIRED_ROOMS, chosenRoomNames);
    // Create connections until requirements met
    while (IsGraphFull(rooms, REQUIRED_ROOMS) == FALSE){
        AddRandomConnection(rooms, REQUIRED_ROOMS);
    }
    // Create files with connected rooms
    CreateRoomFiles(rooms, REQUIRED_ROOMS, dirName);
    // Free memory
    int i;
    for (i = 0; i < REQUIRED_ROOMS; i++){
        free(chosenRoomNames[i]);
        free(rooms[i]);
    }
    free(dirName);
    return 0;
}
