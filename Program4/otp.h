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

#define CHAR_LIST_LENGTH    27

#define CHAR_LIST           "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

void otpEncrypt( char*, char*, char*, char* );

void otpDecrypt( char*, char*, char*, char* );

#endif
