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

enum boolean { FALSE, TRUE };
enum room_type { START_ROOM, MID_ROOM, END_ROOM };

struct room{
    int id;
    char* name;
    int numConnections;
    struct room* connections[6];
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

char* BuildDirName(){
    char* first = "lamartid.rooms.";
    
    const size_t max_pid_len = MAX_CONNECTIONS;   // Max 5 digits on os1 + null terminator
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

        for (j = 0; j < MAX_CONNECTIONS; j++){
            rooms[i]->connections[j] = NULL;
        }
    }
}

enum boolean IsGraphFull(struct room* rooms[], int numRooms){
    int i;
    for (i = 0; i < numRooms; i++){
        if (rooms[i]->numConnections < 3 || rooms[i]->numConnections > 6){
            return FALSE;
        }
    }
    return TRUE;
}

struct room* GetRandomRoom(struct room* rooms[], int numRooms){
    int i, choice;
    for (i = 0; i < numRooms; i++){
        choice = rand() % numRooms;
    }
    return rooms[choice];
}

enum boolean CanAddConnectionFrom(struct room* potentialRoom){
    if (potentialRoom->numConnections < 6){
        return TRUE;
    }
    return FALSE;
}

enum boolean ConnectionAlreadyExists(struct room* roomA, struct room* roomB){
    int i;
    for (i = 0; i < MAX_CONNECTIONS; i++){
        if (roomA->connections[i] == roomB){
            return TRUE;
        }
    }
    return FALSE;
}

void ConnectRoom(struct room* roomA, struct room* roomB){
    roomA->connections[roomA->numConnections] = roomB;
    (roomA->numConnections)++;

    // roomB->connections[roomB->numConnections] = roomA;
    // (roomB->numConnections)++;
}

enum boolean IsSameRoom(struct room* roomA, struct room* roomB){
    if (roomA == roomB){
        return TRUE;
    }else{
        return FALSE;
    }
}

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

void CreateRoomFiles(struct room* rooms[], int numRooms, char* targetDir){
    int i, roomFile;
    for (i = 0; i < numRooms; i++){
        char* roomPath = calloc(strlen(targetDir) +
                                strlen(rooms[i]->name) + 1, sizeof(char));
        strcpy(roomPath, targetDir);
        strcat(roomPath, rooms[i]->name);

        roomFile = open(roomPath, O_WRONLY | O_CREAT, 0660);

        if (roomFile < 0){
            printf("Could not open %s\n", roomPath);
        }

        free(roomPath);
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

int main(int argc, char* argv[]){
    srand(time(0));

    char* dirName = BuildDirName();
    mkdir(dirName, S_IRWXU | S_IRWXG);
    free(dirName);

    char* chosenRoomNames[REQUIRED_ROOMS];
    ChooseRoomNames(chosenRoomNames, REQUIRED_ROOMS);

    struct room* rooms[REQUIRED_ROOMS];
    CreateRooms(rooms, REQUIRED_ROOMS, chosenRoomNames);

    while (IsGraphFull(rooms, REQUIRED_ROOMS) == FALSE){
        AddRandomConnection(rooms, REQUIRED_ROOMS);
    }

    CreateRoomFiles(rooms, REQUIRED_ROOMS, dirName);

    int i;
    for (i = 0; i < REQUIRED_ROOMS; i++){
        free(chosenRoomNames[i]);
        free(rooms[i]);
    }
    return 0;
}
