/* David LaMartina
 * lamartid@oregonstate.edu
 * Program 4: OTP Encryption & Decryption
 * CS344 Spr2019
 * Due June 9, 2019
 *
 * Decryption main
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
    // Require text, key and port arguments
    if( argc < 4 ){
        error( "USAGE: otp_dec <ciphertext> <key> <port #>" );
    }
    // Assign to named vars for clarity
    char*   cipherFileName = argv[ 1 ];
    char*   keyFileName    = argv[ 2 ];
    int     portNumber     = atoi( argv[ 3 ] );

    // Client & connection variables
    int     socketFD,                   // Socket file descriptor
            charsWritten,               // Number of chars written to server / daemon
            charsRead;                  // Number of chars received from server / daemon
    struct  sockaddr_in serverAddress;  // Socket address of server
    struct  hostent* serverHostInfo;    // Server info used to make connection

    // Length of message and key - check for length
    int     numChars  = fileNumChars( cipherFileName );
    int     keyChars  = fileNumChars( keyFileName );
    int     msgSize   = numChars * 2;

    if( keyChars < numChars ){
        fprintf( stderr, "CLIENT: ERROR Key '%s' is too short\n", keyFileName );
        exit( 1 );
    }

    // Buffers for sending characters
    char*   progID      = (char* )calloc( 1,        sizeof(char) );     // Buffer for program ID
    memset( progID, OTP_DEC_ID, 1 * sizeof(char) );                     // Set to proper ID character
    char*   msgBuffer   = (char* )calloc( numChars, sizeof(char) );     // Buffer for cipher text
    memset( msgBuffer, '\0', numChars * sizeof(char) );                 // Clear garbage
    char*   keyBuffer   = (char* )calloc( numChars, sizeof(char) );     // Buffer for key (same length as text)
    memset( keyBuffer, '\0', numChars * sizeof(char) );                 // Clear garbage

    // Set up server address struct
    memset( (char* )&serverAddress, '\0', sizeof( serverAddress ) );    // Clear address struct
    serverAddress.sin_family = AF_INET;                                 // Create network-capable socket
    serverAddress.sin_port   = htons( portNumber );                     // Store part number
    serverHostInfo           = gethostbyname( TARGET_DOMAIN );          // Convert machine name to address
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
    char* validChars    = CHAR_LIST;                // Character list from otp.h
    enum boolean valid  = FALSE;                    // Chars in buffer (from file) must match valid chars

    readIn( msgBuffer, numChars, cipherFileName );
    stripNewline( msgBuffer );
    valid = validateChars( msgBuffer, numChars, validChars, strlen( validChars ) );
    if( valid == FALSE ){
        fprintf( stderr, "CLIENT: ERROR The file %s contains invalid characters. Exiting.\n", cipherFileName );
        exit( 1 );
    }

    readIn( keyBuffer, numChars, keyFileName );
    stripNewline( keyBuffer );
    valid = validateChars( keyBuffer, numChars, validChars, strlen( validChars ) );
    if( valid == FALSE ){
        fprintf( stderr, "CLIENT: ERROR The file %s contains invalid characters. Exiting\n", keyFileName );
        exit( 1 );
    }

    // Send program ID, buffer size, cipher text, and key to server
    // Use numChars, not strlen(), to ensure '\0' is also sent!
    // Need to send everything in big numChars * 2 size package, to be separated on server size
    // Sending ints across socket: http://forums.codeguru.com/showthread.php?492913-Winsock-send()-Integers
    charsWritten = send( socketFD, progID, 1 * sizeof(char), 0 );
    if( charsWritten < 0 ){
        error( "CLIENT: ERROR writing program ID to socket" );
    }
    charsWritten = send( socketFD, (char*)&msgSize, sizeof(int), 0 );
    if( charsWritten < 0 ){
        error( "CLIENT: ERROR writing message length to socket" );
    }
    charsWritten = send( socketFD, msgBuffer, numChars, 0 );
    if( charsWritten < 0 ){
        error( "CLIENT: ERROR writing message to socket" );
    }
    charsWritten = send( socketFD, keyBuffer, numChars, 0 );
    if( charsWritten < 0 ){
        error( "CLIENT: ERROR writing key to socket" );
    }

    // Get response from daemon & output
    memset( msgBuffer, '\0', numChars * sizeof(char) );                     // Empty out buffer
    charsRead = recv( socketFD, msgBuffer, numChars * sizeof(char), 0 );    // Get message
    if( charsRead < 0 ){
        error( "CLIENT: ERROR reading plain text from socket" );
    }
    printf( "%s\n", msgBuffer );        // Output plaintext include newline

    // Clean up
    close( socketFD );

    free( progID );
    free( msgBuffer );
    free( keyBuffer );

    return 0;
}
