/** @file main.c
*
* @brief The Avalon Compiler's entry point.
*
*/

#include <stdio.h>
#include <time.h>

#include "main.h"


// === Protected Function Prototypes ===
//
static inline void StartDisplay (const char * const p_sourcePath, const char * const p_targetPath, const char * const p_verilogDefPath);
static bool GeneratePathes (int argc, char **pp_argv, char *p_source, char *p_target, char *p_verilogWork);
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
    char sourceFile[FILE_NAME_LENGTH_LIMIT + 1] = {'\0'};
    char targetFile[FILE_NAME_LENGTH_LIMIT + 1] = {'\0'};
    char verilogWorkFolder[FILE_NAME_LENGTH_LIMIT + 1] = {'\0'};

    // Handle command line input parameters
    if (!GeneratePathes(argc, pp_argv, sourceFile, targetFile, verilogWorkFolder))
    {
        perror("Undefined I/O file pathes.");
        return -1;
    }
    StartDisplay(sourceFile, targetFile, VERILOG_DEF_FILE);

    // Create Verilog Definition File
    WriteVerilogDefFile(VERILOG_DEF_FILE, VERILOG_DEF, verilogWorkFolder, targetFile, false);

    // Read source file
    textSize_t textParam;
    char ** const pp_source = ReadFile(sourceFile, &textParam);
    if (pp_source == NULL)
    {
        perror("No source file was detected.");
        return -1;
    }

    // Print source file to the console
    printf("--- The input source's raw data: '%s' ---\n", sourceFile);
    PrintText(pp_source, textParam.rowSize);

    // Compile the input
    char **pp_compiled = CompileCode(pp_source, &textParam);

    // Print the compiled code to the console
    printf("\n--- The compiled code: '%s' ---\n", targetFile);
    PrintText(pp_compiled, textParam.rowSize);

    //Write the compiled code to the target file
    WriteFile(targetFile, pp_compiled, textParam.rowSize, true);

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
* @param[in] p_targetPath Target file path.
*
* @return void.
*/
static inline void StartDisplay (const char * const p_sourcePath, const char * const p_targetPath, const char * const p_verilogDefPath)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    puts("=================================");
    puts("| COMPILER FOR AVALON SIMULATOR |");
    printf("| %02d-%02d-%04d v2                 |\n", tm.tm_mon + 1, tm.tm_mday, tm.tm_year + 1900);
    puts("=================================\n");
    printf("Input file: '%s'\n", p_sourcePath);
    printf("Output file: '%s'\n", p_targetPath);
    printf("Verilog definition file: '%s'\n\n", p_verilogDefPath);
}

//
/*!
* @brief Generates the file path depending on the input argument
*
* @param[in]  argc Number of standard I/O arguments.
* @param[in]  pp_argv Standard I/O arguments.
* @param[out] p_source Source file path.
* @param[out] p_target Target file path.
* @param[out] p_verilogWork Verilog working subfolder.
*
* @return Valid, if each path is exists.
*/
static bool GeneratePathes (int argc, char **pp_argv, char *p_source, char *p_target, char *p_verilogWork)
{
    // Create defaults
    strcpy(p_source, SOURCE_FILE_NAME);
    strcpy(p_verilogWork, "");

    if (argc > 1)
    {
        // Set source path by 2nd input argument
        strcpy(p_source, pp_argv[1]);
        // Remove file extension if exists: ".*"
        int i = 0;
        while (p_source[i])
        {
            if (p_source[i] == '.')
            {
                p_source[i] = '\0';
                break;
            }

            i++;
        }

        // Set Verilog Working Subfolder by 3rd input argument
        if (argc >= 2)
        {
            snprintf(p_verilogWork, FILE_NAME_LENGTH_LIMIT, "%s/", pp_argv[2]);
        }

        if (argc > 2)
        {
            perror("Too many input arguments.");
        }
    }

    // Format source and compiled target code pathes
    snprintf(p_target, FILE_NAME_LENGTH_LIMIT, "%s%s", p_source, TARGET_FILE_EXTENSION);
    snprintf(p_source, FILE_NAME_LENGTH_LIMIT, "%s%s", p_source, SOURCE_FILE_EXTENSION);

    return ( (p_source == NULL) ||
             (p_target == NULL) ||
             (p_verilogWork == NULL) ) ? false : true;
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

