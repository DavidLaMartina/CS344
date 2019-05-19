/* David LaMartina
 * lamartid@oregonstate.edu
 * Program 3: smallsh
 * CS344 Spr2019
 * Due May 26, 2019
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

#include "boolean.h"
#include "utilities.h"
#include "commandInput.h"

#define MAX_CHARS       2048    // Maximum length of total command
#define MAX_ARGS        512     // Maximum number of command line arguments
#define MAX_CHILDREN    512     // Assumed...not specified in instructions
#define DEFAULT_EXIT    0       // Default exit status per instruction

// Built-in command prototypes
void changeDirectory( char* );      // cd (change directory)
void printStatus( int );            // print exit status / signal of most recently terminated child

// Driver
int main(int argc, char* argv[])
{
    // Parent state variables
    int shellPid = getpid();                // Parent shell's PID - necessary for $$ expansion
    enum boolean inSession = TRUE;          // Set continue flag to true for first prompt

    // Child state variables
    int backgroundPIDs[ MAX_CHILDREN ];     // PID array for keeping track of background children
    int childExitMethod = DEFAULT_EXIT;     // Most recent foreground exit status
    enum boolean inBackground = FALSE;      // Start w/ assumption child process will be in foreground

    // Handles and buffers
    char inFileName[    MAX_CHARS   ];      // Input file name - set to max chars for safety
    char outFileName[   MAX_CHARS   ];      // Output file name - ""
    char* input[        MAX_ARGS    ];      // Command line input, parsed into <= 512 lines

    // Clear buffers before starting
    memset( inFileName,     '\0', MAX_CHARS * sizeof( inFileName[0] ) );
    memset( outFileName,    '\0', MAX_CHARS * sizeof( outFileName[0] ) );
    nullifyStringArray( input, MAX_ARGS );

    while (inSession == TRUE)
    {
        // Get user input
        printf( ": " );
        fflush( stdout );
        // getCommand( input, &inBackground, inFile, outFile, );
        // *** SET SIGINT HANDLER BACK TO NORMAL AFTER THIS

        // Check if comment or blank line
        if ( canIgnore( input ) ) {}

        // Check built-ins
        else if ( strcmp( input[0], "exit" ) == 0 ){
            inSession = FALSE;
        }
        else if ( strcmp( input[0], "cd" ) == 0 ){
            changeDirectory( input[1] );
        }
        // status
        else if ( strcmp( input[0], "status" ) == 0 ){
            printStatus( childExitMethod );
        }
 
        // Reset state and buffers
        memset( inFileName,     '\0', MAX_CHARS * sizeof( inFileName[0] ) );
        memset( outFileName,    '\0', MAX_CHARS * sizeof( outFileName[0] ) );
        nullifyStringArray( input, MAX_ARGS );
    }

    return 0;
}

/*******************************************************************************
 * Built-in cd and status commands
 ******************************************************************************/

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
        printf( "Process exited with status %d\n",
                WEXITSTATUS( childExitMethod ) );
        fflush( stdout );
    }
    // Could be an else statement, but we're being extra careful before
    // invoking the WTERMSIG macro
    else if ( WIFSIGNALED( childExitMethod ) ){
        printf( "Process terminated by signal %d\n",
                WTERMSIG( childExitMethod ) );
        fflush( stdout );
    }
}

// /*******************************************************************************
//  * Simple utilities for reducing code in main and "big" helpers
//  ******************************************************************************/
// 
// // Set all top-level elements (input buffer lines) of string array to NULL
// void nullifyStringArray( char** buffer, int bufSize )
// {
//     int i;
//     for (i = 0; i < bufSize; i++ ){
//         buffer[i] = NULL;
//     }
// }
// 
// // Check whether line can be ignored (blank or comment)
// enum boolean canIgnore( char** buffer )
// {
//     if ( buffer[0] == NULL ){
//         return TRUE;
//     }
//     else if ( buffer[0][0] == '#'  ||
//               buffer[0][0] == '\0' ||
//               buffer[0][0] == '\n' ){
//         return TRUE;
//     } else {
//         return FALSE;
//     }
// }
// 
// // Re-entrant output using write() and fflush()
// // 'location' paramer is place to output - likely stdin, stdout, or stderr
// //      but could be other file handler
// void printSafe( char* message, int location )
// {
//     // Check null
//     if ( !message ) return;
// 
//     write( location, message, strlen( message ) * sizeof( char ) );
// }
