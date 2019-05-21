/* David LaMartina
 * lamartid@oregonstate.edu
 * CS344 Spr2019
 * Signal handling for smallsh
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "boolean.h"
#include "utilities.h"
#include "commandSignals.h"

// Toggles backgroundEnabled to allow / disallow background processes to be run
void catchSIGTSTP( int signo )
{
    // If catchSIGTSP is ever called, backgroundEnabled will be defined within smallsh.c
    if( backgroundEnabled == TRUE ){
        const char* message = "Entering foreground-only mode (& is now ignored)\n";     // Required message
        printSafe( message, STDOUT_FILENO );                                            // re-entrant print
        backgroundEnabled = FALSE;                                                      // Toggle
    }
    else{
        const char* message = "Exiting foreground-only mode\n";                         // Required message
        printSafe( message, STDOUT_FILENO );                                            // re-entrant print
        backgroundEnabled = TRUE;                                                       // Toggle
    }
}
