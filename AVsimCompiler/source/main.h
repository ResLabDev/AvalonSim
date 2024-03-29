/** @file main.h
*
* @brief Initialization for MAIN entry point.
*
*/

#ifndef MAIN_H
#define MAIN_H

#include "file_access.h"
#include "compile.h"
#include "notify_invalid.h"
#include "help.h"


// === Testing ===
//
#define TEST_ON 0
#if TEST_ENABLE
# include "..\test\test.h"
#endif // TEST_ENABLE

// === Type Definitions ===
//


// === Constant Definitions ===
//
#define FILE_NAME_LENGTH_LIMIT      20
#define SOURCE_FILE_NAME            "instruction"
#define SOURCE_FILE_EXTENSION       ".av"
#define TARGET_FILE_EXTENSION       ".mem"
#define VERILOG_DEF                 "`define INSTRUCTION_PATH  "
#define VERILOG_DEF_FILE            "avsim_define.v"
#define RELEASE_DATE                "11-11-2019"
#define VERSION                     "v2"

// === Macros ===
//


#endif /* MAIN_H */

/*** EOF ***/

