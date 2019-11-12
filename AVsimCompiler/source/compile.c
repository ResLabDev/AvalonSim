/** @file compile.c
*
* @brief Compiles the input text to Avalon Simulator format.
*
*/

#include "compile.h"

// === Protected Functions ===
//

//
/*!
* @brief Returns with TRUE, if the input character is alphanumeric
*
* @param[in] source The input character.
*
* @return True, if alphanumeric.
*/
static bool IsAlpha (const char source)
{
    bool b_ret = true;
    if ((source >= '0') && (source <= '9'))
    {
        // Valid
    }
    else if ((source >= 'A') && (source <= 'Z'))
    {
        // Valid
    }
    else if ((source >= 'a') && (source <= 'z'))
    {
        // Valid
    }
    else
    {
        b_ret = false;
    }

    return b_ret;
}

/*!
* @brief Splits the source string to the instruction parameters such as:
*           operating code, address, data and comment.
*
* @param[in] p_source Raw data instruction string.
* @param[out] p_instruction The splitted instruction result.
*
* @return Returns with true in case of success.
*/
static bool SplitInstruction (const char * const p_source, instruction_t * const p_instruction)
{
    enum stateFSM
    {
        st_OpCode,
        st_Address,
        st_Data,
        st_Comment
    };

    char buffer[TEXT_BUFFER_LIMIT + 1] = {'\0'};
    enum stateFSM state = st_OpCode;
    bool b_readInstr = false;
    bool b_nextState = false;
    bool b_isComment = false;

    int i = 0;
    int j = 0;
    bool alpha;
    while (p_source[i])
    {
        // Splitting the opcode, address and data
        //  handle comments different type via delimiter
        if (state != st_Comment)
        {
            if ( (alpha = IsAlpha(p_source[i])) )
            {
                buffer[j] = p_source[i];
                b_readInstr = true;
                j++;
            }
            else if (b_readInstr)
            {
                b_readInstr = false;
                b_nextState = true;
                buffer[j] = '\0';
                j = 0;
            }
            else if (INPUT_COMMENT == p_source[i])  // Just comment field is detected
            {
                p_instruction->b_justComment = true;
                state = st_Comment;
            }
            else
            {
                // Skipping the input delimiters
            }

        }

        // FSM for instruction splitting
        switch (state)
        {
            case st_OpCode:
                if (b_nextState)
                {
                    strcpy(p_instruction->opCode, buffer);
                    state = st_Address;
                    p_instruction->b_justComment = false;
                    b_nextState = false;
                }
            break;
            case st_Address:
                if (b_nextState)
                {
                    strcpy(p_instruction->address, buffer);
                    state = st_Data;
                    b_nextState = false;
                }
            break;
            case st_Data:
                if (b_nextState)
                {
                    strcpy(p_instruction->data, buffer);
                    state = st_Comment;
                    b_nextState = false;
                }
            break;
            case st_Comment:
                if (p_source[i] == INPUT_COMMENT)
                {
                    b_isComment = true;
                    break;
                }
                if (b_isComment)
                {
                    buffer[j] = p_source[i];
                    j++;
                    if (buffer[j] == '\0')
                    {
                        strcpy(p_instruction->comment, buffer);
                    }
                }
            break;
            default:
            break;
        }

        i++;
    }

    // No comment is detected
    if (!b_isComment)
    {
        strcpy(p_instruction->data, buffer);
        strcpy(p_instruction->comment, "\0");
    }

    return (state >= st_Data) ? true : false;
}

