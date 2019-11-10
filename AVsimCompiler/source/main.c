/** @file main.c
*
* @brief The Avalon Compiler's entry point.
*
*/

#include <stdio.h>
#include <time.h>

#include "file_access.h"
#include "compile.h"
#include "notify_invalid.h"

#if TEST_ENABLE
# include "..\test\test.h"
#endif // TEST_ENABLE

#define TEST_ON 0
#define SOURCE_FILE "instruction.av"
#define TARGET_FILE "instruction.mem"

// === Protected Function Prototypes ===
//
static inline void StartDisplay (const char * const source, const char * const target);
static bool HandleArgument(int argc, char ** pp_argv, char **pp_source, char **pp_target);
static inline void PrintText (char ** const pp_source, int size);

// === MAIN ===
//
int main (int argc, char **pp_argv)
{
// Test is switched on
#if TEST_ON
#   ifdef TEST_ENABLE
    RunTest();
#   endif // TEST_ENABLE

// Test is switched off
#else
    char *p_sourceFile = NULL;
    char *p_targetFile = NULL;

    // Handle command line input parameters
    if (!HandleArgument(argc, pp_argv, &p_sourceFile, &p_targetFile))
    {
        return -1;
    }
    StartDisplay(p_sourceFile, p_targetFile);

    // Read source file
    textSize_t textParam;
    char ** const pp_source = ReadFile(p_sourceFile, &textParam);
    if (pp_source == NULL)
    {
        perror("No source file was detected.");
        return -1;
    }

    // Print source file to the console
    printf("--- The input source's raw data: '%s' ---\n", p_sourceFile);
    PrintText(pp_source, textParam.rowSize);

    // Compile the input
    char **pp_compiled = CompileCode(pp_source, &textParam);

    // Print the compiled code to the console
    printf("\n--- The compiled code: '%s' ---\n", p_targetFile);
    PrintText(pp_compiled, textParam.rowSize);

    //Write the compiled code to the target file
    WriteFile(p_targetFile, pp_compiled, textParam.rowSize, true);

    // Detect the invalid parameters and print to the console
    puts("");
    NotifyInvalid (pp_compiled, textParam.rowSize);

    // Dismiss previous memory allocations
    CleanupText(pp_source, textParam.rowSize);
    CleanupText(pp_compiled, textParam.rowSize);

#endif // TEST_ON

    return 0;
}

// === Protected Functions ===
//
/*!
* @brief Starting screen with release date and file infos.
*
* @param[in] p_sourcePath Source file path.
* @param[in] p_targetPath Target file path.
*
* @return void.
*/
static inline void StartDisplay (const char * const p_sourcePath, const char * const p_targetPath)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    puts("=================================");
    puts("| COMPILER FOR AVALON SIMULATOR |");
    printf("| %02d-%02d-%04d v2                 |\n", tm.tm_mon + 1, tm.tm_mday, tm.tm_year + 1900);
    puts("=================================\n");
    printf("Input file: '%s'\n", p_sourcePath);
    printf("Output file: '%s'\n\n", p_targetPath);
}

//
/*!
* @brief .
*
* @param[in] p_sourcePath Source file path.
* @param[in] p_targetPath Target file path.
*
* @return void.
*/
static bool HandleArgument(int argc, char ** pp_argv, char **pp_source, char **pp_target)
{
    *pp_source = SOURCE_FILE;
    *pp_target = TARGET_FILE;
    bool b_succes = true;

    switch (argc)
    {
        case 1:
            // NOP
        break;
        case 2:
            *pp_source = pp_argv[1];
        break;
        case 3:
            *pp_source = pp_argv[1];
            *pp_target = pp_argv[2];
        break;
        default:
            perror("Too many input arguments.");
            b_succes = false;
        break;
    }

    return b_succes;
}
/*!
* @brief Prints the 1D string array input to the standard output.
*
* @param[in] pp_source 1D string array input.
* @param[in] size Size of the string array.
*
* @return void.
*/
static inline void PrintText (char ** const pp_source, int size)
{
    for (int i = 0; i < size; i++)
    {
        printf("%d. %s\n", i + 1, pp_source[i]);
    }
}

/*** EOF ***/

