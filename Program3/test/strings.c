#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../commandInput.h"
#include "../utilities.h"

int main()
{
    char* original = "test$$this";
    char* old = "$$";
    char* new = "12345";

    char* newWord = replaceWord( original, old, new );
    printf( "Here's your old word: %s\n", original );
    printf( "Here's your new word: %s\n", newWord );
    free( newWord );

    printf( "\nNow testing a word with no replacements...\n" );
    original = "testThisWord";
    newWord = replaceWord( original, old, new );
    printf( "Here's the original: %s\n", original );
    printf( "Here's the new word: %s\n", newWord );
    free( newWord );

    printf( "\nNow testing a word with two replacements...\n" );
    original = "$$Test$$This";
    newWord = replaceWord( original, old, new );
    printf( "Here's the original: %s\n", original );
    printf( "Here's the new word: %s\n", newWord );
    free( newWord );

    printf( "\nNow testing a word with the replace at the end...\n" );
    original = "ThisThis$$";
    newWord = replaceWord( original, old, new );
    printf( "Here's the original: %s\n", original );
    printf( "Here's the new word: %s\n", newWord );
    free( newWord );

    printf( "\nNow testing with string literal passed directly into replaceWord...\n" );
    original = "Test$$This";
    newWord = replaceWord( original, "$$", new );
    printf( "Here's the original: %s\n", original );
    printf( "Here's the new word: %s\n", newWord );
    free( newWord );


    return 0;
}
