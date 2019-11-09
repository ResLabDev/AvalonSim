/** @file test.c
*
* @brief Testing the functionalities of the modules.
*
*/

#include "test.h"

// === Protected Functions ===
//
/*!
* @brief Prints the 1D string array input to the standard output
*
* @param[in] pp_source 1D string array input.
* @param[in] size Size of the string array.
*
* @return void.
*/
static inline void PrintText (char ** const pp_source, int size)
{
    int i;
    for (i=0; i < size; i++)
    {
        printf("%d. %s\n", i + 1, pp_source[i]);
    }
}

/*!
* @brief Reading File Test Procedure.
*
* @return void.
*/
static void FileReadTest (void)
{
    textSize_t testParam = {0, 0};
    char ** const pp_sourceText = ReadFile(TEST_SOURCE_FILE, &testParam);
    printf("--- Source File Reading Test '%s'| Maximum length of rows: %d; Number of rows: %d ---\n",
           TEST_SOURCE_FILE, testParam.bufferSize, testParam.rowSize);
    PrintText(pp_sourceText, testParam.rowSize);

    puts("");
    CleanupText(pp_sourceText, testParam.rowSize);
}

/*!
* @brief Compiling Test Procedure.
*
* @return void.
*/
static void CompileTest (void)
{
    textSize_t testParam = {0, 0};
    char ** const pp_sourceText = ReadFile(TEST_SOURCE_FILE, &testParam);
    printf("--- Compiling Test '%s'| Maximum length of rows: %d; Number of rows: %d ---\n",
           TEST_SOURCE_FILE, testParam.bufferSize, testParam.rowSize);

    char ** const pp_targetText = CompileCode(pp_sourceText, &testParam);
    PrintText(pp_targetText, testParam.rowSize);
    puts("");

    NotifyInvalid(pp_targetText, testParam.rowSize);

    CleanupText(pp_sourceText, testParam.rowSize);
    CleanupText(pp_targetText, testParam.rowSize);
}

// === Public API Functions ===
//
/*!
* @brief Test Setup Procedure.
*
* @return void.
*/
void RunTest (void)
{
    puts("=== Avalon Compiler Test is Runnning... ===\n");

    FileReadTest();
    CompileTest();

    puts("\n=== ...Avalon Compiler Test is Finished. ===");
}

/*** EOF ***/

