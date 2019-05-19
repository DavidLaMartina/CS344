#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../commandInput.h"
#include "../utilities.h"

int main()
{
    int num = 12345;
    int num2 = 12345667;

    printf( "The number of digits in %d is %d.\n", num, numDigits( num ) );
    printf( "The number of digits in %d is %d.\n", num2, numDigits( num2 ) );

    return 0;
}
