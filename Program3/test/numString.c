#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../commandInput.h"
#include "../utilities.h"

int main()
{
    int num = 12345;

    char* numStr = intToString( num );

    printf( "Here is your number: %d\n", num );
    printf( "Here is your string: %s\n", numStr );

    free( numStr );
 
    return 0;
}