/*!
* @brief Converts the program counter integer to the proper string comment format.
*
* @param[out] p_pcReg Program counter output string.
* @param[in] p_instruction Instruction input for validity checking:
*               comments out the whole line in case of invalidity.
* @param[in] n Program counter integer value to be converted.
*
* @return void
*/
static void SetProgramCounter (char * const p_pcReg, const instruction_t * const p_instruction, int n)
{
    if (n > PC_REG_MAX)
    {
        sprintf(p_pcReg, "// > PC_REG_MAX");
        return;
    }
    // Convert input number to string
    p_pcReg[PC_REG_LSD] = (n % 10) + '0';       // Last significant digit
    int i = 1;
    while (n)
    {
        n /= (i * 10);
        p_pcReg[PC_REG_LSD-i] = (n % 10) + '0';
        i++;
    }

    // Comment out the Program Counter if invalid instruction is detected
    if (!p_instruction->b_isValid)
    {
        p_pcReg[1] = '/';
    }
}

/*!
* @brief Validates the operating code via LUT.
*
* @param[in] p_opcode Operating code string in case sensitive format.
*
* @return Returns with the IsValid value of the operating code.
*/
static bool ValidateOpCode (char * const p_opcode)
{
    ToUpperCase(p_opcode, p_opcode);

    for (int i = 0; i < (sizeof(OP_CODES_LUT) / sizeof(OP_CODES_LUT[0])); i++)
    {
        if (!strcmp(OP_CODES_LUT[i].p_name, p_opcode))  // Compare input with valid opcodes
        {
            return true;
        }
    }

    return false;
}

/*!
* @brief Validates the hexadecimal string data.
*
* @param[in] p_hexa Hexadecimal data string in case sensitive format.
*
* @return Returns with IsValid value.
*/
static bool ValidateHexa (char * const p_hexa)
{
    ToUpperCase(p_hexa, p_hexa);
    // 8 Byte hexadecimal size is out of range
    if (strlen(p_hexa) > strlen(HEX_DATA_PATTERN))
    {
        return false;
    }

    int i = 0;
    while (p_hexa[i])
    {
        if ((p_hexa[i] < '0') || (p_hexa[i] > 'F')) return false;
        if ((p_hexa[i] > '9') && (p_hexa[i] < 'A')) return false;
        i++;
    }

    return true;
}

/*!
* @brief Sets string invalid.
*
* @param[out] str
*
* @return void
*/
static inline void SetStrInvalid (char * const str)
{
    str[0] = INVALID;
    str[1] = '\0';
}

/*!
* @brief Validates the instruction parameters such as:
*           operating code, address, data and comment.
*
* @param[out] p_instruction The valid/invalid instruction result.
*
* @return Returns with the IsValid value of the instruction.
*/
static bool ValidateInstruction (instruction_t * const p_instruction)
{
    bool b_isValid = true;

    if (!ValidateOpCode(p_instruction->opCode))
    {
        SetStrInvalid(p_instruction->opCode);
        b_isValid = false;
    }

    if (!ValidateHexa(p_instruction->address))
    {
        SetStrInvalid(p_instruction->address);
        b_isValid = false;
    }

    if (!ValidateHexa(p_instruction->data))
    {
        SetStrInvalid(p_instruction->data);
        b_isValid = false;
    }

    p_instruction->b_isValid = b_isValid;

    return b_isValid;
}

