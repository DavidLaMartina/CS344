#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../otp.h"
#include "../textHandle.h"
#include "../utilities.h"

int main()
{
    char* charList = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

    char* plain = "ESAU ATE HIS OWN POOP HOW GROSS";
    char* key   = "SDFE IOSNDESLKJASELKF JSDFZZZZZ";

    //char* plain = "DAVID FLEW OVER THE CUCKOOS NEST";
    //char* key   = "AD EQWWWQ DFAS EEEE   DFASDFAS Z";
    char* cipher= calloc( strlen( key ) + 1, sizeof( char ));
    memset( cipher, '\0', ( strlen(key) + 1) * sizeof(char) );

    printf( "%s\n", plain );
    printf( "%s\n", key );
    otpEncrypt( plain, key, cipher, charList );

    printf( "%s\n", cipher );

    char* newPlain = calloc( strlen( key ) + 1, sizeof( char ));
    memset( newPlain, '\0', ( strlen(key) + 1) * sizeof( char ));

    otpDecrypt( cipher, key, newPlain, charList );
    printf( "%s\n", newPlain );


    // char* testStr = "DAVIDABD";
    // char* key     = "LAMARJUI";
    // char cipher[ strlen( testStr ) + 1 ];
    // memset( cipher, '\0', (strlen( testStr ) + 1) * sizeof( char ));

    // // printf( "Plaintext length: %d\n", strlen( testStr ) );
    // // printf( "Character array length: %d\n", strlen( charList ) );
    // // printf( "key length: %d\n", strlen( key ) );
    // // printf( "The size of the cipher buffer is %d\n", sizeof( cipher ) );
    // // printf( "The size of the test string is %d\n", sizeof( testStr )) ;
    // 
    // printf( "%s\n", testStr );

    // otpEncrypt( testStr, key, cipher, charList );

    // printf( "%s\n", cipher );

    // char newPlain[ strlen( cipher ) + 1 ];
    // memset( newPlain, '\0', (strlen( cipher ) + 1) * sizeof(char));

    // otpDecrypt( cipher, key, newPlain, charList );

    // printf( "%s\n", newPlain );

    return 0;
}
