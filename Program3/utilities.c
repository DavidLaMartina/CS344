/* David LaMartina
 * lamartid@oregonstate.edu
 * Simple utilities for Program 3
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utilities.h"
#include "boolean.h"

// Set all top-level elements (input buffer lines) of string array to NULL
void nullifyStringArray( char** buffer, int bufSize )
{
    int i;
    for (i = 0; i < bufSize; i++ ){
        buffer[i] = NULL;
    }
}

// Free all memory possibly allocated to char**. All top-level elements
// assumed to be either null or allocated with malloc() or alloc()
void freeStringArray( char** buffer, int bufSize )
{
    int i;
    for (i = 0; i < bufSize; i++){
        free( buffer[i] );
    }
}

// Check whether line can be ignored (blank or # comment)
enum boolean canIgnore( char** buffer )
{
    if ( buffer[0] == NULL ){
        return TRUE;
    }
    else if ( buffer[0][0] == '#'  ||
              buffer[0][0] == '\0' ||
              buffer[0][0] == '\n' ){
        return TRUE;
    } else {
        return FALSE;
    }
}

// Re-entrant output using write() and fflush()
// 'location' param is place to output - stdin, stdout, stderr,
// or some other location / file handler
void printSafe( char* message, int location )
{
    // Check null
    if ( !message )
        return;

    write( location, message, strlen( message ) * sizeof( char ) );
}
