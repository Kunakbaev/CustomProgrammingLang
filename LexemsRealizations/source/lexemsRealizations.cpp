#include <ctype.h>
#include <errno.h>

#include "../include/errorsHandler.hpp"
#include "../include/lexemsRealizations.hpp"

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, LEXEMS_REALIZATIONS_INVALID_ARGUMENT, getLexemsRealizationsErrorMessage)

#define IF_ERR_RETURN(error) \
    COMMON_IF_ERR_RETURN(error, getLexemsRealizationsErrorMessage, LEXEMS_REALIZATIONS_STATUS_OK);

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getLexemsRealizationsErrorMessage)\

const int baseLineNum = __LINE__;

// BRUUUUUUH
#define GENERAL_LEXEM_DEF(lexemType, enumName, repr) \
    {.type=lexemType, .strRepr=repr, {.lexemSpecificName = enumName}},

Lexem allLexemsArr[] = {
    #include "../include/codeGen/allLexems.hpp"
};

#undef GENERAL_LEXEM_DEF



#define GENERAL_LEXEM_DEF(lexemType, enumName, repr) \
    #enumName,

const char* allLexemSpecNames[] = {
    #include "../include/codeGen/allLexems.hpp"
};

#undef GENERAL_LEXEM_DEF

const size_t LEXEM_ARR_SIZE = sizeof(allLexemsArr) / sizeof(*allLexemsArr);
const size_t ALL_LEXEMS_SPEC_NAMES = sizeof(allLexemSpecNames) / sizeof(*allLexemSpecNames);

// TODO: super slow
LexemsRealizationsErrors initLexemWithString(const char* line, Lexem* lexem) {
    IF_ARG_NULL_RETURN(line);
    IF_ARG_NULL_RETURN(lexem);

    LOG_DEBUG_VARS(LEXEM_ARR_SIZE, line);
    for (int i = 0; i < LEXEM_ARR_SIZE; ++i) {
        bool isSame = strcmp(allLexemsArr[i].strRepr, line) == 0;
        //LOG_DEBUG_VARS(i, allLexemsArr[i].strRepr, line, isSame);
        if (isSame) {
            *lexem = allLexemsArr[i];
            return LEXEMS_REALIZATIONS_STATUS_OK;
        }
    }



    // maybe this is const (number or literal) or identificator

    assert(strlen(line) > 0);
    if (isdigit(line[0])) { // this is const number
        char* endPtr = NULL;
        errno = 0;
        lexem->doubleData = strtod(line, &endPtr);
        assert(errno == 0);
        lexem->type = CONST_LEXEM_TYPE;
    } else {
        // TODO: add identificator to table of names
        lexem->lexemSpecificName = INVALID_LEXEM;
        lexem->strRepr = line; // ASK: is this good?
        assert(false); // not implemented yet
        lexem->type = IDENTIFICATOR_LEXEM_TYPE;
    }

    return LEXEMS_REALIZATIONS_STATUS_OK;
}

LexemsRealizationsErrors isCharLexemDelim(const char ch, bool* isDelim) {
    IF_ARG_NULL_RETURN(isDelim);

    *isDelim = false;
    char line[] = { ch, '\0' };
    for (int i = 0; i < LEXEM_ARR_SIZE; ++i) {
        bool isSame = strcmp(allLexemsArr[i].strRepr, line) == 0;
        if (isSame && allLexemsArr[i].type == DELIM_LEXEM_TYPE) { // second operand is just to be sure, it should not necessary as we check strings for equality
            *isDelim = true;
            return LEXEMS_REALIZATIONS_STATUS_OK;
        }
    }

    return LEXEMS_REALIZATIONS_STATUS_OK;
}

const char* getLexemTypeString(LexemType type) {
    #define helpDef(x) case x: return #x
    switch (type) {
        helpDef(INVALID_LEXEM_TYPE);
        helpDef(OPERATOR_LEXEM_TYPE);
        helpDef(KEYWORD_LEXEM_TYPE);
        helpDef(CONST_LEXEM_TYPE);
        helpDef(IDENTIFICATOR_LEXEM_TYPE);
        helpDef(DELIM_LEXEM_TYPE);
        default:
            return "INVALID_LEXEM_TYPE";
    }
}

LexemsRealizationsErrors initLexemFromFileFormat(LexemType type, const char* line, Lexem* lexem) {
    IF_ARG_NULL_RETURN(line);
    IF_ARG_NULL_RETURN(lexem);

    lexem->type = type;
    char* endPtr = NULL;
    errno = 0;
    switch (type) {
        case CONST_LEXEM_TYPE:
            char* strRepr = (char*)calloc(strlen(line), sizeof(char));
            IF_NOT_COND_RETURN(strRepr != NULL, LEXEMS_REALIZATIONS_MEMORY_ALLOCATION_ERROR);
            strcpy(strRepr, line);
            lexem->strRepr = strRepr;

            lexem->doubleData = strtod(line, &endPtr);
            assert(errno == 0);
            break;

        case IDENTIFICATOR_LEXEM_TYPE:



            break;

        case KEYWORD_LEXEM_TYPE:
        case DELIM_LEXEM_TYPE:
        case OPERATOR_LEXEM_TYPE:
            lexem->lexemSpecificName = (Lexems)strtol(line, &endPtr, 10);
            assert(errno == 0);
            break;

        case INVALID_LEXEM_TYPE:
        default:
            assert(false); // add error
            break;
    }

    return LEXEMS_REALIZATIONS_STATUS_OK;
}

LexemsRealizationsErrors saveLexemToFile(FILE* file, const Lexem* lexem) {
    IF_ARG_NULL_RETURN(file);
    IF_ARG_NULL_RETURN(lexem);

    fprintf(file, "%d\n", lexem->type);
    switch (lexem->type) {
        case CONST_LEXEM_TYPE:
            fprintf(file, "%f", lexem->doubleData);
            break;
        case IDENTIFICATOR_LEXEM_TYPE:
            fprintf(file, "%s", lexem->strRepr);
            break;
        case DELIM_LEXEM_TYPE:
        case KEYWORD_LEXEM_TYPE:
        case OPERATOR_LEXEM_TYPE:
            fprintf(file, "%d", lexem->lexemSpecificName); // general lexem id
            break;
        default:
            // invalid lexem type
            assert(false);
            break;
    }
    fprintf(file, "\n\n");

    return LEXEMS_REALIZATIONS_STATUS_OK;
}

LexemsRealizationsErrors getLexemDebugString(const Lexem* lexem, char** result) {
    IF_ARG_NULL_RETURN(lexem);
    IF_ARG_NULL_RETURN(result);

    const size_t DBG_STRING_LEN = 20;
    *result = (char*)calloc(DBG_STRING_LEN, sizeof(char));
    IF_NOT_COND_RETURN(*result != NULL, LEXEMS_REALIZATIONS_MEMORY_ALLOCATION_ERROR);

    switch (lexem->type) {
        case OPERATOR_LEXEM_TYPE:
        case DELIM_LEXEM_TYPE:
        case KEYWORD_LEXEM_TYPE:
            strcpy(*result, lexem->strRepr);
            break;
        case CONST_LEXEM_TYPE:
            snprintf(*result, DBG_STRING_LEN, "%f", lexem->doubleData);
            break;
        case IDENTIFICATOR_LEXEM_TYPE:
            strcpy(*result, lexem->strRepr);
            break;
        default:
            assert(false); // TODO: unknown lexem type
    }

    return LEXEMS_REALIZATIONS_STATUS_OK;
}
