/* David LaMartina
 * lamartid@oregonstate.edu
 * CS344 Spr2019
 * Built-in commands for smallsh
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "boolean.h"
#include "utilities.h"
#include "commandBuiltIn.h"

// If user enters "cd" or "cd [path]"
void changeDirectory( char* path )
{
    // If path not null, change dir to path
    if ( path ){
        if ( chdir( path ) == -1 ){
            printf( "Path not found...try again\n" );
            fflush( stdout );
        }
    }
    // If path null, change dir to HOME
    else {
        chdir( getenv( "HOME" ) );
    }
}

// If user enters "status", prints the appropriate exit status or signal
// termination of the most recently terminated foreground child process
void printStatus( int childExitMethod )
{
    if ( WIFEXITED( childExitMethod ) ){
        printf( "exit value %d\n",
                WEXITSTATUS( childExitMethod ) );
        fflush( stdout );
    }
    // Could be an else statement, but we're being extra careful before
    // invoking the WTERMSIG macro
    else if ( WIFSIGNALED( childExitMethod ) ){
        printf( "terminated by signal %d\n",
                WTERMSIG( childExitMethod ) );
        fflush( stdout );
    }
}

// Print current working directory: diagnostic to ensure
// cd is working correctly!
void printWorkingDirectory()
{
    char cwd[ 512 ];        // arbitrary max buffer size 512...let's hope you don't have longer dir names!
    if( getcwd( cwd, sizeof( cwd ) ) != NULL ) {
        printf( "Current working directory: %s\n", cwd );
        fflush( stdout );
    }
    else{
        perror( "getcwd() error" );
        fflush( stdout );
    }
}
