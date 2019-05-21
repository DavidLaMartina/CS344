#ifndef COMMAND_EXEC_H
#define COMMAND_EXEC_H

#include "boolean.h"

// Execute user's non-built-in command
// Redirect input / output as specified
// Run process in foreground / background as specified
// Alter signal handling to cause child process to terminate itself upon SIGINT

void execCommand( char**,           // Parsed array of strings representing user's command
                  int*,             // Child exit status - will need to be altered so parent knows
                  struct sigaction, // SIGINT action - behavior changd for child proc vs. parent
                  enum boolean,     // Defines whether process will run in background
                  char*,            // Input file name
                  char* );          // Output file name

#endif