/* David LaMartina
 * lamartid@oregonstate.edu
 * Program 4: OTP Encryption & Decryption
 * CS344 Spr2019
 * Due June 9, 2019
 *
 * Encryption daemon
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "utilities.h"

#define MAX_CONNECTIONS     5

// Prototype for handling actual connections & encryptions in child procs
void handleConnection( int );

int main( int argc, char* argv[] )
{
    // Require port argument
    if ( argc < 2 ){
        error( "USAGE: otp_enc_d <port #>" );
    }

    // Daemon variables
    int listenSocketFD,                 // Socket to listen for client connections
        establishedConnectionFD,        // Socket for connecting to client
        portNumber;                     // Port to run server (this daemon)
    socklen_t sizeOfClientInfo;         // Amount of info coming from client
    struct sockaddr_in serverAddress,   // Daemon's identifier
                       clientAddress;   // Client's identifier
   
    // Process handling variables
    int numConnections  = 0;            // Current connections; cannot be greater than MAX_CONNECTIONS
    int childExitMethod = -5;           // Begin with nonsense value to check against

    // Set up address struct for this deamon
    memset( (char* )&serverAddress, '\0', sizeof( serverAddress ) );    // Clear address struct
    portNumber = atoi( argv[ 1 ] );                                     // Convert port number arg to int
    serverAddress.sin_family        = AF_INET;                          // Network-capable socket
    serverAddress.sin_port          = htons( portNumber );              // Store port number
    serverAddress.sin_addr.s_addr   = INADDR_ANY;                       // Any address allowed

    // Set up socket
    listenSocketFD = socket( AF_INET, SOCK_STREAM, 0 );                 // Create socket
    if( listenSocketFD < 0 ){
        error( "ERROR opening socket." );
    }

    // Enable socket to begin listening
    if ( bind( listenSocketFD,                                          // Connect socket to port
               (struct sockaddr *)&serverAddress,
               sizeof( serverAddress ) ) < 0 ){
        error( "ERROR on binding" );
    }
    listen( listenSocketFD, 5 );                                        // Flip socket on w/ queue of 5

    // Listen for client connections indefinitely
    while( 1 )
    {
        // Opening new connection depends on # existing connections;
        // Check number of child processes against max, updating as needed.
        while( waitpid( -1, &childExitMethod, WNOHANG ) > 0 ){
            printf( "A child died!\n" );
            numConnections--;
        }
        // If < max connections exist, accept next client connection
        if( numConnections < MAX_CONNECTIONS ){
            // Accept a connection, blocking if one is not available until one connects
            sizeOfClientInfo        = sizeof( clientAddress );          // Get size of client address
            establishedConnectionFD = accept( listenSocketFD,
                                              (struct sockaddr *)&clientAddress,
                                              &sizeOfClientInfo );      // Accept
            if( establishedConnectionFD < 0 ){
                error( "ERROR on accept" );
            }
            // Fork accepted connection
            pid_t spawnPID  = -5;                                        // start w/ nonsense to check against
            spawnPID        = fork();

            // Branch based on parent / child; execute daemon business logic in child
            switch( spawnPID ){

                case -1:                                                // Error & exit if spawn fails
                    error( "Hull breach!\n" );
                    break;

                case 0:                                                 // Child --> do business logic
                    handleConnection( establishedConnectionFD );
                    //printf( "You're in the child!\n" );
                    //exit( 0 );
                    break;

                default:                                                // Parent --> increment child count
                    numConnections++;
            }
        }
        // If max connectinos exist, re-loop when one exits
        else{
            if( waitpid( -1, &childExitMethod, 0 ) > 0 ){               // Blocks until exit
                numConnections++;                                       // Won't execute until waitpid does
            }
        }
    }

    printf( "We got our of here!\n" );
    close( listenSocketFD );                                            // Close listening socket
    return 0;
}

void handleConnection( int socketFD )
{
}
