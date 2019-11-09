/** @file file_access.c
*
* @brief Reading/writing an array of strings from/to a text file.
*
*/

#include "file_access.h"

// === Protected Functions ===
//
/*
** @brief Calculates the maximum rowLength and rows in a text file.
*
* @param[in] p_path The path of the text file.
* @param[out] p_textParam Text size parameters: bufferSize, rowSize.
*
* @return void
*/
static void GetTextParam (char const * const p_path, textSize_t * const p_textParam)
{
    p_textParam->bufferSize = 0;
    p_textParam->rowSize = 0;
    FILE * const p_file = fopen(p_path, "r");

    if (p_file == NULL)
    {
        fprintf(stderr, "Unable to open file: %s.\n", p_path);
        return;
    }

    int currRowLenght;
    char textBuffer[TEXT_BUFFER_LIMIT + 2];     // + '\n' + '\0'
    while ( NULL != fgets (textBuffer, TEXT_BUFFER_LIMIT + 2, p_file))
    {
        p_textParam->rowSize++;
        currRowLenght = strlen(textBuffer);
        if (currRowLenght > p_textParam->bufferSize)
        {
            p_textParam->bufferSize = currRowLenght;
        }
    }

    fclose(p_file);
}

// === Public Functions ===
//
/*
** @brief Reading each lines to 1D array of strings from a text file.
*
* @param[in] p_path The path of the text file.
* @param[out] textSize parameters: bufferSize, rowSize.
*
* @return MEMORY ALLOCATION -> String array of text lines
*/
char ** const ReadFile (const char * const p_path, textSize_t * const p_textParam)
{
    FILE * const p_file  = fopen(p_path, "r");

    if (p_file == NULL)
    {
        fprintf(stderr, "Unable to open file: %s.\n", p_path);
        return NULL;
    }

    // Determining the line buffer lenght and the number of rows
    GetTextParam(p_path, p_textParam);
    if ((p_textParam->bufferSize == 0) || (p_textParam->rowSize == 0))
    {
        fprintf(stderr, "Unable to determine file size parameters: %s.\n", p_path);
        fclose(p_file);
        return NULL;
    }

    char ** const pp_getText = (char **) calloc(p_textParam->rowSize, sizeof (char *));
    char textBuffer[p_textParam->bufferSize + 2];

    int lastChar;
    int i = 0;
    while ((NULL != fgets (textBuffer, p_textParam->bufferSize + 2, p_file)) && (i < p_textParam->rowSize))
    {
        pp_getText[i] = (char *) calloc(strlen (textBuffer) + 1, sizeof(char));
        if (!pp_getText[i])
        {
            perror("Unable to allocate reading buffer memory.\n");
            return NULL;
        }
        // Remove the new line characters at the end
        lastChar = (int) (strlen(textBuffer) - 1);
        if (textBuffer[lastChar] == '\n')
        {
            textBuffer[lastChar] = '\0';
            // Decrement buffer size if containing new line character
            if ((lastChar + 1) == p_textParam->bufferSize)
            {
                p_textParam->bufferSize--;
            }
        }
        strcpy(pp_getText[i], textBuffer);
        i++;
    }

    fclose(p_file);

    return pp_getText;
}

/*
** @brief Writing 1D array of strings to a text file.
*
* @param[in] p_path The path of the text file.
* @param[in] pp_data The string array to be written.
* @param[in] rows The size of the pp_data 1D array.
* @param[in] addEOL Add end of line character (e.g. '\n').
*
* @return void
*/
void WriteFile (const char * const p_path, char **pp_data, const int rows, bool addEOL)
{
    FILE * const p_file = fopen(p_path, "w");

    if (p_file == NULL)
    {
        perror("Error at output file opening.\n");
        return;
    }

    int i;
    for (i = 0; i < rows; i++)
    {
        if (addEOL)
        {
            fprintf(p_file, "%s%c", pp_data[i], EOL_CHAR);
        }
        else
        {
            fprintf(p_file, "%s", pp_data[i]);
        }
    }

    fclose(p_file);
}

/*
** @brief Clean up of 1D string array memory allocation.
*
* @param[in] pp_stringArray The string array to be cleaned.
* @param[in] rows The size of the pp_stringArray 1D array.
*
* @return void
*/
void CleanupText (char **pp_stringArray, int rows)
{
    int i;
    for (i = 0; i < rows; i++)
    {
        if (NULL != pp_stringArray[i])
        {
            free(pp_stringArray[i]);
        }
    }

   free(pp_stringArray);
}

/*** EOF ***/

