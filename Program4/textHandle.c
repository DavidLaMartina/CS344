/* David LaMartina
 * lamartid@oregonstate.edu
 * Project 4: OTP Encryption & Decryption
 * CS344 Spr2019
 * Due June 9, 2019
 *
 * File-handling helpers
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "boolean.h"
#include "textHandle.h"

// Read in text from file to buffer
// solarianprogrammer.com/2019/04/03/c-programming-read-file-lines-fgets-getline-implement-portable-getline/rror
void readIn( char* buf, int bufSize, char* fileName )
{
    memset( buf, '\0', bufSize * sizeof( char ) );  // Get rid of buffer garbage
    
    FILE* textFile = fopen( fileName, "r" );        // Open file for reading
    if( !textFile ){
        fprintf( stderr, "ERROR: Could not open file %s for reading.\n", fileName );
        exit( 1 );
    }
    // Read file (one line, only newline at the end) into buffer
    fgets( buf, bufSize + 1, textFile );            // bufSize + 1 b/c fgets reads n - 1 chars

    fclose( textFile );                             // Close file handle
}

// Replace ending newline with null terminator
void stripNewline( char* buf )
{
    buf[ strcspn( buf, "\n" ) ] = '\0';
}

// Put newline "back" in place of null terminator
void replaceNewline( char* buf )
{
    buf[ strcspn( buf, "\0" ) ] = '\n';
}

// Replace first instance of one character with another
void replaceChar( char* buf, char* old, char new )
{
    buf[ strcspn( buf, old ) ] = new;
}

// Validate characters in a buffer against set of acceptable characters
enum boolean validateChars( char* buf, int bufSize, char* validChars, int validCharsSize )
{
    int bufIdx, charIdx;                                            // Looping indices
    enum boolean valid;                                             // Flag for return

    // For each char in buffer, check against all chars in valid
    for( bufIdx = 0; bufIdx < bufSize && buf[ bufIdx ] != '\0'; bufIdx++ ){
        valid = FALSE;

        for( charIdx = 0; charIdx < validCharsSize && validChars[ charIdx ] != '\0'; charIdx++ ){
            if ( buf[ bufIdx ] == validChars[ charIdx ] ){
                valid = TRUE;
                break;
            }
        }

        if ( valid == FALSE )
            break;
    }

    return valid;
}
