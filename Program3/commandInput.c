/* David LaMartina
 * lamartid@oregonstate.edu
 * Command line input retrieval and parsing for Program 3: smallsh
 * CS344 Spr2019
 * Due May 26, 2019
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "boolean.h"
#include "commandInput.h"
#include "utilities.h"

// SIGINT-catcher function -
// credit Lecture 3.3 - Advanced User Input with Getline()
void catchSIGINT( int signo )
{
    const char* message = "SIGINT. Use CTRL-Z to Stop.\n";
    printSafe( message, STDOUT_FILENO );
}

// Get input from user via getline and parse individual words into strings.
// Store strings into passed-in array, whose memory can be managed by caller.
// Credit to example at https://stackoverflow.com/questions/4513316/split-string-in-c-every-white-space
void getCommand( char** linesArr,
                 int    linesArrSize,
                 enum   boolean* inBackground,
                 char*  inFileName,
                 char*  outFileName,
                 int    shellPID,
                 int    maxChars )
{
    const char* shellExpand = "$$"; // const string to be used in expansion from $$ to shellPID

    // Convert shellPID to string for use in expansions as needed -- needs to be freed at end
    char* shellPIDString = intToString( shellPID );

    // Get user input, accounting for possible interruptions; returned line is scrubbed of \n
    char* userInput = getInputLine();

    // If blank...do anything?
    
    // Parse line of input into discrete words
    const char* space = " ";                        // space is our only delimeter
    char* nextWord = strtok( userInput, space );    // start up strtok (internal state) and get first word

    int i;
    // Continue parsing until words are depleted or input array is full
    // For each word / phrase, either set appropriate referened variable or store in array
    // Using for-loop vs. while to easily maintain index; could have also been done w/ while and counter
    for ( i = 0; nextWord && i < linesArrSize; i++ ){
        // Check < for input file
        if ( strcmp( nextWord, "<" ) == 0 ){
            nextWord = strtok( NULL, space );       // Next word will be input filename
            strcpy( inFileName, nextWord );         // Copy to retain characters in main's memory
        }

        // Check > for output file
        else if ( strcmp( nextWord, ">" ) == 0 ){
            nextWord = strtok( NULL, space );       // Next word will be output filename
            strcpy( outFileName, nextWord );        // Copy to retain characters in main's memory
        }

        // Check & for background process
        else if ( strcmp( nextWord, "&" ) == 0 ){
            *inBackground = TRUE;
        }

        // If still in loop, phrase / word must be a command or argument (so store in array)
        // Run every word through replaceWord to ensure any instance of $$ replaced with shell PID
        else {
            char* nextWordExpanded = replaceWord( nextWord, shellExpand, shellPIDString );
            linesArr[i] = nextWordExpanded;
        }
        // Call strtok again to advance to next word
        nextWord = strtok( NULL, space );
    }
    // Free memory alloc'd by getline() in getInputLine() call and intToString
    free( userInput );
    free( shellPIDString );
}

// Input-fetching function
// Adapted from 3.3 Advanced User Input with getline()
char* getInputLine()
{
    // Set up SIGINT handler for interruptions and avoidance of stuck errors
    struct sigaction SIGINT_action = {0};
    // SIGINT_action.sa_handler = catchSIGINT;
    SIGINT_action.sa_handler = SIG_IGN;
    sigfillset( &SIGINT_action.sa_mask );
    sigaction( SIGINT, &SIGINT_action, NULL );

    int numCharsEntered = -5;       // Start with nonsense value for numChars
    size_t bufferSize = 0;          // Size allocated buffer
    char* lineEntered = NULL;       // Points to buffer to be alloc'd by getline()
 
    // Loop to ensure entire input is gathered, even in case of interrupt
    while( 1 ){
        // prompt each time
        printf( ": " );
        fflush( stdout );

        numCharsEntered = getline( &lineEntered, &bufferSize, stdin );  // Get characters from user + \n + \0
        if ( numCharsEntered < 0 )
            clearerr( stdin );
        else
            break;
    }
    // Remove newline
    lineEntered[ strcspn( lineEntered, "\n" )] = '\0';
    
    return lineEntered;
}
