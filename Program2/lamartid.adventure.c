#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

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

char* MostRecentRooms(){
    int newestDirTime = -1;                         // timestamp of newest subdir
    char targetDirPrefix[100] = DIR_NAME_FIRST;     // prefix to search for
    char* newestDirName = calloc(100, sizeof(char)); // holds name of newest candidate
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

int main(int argc, char* argv[]){
    char* roomsDir = MostRecentRooms();

    printf("The most recent directory for rooms is %s\n", roomsDir);

    free(roomsDir);

    return 0;
}
