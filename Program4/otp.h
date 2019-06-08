/* David LaMartina
 * lamartid@oregonstate.edu
 * Program 4: OTP Encryption & Decryption
 * CS344 Spr2019
 * Due June 9, 2019
 *
 * OTP helpers
 */

#ifndef OTP_H
#define OTP_H

#define CHAR_LIST           "ABCDEFGHIJKLMNOPQRSTUVWXYZ "   // List of valid characters for otp program
#define CHUNK_SIZE          1000                            // Size of byte chunks received

// Identifiers to be sent before messages to ensure correct programs connecting to daemons
#define OTP_ENC_ID          'e'
#define OTP_DEC_ID          'd'

// Bad connection flag -- sent from server to access-restricted client
#define OTP_BAD_CT          "b"     // String for sending
#define OTP_BAD_CT_RESPONSE 'b'     // Character for receiving

// Toggle for encryption / decryption in child process handling
enum OtpChoice { ENC, DEC };

// Encryption code
void otpEncrypt( char*, char*, char*, char* );

// Decryption code
void otpDecrypt( char*, char*, char*, char* );

// Child process handling for encryption and decryption daemons
void processConnection( int, enum OtpChoice );

#endif
