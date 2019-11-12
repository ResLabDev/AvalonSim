/** @file compile.h
*
* @brief Compiles the input text to Avalon Simulator format.
*
*/

#ifndef COMPILE_H
#define COMPILE_H

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "file_access.h"
#include "common.h"


// === Constant Definitions ===
//
#define NOP                 "NOP"
#define LOAD                "LOAD"
#define READ                "READ"
#define WRITE               "WRITE"
#define WAIT                "WAIT"
#define OPCODE_LIMIT        5
#define HEX_DATA_PATTERN    "00000000"
#define HEX_LIMIT           8
#define INVALID             'X'
#define INPUT_ERROR         '/'
#define INPUT_COMMENT       ';'
#define OUTPUT_COMMENT      "//"
#define OUTPUT_DELIM        '_'
#define INSTR_LIMIT         (1 + 2*HEX_LIMIT + 2)                       // 1_8_8 : opcode_address_data
#define COMMENT_LIMIT       (TEXT_BUFFER_LIMIT - INSTR_LIMIT - 8 - 4)   // 8: PC_REG_PATTERN; 4: OUTPUT_COMMENT
#define PC_REG_PATTERN      "/*000*/ "
#define PC_REG_LSD          4
#define PC_REG_MAX          999

// === Type Definitions ===
//
typedef enum
{
    nop = '0',
    read,
    write,
    wait,
    load
} opCodeType_t;

typedef struct instruction
{
    bool b_isValid;
    bool b_justComment;
    char opCode[OPCODE_LIMIT + 1];
    char address[HEX_LIMIT + 1];
    char data[HEX_LIMIT + 1];
    char comment[COMMENT_LIMIT + 1];
} instruction_t;

typedef struct opCode
{
    char *p_name;
    opCodeType_t value;
} opCode_t;

typedef enum
{
    zeroAddressData,
    zeroAddress,
    zeroData,
    fullAddressData,
    lshdAddress             // Least significant hexadecimal address
} hexType_t;

typedef struct addressDataFormat
{
    opCodeType_t opCode;
    hexType_t type;
} addressDataFormat_t;

// === Advanced Constant Definitions ===
//
static opCode_t const OP_CODES_LUT[] =
{
    { "NOP", nop     },
    { "READ", read   },
    { "WRITE", write },
    { "WAIT", wait   },
    { "LOAD", load   }
};

static addressDataFormat_t const ADDRESS_DATA_LUT[] =
{
    { nop, zeroAddressData   },
    { read, zeroData         },
    { write, fullAddressData },
    { wait, zeroAddress      },
    { load, lshdAddress      }
};

// === Macros ===
//


// === Public API Functions ===
//
char **CompileCode (char **pp_source, const textSize_t * const p_textParam); // MEMORY ALLOCATION

#endif // COMPILE_H

/*** EOF ***/

