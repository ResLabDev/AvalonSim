/** @file common.h
*
* @brief Commonly used API function collection.
*
*/

#ifndef COMMON_H
#define COMMON_H

#include <ctype.h>

// === Type Definitions ===
//


// === Constant Definitions ===
//


// === Macros ===
//


// === Public API Functions ===
//
/*!
* @brief Converts the input string to uppercase.
*
* @param[in] p_source Input string to be converted.
* @param[out] p_target Output converted string.
*
* @return Returns with p_target pointer.
*/
char *ToUpperCase (char * const p_source, char * const p_target);


#endif /* COMMON_H */

/*** EOF ***/
