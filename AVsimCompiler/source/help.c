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
      - 1. <opcode> <hexadecimal address> <hexadecimal data> ; <any comments>\n\
      - 2. Operating code, address and data must be separated by non-alphanumeric character e.g. white space.\n\
      - 3. Comment section is not mandatory at the end, use the ';' key if needed.\n\
  IV. Limits:\n\
       - 1. Each line (including the comment) is limited to 100 character.\n\
       - 2. 4 Byte address and data in hexadecimal format.\n\
  V. Timing settings: 1 Byte format with the usage of LOAD operating code.\n\
       - 1. data: <ReadWait><WriteWait><Hold><ReadLatency> (MSB --> LSB)\n\
       - 2. address: 0x000000<Setup> (MSB --> LSB)\n\
       - 3. Example: \"load 11 2233aaff ; setting avalon timing parameters\"\n\
              => Setup: 0x11, ReadWait: 0x22, WriteWait: 0x33, Hold: 0xaa, ReadLatency: 0xff\n\
  VI. Input Source Format Error Handling:\n\
      - 1. The specific line of the compiled output will be commented out in case of any source error.\n\
      - 2. Empty line is acceptable only once at the end of the file.\n\
      - 3. Compiler is able to distinguish the 3 different type of input mismatch: opcode, address, data.\n\
             There will be placed an 'X' key where the input error is occurred.\n\
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
