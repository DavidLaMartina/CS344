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
void printSafe( const char* message, int location )
{
    // Check null
    if ( !message )
        return;

    write( location, message, strlen( message ) * sizeof( char ) );
}

// Replaces every instance of old word in original string with new word
// Assumes the original string is null-terminated
// *** Malloc'd memory must be freed by caller
// Credit: https://www.geeksforgeeks.org/c-program-replace-word-text-another-given-word/
char* replaceWord( const char* original, const char* oldWord, const char* newWord )
{
    char *result;       // Will hold result (allocated memory)
    int i;              // index variable
    int count = 0;      // count of old word instances to be replaced

    int newWordLength = strlen( newWord );  // Track lengths to allocate memory properly
    int oldWordLength = strlen( oldWord );

    // Count number of times old word occurs in string
    for ( i = 0; original[i] != '\0'; i++ ){
        if ( strstr( &original[i], oldWord ) == &original[i] ){
            count++;

            // If found, jump to index just beyond the instance of oldWord
            // to ensure exactly the right number of instances found
            i += oldWordLength - 1;
        }
    }
    // Make new string long enough to hold all instances of new word
    // +1 at end to account for null terminator (will be added before returning)
    result = (char*)malloc( i + count * (newWordLength - oldWordLength) + 1 );
    
    i = 0;      // Reset index to 0 before second traversal & replacement

    while( *original ){
        // Find instances of old word again, replacing and advancing respective pointesr
        // to account for difference in length between old and new words
        if ( strstr( original, oldWord ) == original ){
            strcpy( &result[i], newWord );
            i += newWordLength;
            original += oldWordLength;
        }
        else{
            result[i++] = *original++;
        }
    }
    result[i] = '\0';   // Add null terminator to finish
    return result;
}

// Get the number of digits in an integer (required for conversion from int to string)
// Credit: https://www.programiz.com/c-programming/examples/digits-count
int numDigits( int num )
{
    int count = 0;

    while( num != 0 ){
        num /= 10;
        ++count;
    }
    return count;
}

// Convert an integer to a string
// Requires a passed-in buffer - This keeps memory allocation responsibility on the caller
// Credit: https://stackoverflow.com/questions/8257714/how-to-convert-an-int-to-string-in-c
// void intToString( int num, char* buf )
// {
//     // First memset to null to ensure digit string will be null-terminated
//     memset( buf, '\0', strlen( buf ) * sizeof( char ) );
// 
//     // Get max number of bytes to write with numDigits
//     int maxBytes = sizeof( char ) * ( numDigits( num ) + 1 );   // +1 to account for null termiantor
// 
//     // Now use snprintf to 
//     snprintf( buf, maxBytes, "%d", num );
// }

char* intToString( int num )
{
    // First get length
    int length = snprintf( NULL, 0, "%d", num );
    
    // Allocate buffer based on length + null terminator
    char* numString = ( char* )malloc( sizeof(char) * (length + 1) );

    // Now call snprintf again to put string into buffer
    snprintf( numString, length + 1, "%d", num );

    return numString;
}
