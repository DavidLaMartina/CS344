/* David LaMartina
 * lamartid@oregonstate.edu
 * CS344 Spr2019
 * Built-in commands for Program 3: smallsh
 * Due May 26, 2019
 */

#include <stdlib.h>
#include <stdio.h>
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
        printf( "exit value %d\n", WEXITSTATUS( childExitMethod ) );
        fflush( stdout );
    }
    // Could be an else statement, but we're being extra careful before
    // invoking the WTERMSIG macro
    else if ( WIFSIGNALED( childExitMethod ) ){
        printf( "terminated by signal %d\n", WTERMSIG( childExitMethod ) );
        fflush( stdout );
    }
}
