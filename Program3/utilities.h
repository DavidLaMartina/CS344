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
void printSafe( char* message, int location );

#endif
