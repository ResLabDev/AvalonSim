/** @file file_access.h
*
* @brief Reading/writing an array of strings from/to a text file.
*
*/

#ifndef FILE_ACCESS_H
#define FILE_ACCESS_H

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// === Type Definitions ===
//
typedef struct textSize
{
    int bufferSize;
    int rowSize;
} textSize_t;

// === Constant Definitions ===
//
#define TEXT_BUFFER_LIMIT   100
#define EOL_CHAR            '\n'


// === Macros ===
//


// === Public API Functions ===
//
char ** const ReadFile (const char * const p_path, textSize_t * const p_textParam);           // MEMORY ALLOCATION
void WriteFile (const char * const p_path, char **pp_data, const int rows, bool addEOL);
void CleanupText (char **pp_stringArray, int rows);

#endif // FILE_ACCESS_H

/*** EOF ***/
