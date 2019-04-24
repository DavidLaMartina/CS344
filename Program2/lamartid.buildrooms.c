#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

char* buildDirName();

int main(int argc, char* argv[]){
    // Create dir lamartid.rooms.procID
    char* dirName = buildDirName();
    mkdir(dirName, S_IRWXU | S_IRWXG);
    // Generate 7 room files - one room per file

    free(dirName);
    return 0;
}

char* buildDirName(){
    char* first = "lamartid.rooms.";
    
    const size_t max_pid_len = 6;   // Max 5 digits on os1 + null terminator
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
