/** @file common.c
*
* @brief Implementation of commonly used API function collection.
*
*/

#include "common.h"


// === Protected Functions ===
//


// === Public API Functions ===
//
char *ToUpperCase (char * const p_source, char * const p_target)
{
    int i = 0;

    while (p_source[i])
    {
        p_target[i] = (char) toupper((int) p_source[i]);
        i++;
    }
    p_target[i] ='\0';

    return p_target;
}


/*** EOF ***/

