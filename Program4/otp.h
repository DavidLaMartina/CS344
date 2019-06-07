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

#define CHAR_LIST           "ABCDEFGHIJKLMNOPQRSTUVWXYZ "  // List of valid characters for otp program

// Identifiers to be sent before messages to ensure correct programs connecting to daemons
#define OTP_ENC_ID          'e'
#define OTP_DEC_ID          'd'

// Bad connection flag -- sent from server to access-restricted client
#define OTP_BAD_CT          "b"

// End-of-buffer identifying characters
#define OTP_TEXT_END        '@'
#define OTP_KEY_END         '#'

void otpEncrypt( char*, char*, char*, char* );

void otpDecrypt( char*, char*, char*, char* );

#endif
