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
