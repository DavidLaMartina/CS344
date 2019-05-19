#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../commandInput.h"
#include "../utilities.h"

#define MAX_CHAR        2048
#define MAX_ARGS        512

int main()
{
    char* linesArr[ MAX_ARGS ];
    nullifyStringArray( linesArr, MAX_ARGS );

    int linesArrSize = MAX_ARGS;

    enum boolean inBackground = FALSE;

    char inFileName[ MAX_CHAR ];
    char outFileName[ MAX_CHAR ];

    int shellPID = 12345;

    int maxChars = MAX_CHAR;

    getCommand( linesArr,
                linesArrSize,
                &inBackground,
                inFileName,
                outFileName,
                shellPID,
                maxChars );



    return 0;
}
