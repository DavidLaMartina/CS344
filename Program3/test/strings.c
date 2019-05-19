#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../commandInput.h"

int main()
{
    printf( "Enter a line: " );
    fflush( stdout );
    char* newLine = getInputLine();

    printf( "Here's your line - %s", newLine );

    free( newLine );

    return 0;
}
