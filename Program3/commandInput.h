/* David LaMartina
 * lamartid@oregonstate.edu
 * Command line input retrieval and parsing for Program 3: smallsh
 * CS344 Spr2019
 * Due May 26, 2019
 */

#ifndef COMMAND_INPUT_H
#define COMMAND_INPUT_H

#include "boolean.h"

// Get entire command as array of strings in char** buffer
void getCommand( char**,
                 int,
                 enum boolean*,
                 char*,
                 char*,
                 int,
                 int );

// Fetch one uninterrupted line of text from user - to be freed by caller
char* getInputLine();

#endif
