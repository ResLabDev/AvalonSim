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
/*
** @brief Reading each lines to 1D array of strings from a text file.
*
* @param[in] p_path The path of the text file.
* @param[out] textSize parameters: bufferSize, rowSize.
*
* @return MEMORY ALLOCATION -> String array of text lines
*/
char ** const ReadFile (const char * const p_path, textSize_t * const p_textParam);           // MEMORY ALLOCATION

/*
** @brief Writing 1D array of strings to a text file.
*
* @param[in] p_path The path of the text file.
* @param[in] pp_data The string array to be written.
* @param[in] rows The size of the pp_data 1D array.
* @param[in] b_dEOL Add end of line character (e.g. '\n').
*
* @return void
*/
void WriteFile (const char * const p_path, char **pp_data, const int rows, bool b_addEOL);

/*
** @brief Writes the Verilog Definition File.
*
* @param[in] p_path The path of the text file.
* @param[in] p_define The definition string constant.
* @param[in] p_subfolder Verilog project subfolder path of the compiled code.
* @param[in] p_data The definition string value.
* @param[in] b_append Append file at the end.
*
* @return void
*/
void WriteVerilogDefFile (const char * const p_path, char *p_define, char *p_subfolder, char *p_data, bool b_append);

/*
** @brief Clean up of 1D string array memory allocation.
*
* @param[in] pp_stringArray The string array to be cleaned.
* @param[in] rows The size of the pp_stringArray 1D array.
*
* @return void
*/
void CleanupText (char **pp_stringArray, int rows);

#endif // FILE_ACCESS_H

/*** EOF ***/
