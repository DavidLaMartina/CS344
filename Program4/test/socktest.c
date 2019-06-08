#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

void error( const char *msg ) { perror(msg); exit(1); }

int main(int argc, char *argv[])
{
    int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
    socklen_t sizeOfClientInfo;
    char buffer[ 256 ];
    struct sockaddr_in serverAddress, clientAddress;

    if( argc < 2 ) { fprintf( stderr, "USAGE: %s port\n", argv[ 0 ]); exit( 1 ); }

    // Set up address struct for this process
    memset( (char*)&serverAddress, '\0', sizeof( serverAddress ) );
    portNumber = atoi( argv[ 1 ] );
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port   = htons( portNumber );
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    listenSocketFD = socket( AF_INET, SOCK_STREAM, 0 );
    if( listenSocketFD < 0 ) error( "ERROR opening socket" );

    if( bind( listenSocketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress) ) < 0 )
        error( "ERROR on binding!" );
    listen( listenSocketFD, 5 );

    sizeOfClientInfo = sizeof( clientAddress

    int optVal;
    int optLen;
    char *optVal2;

    // set SO_REUSEADDR on a socket to true (1):
    optVal = 1;
    setsockopt(


    return 0;
}
