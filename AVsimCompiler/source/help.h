/** @file help.h
*
* @brief For storing a short user guide.
*
*/

#ifndef HELP_H
#define HELP_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "common.h"

// === Type Definitions ===
//


// === Constant Definitions ===
//
#define HELP    "HELP"

// === Macros ===
//


// === Public API Functions ===
//
/*!
* @brief Detect "help" request and print user guide if valid.
*
* @param[in]  argc Number of standard I/O arguments.
* @param[in]  pp_argv Standard I/O arguments.
*
* @return True, if "help" requested.
*/
bool IsHelpRequest (int argc, char **pp_argv);


#endif /* HELP_H */

/*** EOF ***/
