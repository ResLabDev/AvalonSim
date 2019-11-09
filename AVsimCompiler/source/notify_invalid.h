/** @file notify_invalid.h
*
* @brief Checks for invalid compiles and notifies via std_err.
*
*/

#ifndef NOTIFY_INVALID_H
#define NOTIFY_INVALID_H

#include <stdio.h>
#include <stdbool.h>

#include "compile.h"

// === Type Definitions ===
//
typedef enum
{
    opCode,
    address,
    data
} error_t;

typedef struct
{
    char *p_message;
    error_t error;
} srcError;

// === Constant Definitions ===
//
#define ERROR_MSG   "=> ERROR at line"

static srcError const ERROR_LUT[] =
{
    { "OPCODE", opCode },
    { "ADDRESS", address },
    { "DATA", data },
};

// === Macros ===
//


// === Public API Functions ===
//
void NotifyInvalid (char **pp_source, const int length);

#endif // NOTIFY_INVALID_H

/*** EOF ***/
