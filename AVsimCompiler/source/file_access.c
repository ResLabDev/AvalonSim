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

void WriteFile (const char * const p_path, char **pp_data, const int rows, bool b_addEOL)
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
        if (b_addEOL)
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

void WriteVerilogDefFile (const char * const p_path, char *p_define, char *p_subfolder, char *p_data, bool b_append)
{
    char fileAttribute[] = "w";

    if (b_append)
    {
        strcpy(fileAttribute, "a");
    }

    FILE * const p_file = fopen(p_path, fileAttribute);

    if (p_file == NULL)
    {
        perror("Error at output file opening.\n");
        return;
    }

    fprintf(p_file, "%s\"%s%s\"\n", p_define, p_subfolder, p_data);

    fclose(p_file);
}

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

