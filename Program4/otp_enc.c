/* David LaMartina
 * lamartid@oregonstate.edu
 * Program 4: OTP Encryption & Decryption
 * CS344 Spr2019
 * Due June 9, 2019
 *
 * Encryption client
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "utilities.h"
#include "textHandle.h"
#include "otp.h"

#ifndef TARGET_DOMAIN
#define TARGET_DOMAIN   "localhost"
#endif

int main( int argc, char* argv[] )
{
    // Require text, key, and port arguments
    if ( argc < 4 ){
        error( "USAGE: otp_enc <plaintext> <key> <port #>" );
    }
    // Assign to named vars for clarity
    char*   textFileName = argv[ 1 ];
    char*   keyFileName  = argv[ 2 ];
    int     portNumber   = atoi( argv[ 3 ] );

    // Client & connection variables
    int     socketFD,                   // Socket file descriptor
            charsWritten,               // Number of chars written to server / daemon
            charsRead;                  // Number of chars received from server / daemon
    struct  sockaddr_in serverAddress;  // Socket address of server
    struct  hostent* serverHostInfo;    // Server info used to make connection

    // Buffer for sending characters
    int     numChars = fileNumChars( textFileName );                    // Get num chars in file (including \n)
    char*   msgBuffer = (char*)calloc( numChars, sizeof(char) );        // Buffer for plainText message
    memset( msgBuffer, '\0', numChars * sizeof(char) );                 // Clear garbage
    char*   keyBuffer = (char*)calloc( numChars, sizeof(char) );        // Buffer for key ( same length as text )
    memset( keyBuffer, '\0', numChars * sizeof(char) );                 // Clear garbage

    // Set up server address struct
    memset( (char* )&serverAddress, '\0', sizeof( serverAddress ) );    // Clear address struct
    serverAddress.sin_family = AF_INET;                                 // Create network-capable socket
    serverAddress.sin_port   = htons( portNumber );                     // Store port number
    serverHostInfo           = gethostbyname( TARGET_DOMAIN );          // Convert machine name into address
    if( serverHostInfo == NULL ){
        fprintf( stderr, "CLIENT: ERROR, no such host\n" );
        exit( 0 );
    }
    memcpy( (char* )&serverAddress.sin_addr.s_addr,                     // Copy in address
            (char* )serverHostInfo->h_addr,
            serverHostInfo->h_length );

    // Set up socket
    socketFD = socket( AF_INET, SOCK_STREAM, 0 );                       // Create socket
    if( socketFD < 0 ){
        error( "CLIENT: ERROR opening socket" );
    }
    
    // Connect to server
    if( connect( socketFD, (struct sockaddr* )&serverAddress, sizeof( serverAddress ) ) < 0 ){
        error( "CLIENT: ERROR connecting" );
    }

    // Read in text to buffers and validate
    char* validChars    = CHAR_LIST;                                    // Character list from otp.h
    enum boolean valid  = FALSE;                                        // Chars in buffer (from file) must only match validChars

    readIn( msgBuffer, numChars, textFileName );
    stripNewline( msgBuffer );
    valid = validateChars( msgBuffer, numChars, validChars, strlen( validChars ) );
    
    if( valid == FALSE ){
        fprintf( stderr, "The file %s contains invalid characters. Exiting.\n", textFileName );
        exit( 1 );
    }
    printf( "msgBuffer: %s\n", msgBuffer );

    readIn( keyBuffer, numChars, keyFileName );
    stripNewline( keyBuffer );
    valid = validateChars( keyBuffer, numChars, validChars, strlen( validChars ) );

    if( valid == FALSE ){
        fprintf( stderr, "The file %s contains invalid characters. Exiting.\n", keyFileName );
        exit( 1 );
    }
    printf( "keyBuffer: %s\n", keyBuffer );
 
    // Send plaintext and key to server
    // Use while loop to ensure all bytes sent in case of interrupt
    do{
        charsWritten = send( socketFD, msgBuffer, strlen( msgBuffer ), 0 );         // Send message to daemon
        if( charsWritten < 0 ){                                                     // Error check
            error( "CLIENT: ERROR writing message to socket" );
        }
    }while( charsWritten < strlen( msgBuffer ) );

    do{
        charsWritten = send( socketFD, msgBuffer, strlen( keyBuffer ), 0 );         // Send key to daemon
        if( charsWritten < 0 ){
            error( "CLIENT: ERROR writing key to socket" );
        }
    }while( charsWritten < strlen( keyBuffer ) );

    // Get response from daemon & output

    close( socketFD );
    return 0;
}
