/* David LaMartina
 * lamartid@oregonstate.edu
 * Utilities for Program 4
 * CS344 Spr2019
 */

#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "boolean.h"

// Simple utilities for reducing code in Program 3 main

// Set all top-level (input buffer lines) of string array to NULL
void nullifyStringArray( char** buffer, int bufSize );

// Free all top-level (input buffer lines) of string array
void freeStringArray( char** buffer, int bufSize );

// Check whether line can be ignored (blank or comment)
enum boolean canIgnore( char** buffer );

// Re-entrant output using write() and fflush()
void printSafe( const char* message, int location );

// Replace word (required to replace $$ with shell PID)
char* replaceWord( const char*, const char*, const char* );

// Determine number of digits in integer (Required for int - string conversion)
int numDigits( int );

// Convert integer to string for $$ expansion
char* intToString( int num );

// Check whether string is empty
enum boolean isStringEmpty( char* );

// Print error message
void error( const char* );

// Get number of characters in a file
int fileNumChars( const char* fileName );

#endif
