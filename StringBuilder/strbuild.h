/* David LaMartina
 * lamartid@oregonstate.edu
 * May 2019
 * C String Builder for easier string operations
 * Adapted from implementation at 
 * https://nachtimwald.com/2017/02/26/efficient-c-string-builder/
 */

#ifndef __STRBUILD_H__
#define __STRBUILD_H__

#include <stddef.h>

/* Mutable string / character array builder */
struct str_builder;
typedef struct str_builder str_builder_t;

// Instantiate & allocate initial memory
str_builder_t *str_builder_create(void);

// Deallocate
void str_builder_destroy(str_builder_t *sb);


#endif
