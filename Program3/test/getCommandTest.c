#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../commandInput.h"
#include "../utilities.h"

#define MAX_CHAR        2048
#define MAX_ARGS        512

int main()
{
    // prompt
    printf( ": " );
    fflush(stdout);

    char* linesArr[ MAX_ARGS ];
    nullifyStringArray( linesArr, MAX_ARGS );

    int linesArrSize = MAX_ARGS;

    enum boolean inBackground = FALSE;

    char inFileName[ MAX_CHAR ];
    memset( inFileName, '\0', MAX_CHAR );
    char outFileName[ MAX_CHAR ];
    memset( outFileName, '\0', MAX_CHAR );

    int shellPID = 12345;

    int maxChars = MAX_CHAR;

    getCommand( linesArr,
                linesArrSize,
                &inBackground,
                inFileName,
                outFileName,
                shellPID,
                maxChars );

    int i;
    for ( i = 0; i < MAX_ARGS && linesArr[i]; i++ )
        printf( "%s\n", linesArr[i] );

    printf( "Here is the inFile: %s\n", inFileName );
    printf( "Here is the outFile; %s\n", outFileName );
    printf( "Are we going to run in the background? %d\n", inBackground );

    freeStringArray( linesArr, MAX_ARGS );

    return 0;
}
