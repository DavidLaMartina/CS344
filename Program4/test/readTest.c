#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../textHandle.h"
#include "../utilities.h"
#include "../boolean.h"

int main()
{
    char* fileName = "testFile";

    int numChars = fileNumChars( fileName);
    printf( "There are %d characters in the file\n", numChars );

    // char buf[ numChars ];
    char* buf = calloc( numChars, sizeof( char ));
    memset( buf, '\0', (numChars) * sizeof( char ) );

    readIn( buf, numChars, fileName );
    stripNewline( buf );

    printf( "%s\n", buf );

    char* validChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
    enum boolean valid = validateChars( buf, numChars, validChars, strlen(validChars) );

    if( valid == TRUE ) printf( "Valid!\n" );
    else printf( "Invalid!\n" );

    return 0;
}
