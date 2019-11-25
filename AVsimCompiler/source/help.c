/** @file help.c
*
* @brief For printing a short user guide.
*
*/

#include "help.h"

// === Protected Functions ===
//
static inline const char *GetGuide (void)
{
    static const char userGuide[] =
"\
========================================\n\
* Avalon MM Master Interface Simulator *\n\
*   Compiler User Guide                *\n\
========================================\n\
\n\
  I. Input arguments:\n\
       - (1) *.av source to be compiled [by default: \"instruction.av\"]\n\
       - (2) Verilog definition subfolder path [by default that is in the root]\n\
       - Compiled file output: \"<source>.mem\" stored in the root directory.\n\
       - Verilog definition file output: \"avsim_define.v\" stored in the root directory.\n\
  II. Acceptable Operating Codes (case-insensitive):\n\
       - 1. nop: No operation for an Avalon cycle\n\
       - 2. read: Reads the data from the specific address\n\
       - 3. write: Writes the data to the specific address\n\
       - 4. wait: Waiting until the specified cycles defined by the data\n\
       - 5. load: Loading the timing parameters (see later)\n\
  III. Input Source Format:\n\
      - 1. Instruction: <opcode> <hexadecimal address> <hexadecimal data>\n\
      - 2. Comment: ; <any comments>\n\
      - 3. Operating code, address and data must be separated by non-alphanumeric character e.g. white space.\n\
      - 4. Comment section is not mandatory at the end, use the ';' key if needed.\n\
      - 5. It is valid to use single line comment without instruction\n\
  IV. Limits:\n\
       - 1. Each line (including the comment) is limited to 100 character.\n\
       - 2. 4 Byte address and data in hexadecimal format.\n\
       - 3. Maximum value of program counter: 999.\n\
  V. Timing settings: 1 Byte format with the usage of LOAD operating code.\n\
       - 1. data: <Hold><ReadLatency><WriteWait><ReadWait> (MSB --> LSB)\n\
       - 2. address: 0x000000<Setup> (MSB --> LSB)\n\
       - 3. Example: \"load 11 2233aa01 ; setting avalon timing parameters\"\n\
              => Setup: 0x11, ReadWait: 0x01, WriteWait: 0xaa, ReadLatency: 0x33, Hold: 0x22\n\
  VI. Input Source Format Error Handling:\n\
      - 1. The specific line of the compiled output will be commented out in case of any source error.\n\
      - 2. Compiler is able to distinguish the 3 different type of errors: opcode, address, data.\n\
             There will be placed an 'X' key where the input error is occurred.\n\
  VII. Source Example:\n\
      ; Initialization\n\
      load 0 00020001 ; Timing parameters: ReadWait = 1, ReadLatency = 2\n\
      read 5 0        ; get module status\n\
      \n\
      ; Setting the module I/O data\n\
      write 0 1235fe  ; setting the dividend\n\
      write 1 a12     ; setting the divisor\n\
      write 2 1       ; starting the module\n\
      write 2 0\n\
      wait 0 5        ; waiting for completion\n\
      nop 0 0         ; wait one more cycle\n\
      \n\
      ; Obtaining the results\n\
      read 3 0        ; get quotient\n\
      read 4 a12      ; get remainder\n\
      \n\
      ; Integer Division Module Address Mapping\n\
      ;   0x00: 32-bit dividend (cpu write)\n\
      ;   0x01: 32-bit divisor (cpu write)\n\
      ;   0x02: start operation (cpu write)\n\
      ;   0x03: 32-bit quotient (cpu read)\n\
      ;   0x04: 32-bit remainder (cpu read)\n\
      ;   0x05: bit 0: 1-bit ready (cpu read)\n\
      ;   0x06: bit 0: 1-bit done_trg (cpu read/write)\n\
";

    return userGuide;
}


// === Public API Functions ===
//

bool IsHelpRequest (int argc, char **pp_argv)
{
    bool isHelp = false;

    if (argc > 1)
    {
        char argIn[strlen(pp_argv[1]) + 1];
        ToUpperCase(pp_argv[1], argIn);

        if (!strcmp(argIn, HELP))
        {
            printf("\n%s\n", GetGuide());
            isHelp = true;
        }
    }

    return isHelp;
}

/*** EOF ***/
