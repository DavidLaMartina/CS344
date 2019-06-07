/* David LaMartina
 * lamartid@oregonstate.edu
 * Project 4: OTP Encryption & Decryption
 * CS344 Spr2019
 * Due June 9, 2019
 *
 * File-handling helpers
 */

#ifndef TEXTHANDLE_H
#define TEXTHANDLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "boolean.h"

// Read in text from file to buffer
void readIn( char*, int, char* );

// Replace ending newline with null terminator
void stripNewline( char* );

// Replace ending newline with other character
void replaceNewline( char* );

// Replace first instance of one character with another
// Old 'character' must be passed in as string
void replaceChar( char*, char*, char );

// Validate characters in a buffer against set of acceptable characters
enum boolean validateChars( char*, int, char*, int );

#endif
