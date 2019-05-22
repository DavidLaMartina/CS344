/* David LaMartina
 * lamartid@oregonstate.edu
 * Command execution for smallsh
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#include "boolean.h"
#include "commandInput.h"
#include "commandExec.h"
#include "commandBuiltIn.h"
#include "utilities.h"

// Execute user's non-built-in command
// Redirect input / output as specified
// Run process in foreground / background as specified
// Alter signal handling to cause child process to terminate itself upon SIGINT

void execCommand( char** command,                   // Parsed array of strings representing user's command
                  int* childExitMethod,             // Child exit status - will need to be altered so parent knows
                  struct sigaction SIGINT_action,   // SIGINT action - behavior changd for child proc vs. parent
                  enum boolean inBackground,        // Defines whether process will run in background if exec
                  enum boolean backgroundEnabled,   // Defines whether process CAN run in background
                  char inFileName[],                // Input file name
                  char outFileName[],               // Output file name
                  int backgroundPIDs[],             // Array of background child PIDs
                  int* numBackgroundPIDs )          // Current number of background child processes
{
    pid_t spawnPID = -5;                            // Begin with nonsense value to check against
    int redirSuccess;                               // Redirection success indicator for branching

    spawnPID = fork();                              // Create new child shell    
    switch( spawnPID ){                             // Branch based on return
        
        case -1:                                    // Exit with error if fork disallowed by the OS
            perror( "Hull breach!\n" );
            exit( 1 );
            break;

        case 0:                                     // In child process
            // If foreground process, set SIGINT handler to default (terminate self) behavior
            if ( inBackground == FALSE ){
                SIGINT_action.sa_handler = SIG_DFL;
                sigaction( SIGINT, &SIGINT_action, NULL );
            }
            // Redirect input & output as needed; non-used inFile / outFile names will be empty
            // No redirection will occur if foreground process w/ no files specified
            redirInput( inFileName, inBackground, backgroundEnabled );
            redirOutput( outFileName, inBackground, backgroundEnabled ); 

            // Execute using redirects created by dup2() calls
            if ( execvp( command[0], command ) < 0 ){
                printf( "%s: No such file or directory.\n", command[0] );
                fflush( stdout );
                exit( 1 );
            }
            break;

        default:                                    // In parent process
            // Run as background only if chosen AND enabled
            // Running in background means not waiting for child termination to reprompt
            if ( inBackground == TRUE && backgroundEnabled == TRUE ){
                pid_t actualPID = waitpid( spawnPID, childExitMethod, WNOHANG );
                backgroundPIDs[ *numBackgroundPIDs ] = actualPID;
                ( *numBackgroundPIDs )++;
                printf( "background PID is %d\n", spawnPID );
                fflush( stdout );
            }
            // Else run in foreground by waiting for child termination to re-prompt 
            else{
                pid_t actualPID = waitpid( spawnPID, childExitMethod, 0 );
            }

        // Check all background processes for termination before returning to prompt
        int i;
        for ( i = 0; i < *numBackgroundPIDs; i++ ){
            pid_t actualPID = waitpid( -1, childExitMethod, WNOHANG );
            if ( actualPID > 0 ){
                printf( "Child %d terminated\n", actualPID );
                printStatus( *childExitMethod );
                fflush( stdout );
            }
        }
    }
}

// Redirect input to file opened for read-only; exit if open fails
void redirInput( char inFileName[],
                 enum boolean inBackground,
                 enum boolean backgroundEnabled )
{
    // Whether background or foreground, try to redirect if input file specified
    if ( strcmp( inFileName, "" ) != 0 ){
        int inFD = open( inFileName, O_RDONLY );
        if ( inFD == -1 ){
            perror( "Unable to open file for input. Terminating process.\n" );
            exit( 1 );
        }
        int result = dup2( inFD, 0 );
        if ( result == -1 ){
            perror( "Unable to redirect standard input. Exiting.\n" );
            exit( 1 );
        }
        // Designate close on exec
        fcntl( inFD, F_SETFD, FD_CLOEXEC );
    }
    // If no input file specified and proc will run in background, redir from /dev/null
    else if ( inBackground == TRUE && backgroundEnabled == TRUE ){
        int inFD = open( "/dev/null", O_RDONLY );
        if ( inFD == -1 ){
            perror( "This is weird, but /dev/null couldn't be opened for input. Terminating process.\n" );
            exit( 1 );
        }
        int result = dup2( inFD, 0 );
        if ( result == -1 ){
            perror( "This is weird, but the system couldn't read from /dev/null. Exiting.\n" );
            exit( 1 );
        }
    }
}

// Redirect output to file opened for creation / truncation; exit if open fails
void redirOutput( char outFileName[],
                  enum boolean inBackground,
                  enum boolean backgroundEnabled )
{
    if ( strcmp( outFileName, "" ) != 0 ){
        int outFD = open( outFileName, O_WRONLY | O_CREAT | O_TRUNC, 0644 );
        if ( outFD == -1 ){
            perror( "Unable to open file for output. Terminating process.\n" );
            exit( 1 );
        }
        int result = dup2( outFD, 1 );
        if ( result == -1 ){
            perror( "Unable to redirect standard output. Exiting.\n" );
            exit( 1 );
        }
        // Designate close on exec
        fcntl( outFD, F_SETFD, FD_CLOEXEC );
    }
    // If no output file specified and proc will run in background, redir to /dev/null
    else if ( inBackground == TRUE && backgroundEnabled == TRUE ){
        int outFD = open( "/dev/null", O_WRONLY );
        if ( outFD == -1 ){
            perror( "This is weird, but /dev/null couldn't be opened for output. Terminating process.\n" );
            exit( 1 );
        }
        int result = dup2( outFD, 1 );
        if ( result == -1 ){
            perror( "This is weird, but the system couldn't write to /dev/null. Exiting.\n" );
            exit( 1 );
        }
    }
}
