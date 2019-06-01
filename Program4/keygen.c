/* David LaMartina
 * lamartid@oregonstate.edu
 * Program 4: OTP Encryption & Decryption
 * CS344 Spr2019
 * Due June 9, 2019
 *
 * Key generator:
 *  Keygen creates a key file of specified length intended for OTP encryption
 *  and decryption. The characters generated will include the 26 capital English
 *  alphabet characters and a white space. The last character of the output is a
 *  newline, which adds an extra character to the specified length.
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define CHAR_CODE_LENGTH        27

const char charList[ CHAR_CODE_LENGTH ] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

int main(int argc, char* argv[])
{
    // Exit w/ error if no argument
    if( argc < 2 ){
        fprintf( stderr, "Keygen usage: 'keygen <length>'\n" );
        exit( 1 );
    }

    int length = atoi( argv[1] );                           // Convert arg to length
    char* key = (char* )calloc( length + 1, sizeof(char) ); // Buffer + extra char for newline
    memset( key, '\0', length + 1 );                        // Avoid garbage

    srand( time(0) );                                       // Seed random number generator

    // Fill array with random characters and finish with newline
    int i;
    for( i = 0; i < length; i++ ){
        key[ i ] = charList[ rand() % CHAR_CODE_LENGTH ];
    }
    key[ length ] = '\n';

    fprintf( stdout, "%s", key );   // Output for redirection
    free( key );

    return 0;
}
