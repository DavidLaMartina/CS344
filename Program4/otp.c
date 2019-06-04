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
