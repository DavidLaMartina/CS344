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
#include <sys/wait.h>

#include "boolean.h"
#include "utilities.h"
#include "commandBuiltIn.h"
#include "commandInput.h"
#include "commandExec.h"
#include "commandSignals.h"

#define MAX_CHARS       2048        // Maximum length of total command
#define MAX_ARGS        512         // Maximum number of command line arguments
#define MAX_CHILDREN    512         // Assumed...not specified in instructions
#define DEFAULT_EXIT    0           // Default exit status per instruction

// Global to be toggled by SIGTSTP
enum boolean backgroundEnabled = TRUE;

// Built-in command prototypes
void changeDirectory( char* );      // cd (change directory)
void printStatus( int );            // print exit status / signal of most recently terminated child

// Driver
int main(int argc, char* argv[])
{
    // Parent state variables
    int shellPID = getpid();                    // Parent shell's PID - necessary for $$ expansion
    enum boolean inSession = TRUE;              // Continue / end shell flag - only set FALSE by exit

    // Child state variables
    int backgroundPIDs[ MAX_CHILDREN ];         // PID array for keeping track of background children
    int numBackgroundPIDs = 0;                  // Keep track of number of child PIDs for array traversal
    int childExitMethod = DEFAULT_EXIT;         // Most recent foreground exit status
    enum boolean inBackground = FALSE;          // Start w/ assumption child process will be in foreground

    // Handles and buffers
    char inFileName[    MAX_CHARS   ];          // Input file name - set to max chars for safety
    char outFileName[   MAX_CHARS   ];          // Output file name - ""
    char* input[        MAX_ARGS    ];          // Command line input, parsed into <= 512 lines

    // SIGINT handler
    struct sigaction SIGINT_action = {0};       // Instantiate 'empty' handler
    SIGINT_action.sa_handler = SIG_IGN;         // Set to ignore for parent shell
    sigfillset( &SIGINT_action.sa_mask );       // Block / delay all signals arriving while mask in place
    SIGINT_action.sa_flags = 0;                 // Not planning to set flags

    // SIGTSTP handler
    struct sigaction SIGTSTP_action = {0};      // Instantiate 'empty' handler
    SIGTSTP_action.sa_handler = catchSIGTSTP;   // SIGTSTP handler defined in commandSignals.h
    sigfillset( &SIGTSTP_action.sa_mask );      // Block / delay all signals arriving while mask in place
    SIGTSTP_action.sa_flags = 0;                // Not planning to set flags

    // Clear buffers before starting
    memset( inFileName,     '\0', MAX_CHARS * sizeof( inFileName[0] ) );
    memset( outFileName,    '\0', MAX_CHARS * sizeof( outFileName[0] ) );
    nullifyStringArray( input, MAX_ARGS );

    // Set signal handlers before starting
    sigaction( SIGINT, &SIGINT_action, NULL );
    sigaction( SIGTSTP, &SIGTSTP_action, NULL );

    while (inSession == TRUE)
    {
        getCommand( input,
                    MAX_ARGS,
                    &inBackground,
                    inFileName,
                    outFileName,
                    shellPID,
                    MAX_CHARS );

        // If input is not a blank line or comment, proceed with either built-in or executable
        //if ( canIgnore( input ) ) {}
        if ( !canIgnore( input ) ){
            // Check built-ins
            if ( strcmp( input[0], "exit" ) == 0 ){
                inSession = FALSE;
            }
            else if ( strcmp( input[0], "cd" ) == 0 ){
                changeDirectory( input[1] );
            }
            else if ( strcmp( input[0], "status" ) == 0 ){
                printStatus( childExitMethod );
            }

            // If not built-in or blank line, exec command
            else{
                execCommand( input,
                             &childExitMethod,
                             SIGINT_action,
                             inBackground,
                             backgroundEnabled,
                             inFileName,
                             outFileName,
                             backgroundPIDs,
                             &numBackgroundPIDs );
            }
        }

        // Check all background process for termination before returning to prompt
        // ***This happens whether or not the input command was ignored
        int i;
        for ( i = 0; i < numBackgroundPIDs; i++ ){
            pid_t actualPID = waitpid( -1, &childExitMethod, WNOHANG );
            if ( actualPID > 0 ){
                printf( "Background child %d: ", actualPID );
                fflush( stdout );
                printStatus( childExitMethod );
            }
        }

        // If last executed process was foreground, reset SIGINT handler to ignore
        if ( inBackground == FALSE ){
            sigaction( SIGINT, &SIGINT_action, NULL );
        } 
        // Reset next child state and buffers (but retain background PID array and exit method)
        memset( inFileName,     '\0', MAX_CHARS * sizeof( inFileName[0] ) );
        memset( outFileName,    '\0', MAX_CHARS * sizeof( outFileName[0] ) );
        nullifyStringArray( input, MAX_ARGS );
        freeStringArray( input, MAX_ARGS );
        inBackground = FALSE;
    }

    return 0;
}
