/** @file notify_invalid.c
*
* @brief Checks for invalid compiles and notifies via std error.
*
*/

#include "notify_invalid.h"

// === Protected Functions ===
//


// === Public API Functions ===
//
/*!
* @brief This function...
*
* @param[in] num1 pp_source Compiled string array to be checked.
* @param[in] num2 length The size of the 1D string array input.
*
* @return void
*/
void NotifyInvalid (char ** const pp_source, const int length)
{
    bool b_invalid = false;
    char detectError[] = INPUT_ERROR;
    int j = 0;
    int z = 0;

    for (int i = 0; i < length; i++)
    {
        snprintf(detectError, (sizeof(detectError) / sizeof(detectError[0])), "%s", pp_source[i]);
        // Detect Input Error - the whole input line is unacceptable
        if (!strcmp(detectError, INPUT_ERROR))
        {
            fprintf(stderr, "%s %d.: Unacceptable input error.\n", ERROR_MSG, i + 1);
            b_invalid = true;
        }
        // Instruction error, detect it
        else if (detectError[1] == INPUT_ERROR[1])
        {
            j = 0;
            z = 0;
            while (pp_source[i][j])
            {
                if (pp_source[i][j] == OUTPUT_DELIM)
                {
                    z++;
                }
                if (pp_source[i][j] == INVALID)
                {
                    fprintf(stderr, "%s %d.: Instruction '%s' error.\n", ERROR_MSG, i + 1, ERROR_LUT[z].p_message);
                    b_invalid = true;
                }
                j++;
            }
        }
    }

    if (!b_invalid)
    {
        perror("Successfully compiled without error.");
    }
}

/*** EOF ***/

