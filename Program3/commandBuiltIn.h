/* David LaMartina
 * lamartid@oregonstate.edu
 * CS344 Spr2019
 * Built-in commands for Program 3: smallsh
 * Due May 26, 2019
 */

#ifndef COMMAND_BUILTIN_H
#define COMMAND_BUILTIN_H

// If user enters "cd" or "cd [ path ]"
void changeDirectory( char* );

// If user enters "status", prints appropriate exit status OR signal
void printStatus( int );

#endif
