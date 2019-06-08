/* David LaMartina
 * lamartid@oregonstate.edu
 * Program 4: OTP Encryption & Decryption
 * CS344 Spr2019
 * Due June 9, 2019
 *
 * Decryption daemon
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "utilities.h"
#include "textHandle.h"
#include "otp.h"

#define MAX_CONNECTIONS     5       // max concurrent connections
#define CHUNK_SIZE          1000    // chunk size for receiving data from client

// Prototype for handling actual connections & encryptions in child procs
void decrypt( int );

int main( int argc, char* argv[] )
{
    // Require port argument
    if( argc < 2 ){
        error( "USAGE: otp_dec_d <port #>" );
    }

    // Daemon variables
    int listenSocketFD,                 // Socket to listen for client connections
        establishedConnectionFD,        // Socket for connecting to client
        portNumber;                     // Port to run server (this daemon)
    socklen_t sizeOfClientInfo;         // Amount of into coming from client
    struct sockaddr_in serverAddress,   // Daemon's identifier
                       clientAddress;   // Client's identifier

    // Process handling variables
    int numConnections  = 0;            // Current connections; cannot be greater than MAX_CONNECTIONS
    int childExitMethod = -5;           // Begin with nonsense value to check against

    // Set up address struct for this daemon
    memset( (char* )&serverAddress, '\0', sizeof( serverAddress ) );    // Clear address struct
    portNumber = atoi( argv[ 1 ] );                                     // Convert port number arg to int
    serverAddress.sin_family        = AF_INET;                          // Network-capable socket
    serverAddress.sin_port          = htons( portNumber );              // Store port number
    serverAddress.sin_addr.s_addr   = INADDR_ANY;                       // Any address allowed

    // Set up socket
    listenSocketFD = socket( AF_INET, SOCK_STREAM, 0 );                 // Create socket
    if( listenSocketFD < 0 ){
        error( "SERVER: ERROR opening socket." );
    }

    // Enable socket to being listening
    if( bind( listenSocketFD,                                           // Connect socket to port
              (struct sockaddr *)&serverAddress,
              sizeof( serverAddress ) ) < 0 ){
        error( "SERVER: ERROR on binding" );
    }
    listen( listenSocketFD, 5 );                                        // Flip socket on w/ queue of 5

    // Listen for client connections indefinitely
    while( 1 )
    {
        // Opening new connection depends on # existing connections
        // Check number of child processes against max, updating as needed
        while( waitpid( -1, &childExitMethod, WNOHANG ) > 0 ){
            // printf( "A child died!\n" );
            numConnections--;
        }
        // If < max connections exist, accept next client connection
        if( numConnections < MAX_CONNECTIONS ){
            // Accept a connection, blocking if one is not available until one connects
            sizeOfClientInfo        = sizeof( clientAddress );      // Get size of client address
            establishedConnectionFD = accept( listenSocketFD,
                                              (struct sockaddr *)&clientAddress,
                                              &sizeOfClientInfo );  // Accept
            if( establishedConnectionFD < 0 ){
                error( "SERVER: ERROR on accpet" );
            }
            // Fork accepted connection
            pid_t spawnPID  = -5;                               // start w/ nonsense to check against
            spawnPID        = fork();

            // Branch based on parent / child; execute daemon business logic in child
            switch( spawnPID ){

                case -1:                                        // Error & exit if spawn fails
                    error( "Hull breach!\n" );
                    break;

                case 0:                                         // Child --> do business logic
                    decrypt( establishedConnectionFD );
                    break;

                default:                                        // Parent --> increment child count
                    numConnections++;
            }
        }
        // If max connections exist, re-loop when one exits
        else{
            if( waitpid( -1, &childExitMethod, 0 ) > 0 ){
                numConnections++;
            }
        }
    }

    close( listenSocketFD );
    return 0;
}

void decrypt( int socketFD )
{
    int charsRead,          // Characters read from packet; must be checked against 0
        charsSum,           // Sum of characters from multiple packets
        charsWritten,       // Check to ensure all characters have been sent to client
        msgSize;            // Size of total message -- need to get this first!

    // Check identity of client (We know first message sent is identifying character )
    // If wrong program, write error message to be interpreted for output on client side
    char* clientChar = calloc( 1, sizeof(char) );               // Need char buffer to hold 1 char
    charsRead = recv( socketFD, clientChar, sizeof(char), 0 );
    if( charsRead < 0 ){
        error( "SERVER: ERROR reading client ID from socket" );
    }
    if( clientChar[ 0 ] != OTP_DEC_ID ){
        charsWritten = send( socketFD, OTP_BAD_CT, strlen( OTP_BAD_CT ), 0 );
        if( charsWritten < 0 ){
            error( "SERVER: ERROR writing bad connection error to socket" );
        }
        close( socketFD );  // Close down connection if client is unauthorized
        exit( 0 );          // Server can exit successfully here; client's 'fault' for making bad connect
    }

    // Get the total size of the message to determine buffer length
    // Credit: http://forums.codeguru.com/showthread.php?492913-Winsock-send()-Integers
    charsRead  = recv( socketFD, (char* )&msgSize, sizeof(int), 0 );
    if( charsRead < 0 ){
        error( "SERVER: ERROR reading message size from socket" );
    }

    // Instantiate buffers for message and key based on received length
    char* msgBuffer     = calloc( msgSize, sizeof(char) );
    char* txtBuffer     = calloc( msgSize / 2, sizeof(char) );
    char* keyBuffer     = calloc( msgSize / 2, sizeof(char) );
    char* plainBuffer   = calloc( msgSize / 2, sizeof(char) );

    // Read in plain text and key as one large message - necessary because breaking up into
    // segments removes our ability to differentiate until we have the whole thing.
    // Must provide the address of the current spot in the array on each iteration! Otherwise
    // multiple iterations will override the priors, never exceeding a CHUNK_SIZE segment in length.
    charsSum = 0;
    do{
        charsRead = recv( socketFD, &(msgBuffer[ charsSum ]), CHUNK_SIZE, 0 );
        charsSum += charsRead;
        if( charsRead < 0 ){
            error( "SERVER: ERROR reading message from socket" );
        }
    }while( charsSum < msgSize );

    // Split cipher text and key - we can assume they're each exactly half the message buffer,
    // and we know they were sent in order 1. cipher 2. key
    memcpy( txtBuffer, msgBuffer, (msgSize / 2) * sizeof(char) );
    memcpy( keyBuffer, &(msgBuffer[ msgSize / 2 ]), (msgSize / 2) * sizeof(char) );

    // Decrypt message with key, storing result in plain text buffer
    char* charList = CHAR_LIST;                                         // Our char list stored in otp.h as macro
    otpDecrypt( txtBuffer, keyBuffer, plainBuffer, charList );          // Decrypt
    // replaceNewline( plainBuffer );                                   // Put newline 'back' in place of '\0'

    // Send cipher back to client
    charsWritten = send( socketFD, plainBuffer, msgSize / 2, 0 );       // Send back w/ known size
    if( charsWritten < 0 ){
        error( "SERVER: ERROR writing plain text to socket" );
    }

    // Clean up
    free( clientChar );
    free( msgBuffer  );
    free( txtBuffer  );
    free( keyBuffer  );
    free( plainBuffer);

    // Exit successfully after all message properly received and sent
    close( socketFD );
    exit( 0 );
}
