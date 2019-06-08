/* David LaMartina
 * lamartid@oregonstate.edu
 * Program 4: OTP Encryption & Decryption
 * CS344 Spr2019
 * Due June 9, 2019
 *
 * OTP helpers
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "otp.h"            // Includes char* charList that contains all chars used (in order)
#include "utilities.h"
#include "boolean.h"

// Encrypt plainText using key -- store result in cipherText buffer
// Inputs must be char buffers containing only valid characters and null terminator (no newline)
void otpEncrypt( char* plainText, char* key, char* cipherText, char* charList )
{
    // Make sure key is at least as long as plainText
    if( strlen( plainText ) > strlen( key ) ){
        return;
    } 

    int i, j;               // Inner & outer loop variables
    int plainVal = 0,       // Plaintext value (index of charList)
        keyVal = 0,         // Key value (index of charlist)
        cipherVal = 0;      // Value to be written to cipher buffer

    // Loop through corresponding characters in plainText and key, using their combined
    // values to determine cipher character & write to cipher buffer
    for( i = 0; i < strlen( plainText ); i++ ){

        // Get index values of charList for plainText and key
        for( j = 0; j < strlen( charList ); j++ ){
            if( plainText[ i ] == charList[ j ] ){
                plainVal = j;
            }
            if( key[ i ] == charList[ j ] ){
                keyVal = j;
            }
        }
        // Calculate sum, ensure value not greater than length of charList
        cipherVal = plainVal + keyVal;
        while( cipherVal > strlen( charList ) ){
            cipherVal -= strlen( charList );
        }
        // Use modulo to get cipher value
        cipherVal %= strlen( charList );
        cipherText[ i ] = charList[ cipherVal ];
    }
}

// Decrypt cipherText using key -- store result in plainText buffer
// Inputs must be char buffers containing only valid characters and null terminator (no newline)
void otpDecrypt( char* cipherText, char* key, char* plainText, char* charList )
{
    // Make sure key is at least as long as cipherText
    if( strlen( cipherText ) > strlen( key ) ){
        return;
    }

    int i, j;               // Inner & outer loop variables
    int cipherVal = 0,      // Plaintext value (index of charList)
        keyVal    = 0,      // Key value (index of charlist)
        plainVal  = 0;      // Value to be written to cipher buffer

    // Loop through corresponding characters in cipherText and key, using their subtracted
    // values to determine plainText character & write to plainText buffer
    for( i = 0; i < strlen( cipherText ); i++ ){

        // Get index values of charList for cipherText and key
        for( j = 0; j < strlen( charList ); j++ ){
            if( cipherText[ i ] == charList[ j ] ){
                cipherVal = j;
            }
            if( key[ i ] == charList[ j ] ){
                keyVal = j;
            }
        }
        // Calculate difference, ensuring value is not negative
        plainVal = cipherVal - keyVal;
        while( plainVal < 0 ){
            plainVal += strlen( charList );
        }
        plainVal %= strlen( charList );
        plainText[ i ] = charList[ plainVal ];
    }    
}

// Child process handling for encryption and decryption daemons
void processConnection( int socketFD, enum OtpChoice EorD )
{
    int charsRead,          // Characters read from packet; must be checked against 0
        charsSum,           // Sum of characters from multiple packets
        charsWritten,       // Check to ensure all characters have been sent to client
        msgSize;            // Size of total message -- need to get this first!

    char correctClient;
    if( EorD == ENC )
        correctClient = OTP_ENC_ID;
    else
        correctClient = OTP_DEC_ID;

    // Check identity of client (We know first message sent is identifying character)
    // If wrong program, write error message to be interpreted for output on client side
    char* clientChar = calloc( 1, sizeof(char) );               // Need char buffer to hold 1 char
    charsRead = recv( socketFD, clientChar, sizeof(char), 0 );
    if( charsRead < 0 ){
        error( "SERVER: ERROR reading client ID from socket" );
    }
    if( clientChar[0] != correctClient ){
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
    char* returnBuffer  = calloc( msgSize / 2, sizeof(char) );

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

    // Split plain text and key - we can assume they're each exactly half the message buffer,
    // and we know they were sent in order 1. message 2. key
    memcpy( txtBuffer, msgBuffer, (msgSize / 2) * sizeof(char) );
    memcpy( keyBuffer, &(msgBuffer[ msgSize / 2 ]), (msgSize / 2) * sizeof(char) );

    // Encrypt or decrypt message with key, storing result in return buffer
    char* charList = CHAR_LIST;                                     // Out char list stored in otp.h as macro
    if( EorD == ENC )
        otpEncrypt( txtBuffer, keyBuffer, returnBuffer, charList );
    else
        otpDecrypt( txtBuffer, keyBuffer, returnBuffer, charList );

    // Send return buffer back to client
    charsWritten = send( socketFD, returnBuffer, msgSize / 2, 0 );  // Send back w/ known size
    if( charsWritten < 0 ){
        error( "SERVER: ERROR writing return buffer to socket" );
    }

    // Clean up
    free( clientChar    );
    free( msgBuffer     );
    free( txtBuffer     );
    free( keyBuffer     );
    free( returnBuffer  );

    // Exit successfully after all messages properly received and sent
    close( socketFD );
    exit( 0 );
}
