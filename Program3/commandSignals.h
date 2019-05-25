/* David LaMartina
 * lamartid@oregonstate.edu
 * CS344 Spr2019
 * Signal handling for Program 3: smallsh
 * Due May 26, 2019
 */

#ifndef COMMAND_SIGNALS_H
#define COMMAND_SIGNALS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "boolean.h"

extern enum boolean backgroundEnabled;

// SIGTSTP catchr
void catchSIGTSTP( int );

#endif
