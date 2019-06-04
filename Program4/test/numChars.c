#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utilities.h"

int main()
{
    int numChars = fileNumChars( "testFile" );
    printf( "There are %d characters in the file\n", numChars );
    return 0;
}
