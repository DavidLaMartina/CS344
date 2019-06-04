#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../textHandle.h"
#include "../boolean.h"

char* code = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

int main()
{
    int bufSize = 10000;
    char buf[ bufSize ];
    char* fileName = "../plaintext1";

    readIn( buf, sizeof(buf), fileName );
    stripNewline( buf );

    printf( "%s\n", buf );

    enum boolean valid = validateChars( buf, sizeof(buf), code, strlen(code) );

    if( valid == TRUE ) printf( "valid!\n" );
    else printf( "invalid!\n" );

    return 0;
}