/*!
* @brief Formats the valid hexadecimal address / data.
*
* @param[in] p_instruction Hexadecimal address / data string to be formatted.
*
* @return void
*/
static void FormatAddressData (instruction_t * const p_instruction)
{
    bool b_convertAddress = true;
    bool b_convertData = true;
    char hexa[] = HEX_DATA_PATTERN;
    int j;
    int hexLimit = (int) strlen(hexa);

    // Conversion logic depending on look-up table
    j = (int) p_instruction->opCode[0] - '0';
    switch (ADDRESS_DATA_LUT[j].type)
    {
        case zeroAddressData:
            b_convertAddress = false;
            b_convertData = false;
        break;
        case zeroAddress:
            b_convertAddress = false;
        break;
        case zeroData:
            b_convertData = false;
        break;
        case fullAddressData:
            // NOP
        break;
        case lshdAddress:
            b_convertAddress = false;
            int instructionAddressSize = strlen(p_instruction->address);
            hexa[HEX_LIMIT - 1] = p_instruction->address[instructionAddressSize - 1];
            if (instructionAddressSize > 1)
            {
                hexa[HEX_LIMIT - 2] = p_instruction->address[instructionAddressSize - 2];
            }
        break;
        default:
            // NOP
        break;
    }

    // Address conversion
    if (b_convertAddress)
    {
        j = 0;
        for (int i = hexLimit - strlen(p_instruction->address); i < hexLimit; i++)
        {
            hexa[i] = p_instruction->address[j];
            j++;
        }
    }
    strcpy(p_instruction->address, hexa);

    // Data conversion
    strcpy(hexa, HEX_DATA_PATTERN);
    if (b_convertData)
    {
        j = 0;
        for (int i = hexLimit - strlen(p_instruction->data); i < hexLimit; i++)
        {
            hexa[i] = p_instruction->data[j];
            j++;
        }
    }
    strcpy(p_instruction->data, hexa);
}

/*!
* @brief Converts the instruction to the valid format.
*
* @param[in] p_instruction Valid instruction to be formatted.
*
* @return void
*/
static void CompileInstruction (instruction_t * const p_instruction)
{
    // Compile Operating Code
    char opCode[2] = {'\0'};

    // Compile the Operating Code
    for (int i = 0; i < (sizeof(OP_CODES_LUT) / sizeof(OP_CODES_LUT[0])); i++)
    {
        if (!strcmp(OP_CODES_LUT[i].p_name, p_instruction->opCode))
        {
            opCode[0] = OP_CODES_LUT[i].value;
            strcpy(p_instruction->opCode, opCode);
        }
    }

    // Compile Hexadecimal address and data
    FormatAddressData(p_instruction);
}

// === Public API Functions ===
//
/*!
* @brief Compiles the input 1D string array to final Avalon Simulator format.
*
* @param[in] pp_source Raw data as string array to be compiled.
* @param[in] p_textParam Text parameters: maximum size of row, number of raw.
*
* @return MEMORY ALLOCATION: 1D string array with the fully compiled code.
*/
char **CompileCode (char **pp_source, const textSize_t * const p_textParam)
{
    char **pp_result = (char **) calloc(p_textParam->rowSize, sizeof(char *));
    if (pp_result == NULL)
    {
        perror("Unable to allocate memory for compilation results.");
        return NULL;
    }

    instruction_t instruction = {false, false, {'\0'}, {'\0'}, {'\0'}, {'\0'}};
    char converted[TEXT_BUFFER_LIMIT + 1] = {'\0'};
    int progCount = 0;
    char pcReg[] = PC_REG_PATTERN;

    for (int i = 0; i < p_textParam->rowSize; i++)
    {
        if (SplitInstruction(pp_source[i], &instruction))
        {
            if (!instruction.b_justComment) // Detect if line contains instructions not just comments
            {
                if (ValidateInstruction(&instruction))
                {
                    CompileInstruction(&instruction);
                }

                strcpy(pcReg, PC_REG_PATTERN);
                SetProgramCounter(pcReg, &instruction, progCount);
                sprintf(converted, "%s%s%c%s%c%s%c%s%s", pcReg, instruction.opCode,OUTPUT_DELIM,
                    instruction.address, OUTPUT_DELIM, instruction.data, ' ', OUTPUT_COMMENT, instruction.comment);

                if (instruction.b_isValid)
                {
                    progCount++;
                }
            }
            else
            {
                sprintf(converted, "%s%s", OUTPUT_COMMENT, instruction.comment);
            }
        }
        else
        {
            // Skip dummy data or simple new line
            strcpy(converted, "");
        }

        // Memory allocation for each lines
        pp_result[i] = (char *) calloc(strlen(converted) + 1, sizeof(char));
        strcpy(pp_result[i], converted);
    }

    return pp_result;
}

/*** EOF ***/
