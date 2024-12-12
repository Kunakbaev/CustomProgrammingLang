#include <ctype.h>
#include <errno.h>

#include "../include/errorsHandler.hpp"
#include "../include/lexemsRealizations.hpp"

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, LEXEM_REALIZATIONS_INVALID_ARGUMENT, getLexemsRealizationsErrorMessage)

#define IF_ERR_RETURN(error) \
    COMMON_IF_ERR_RETURN(error, getLexemsRealizationsErrorMessage, LEXEMS_REALIZATIONS_STATUS_OK);

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getLexemsRealizationsErrorMessage)\

const int baseLineNum = __LINE__;

// BRUUUUUUH
#define GENERAL_LEXEM_DEF(enumName, strRepr) \
    {.type=enumName, .strRepr=strRepr, {.data = __LINE__ - baseLineNum - (23 - 16)}}, // AHAHAHA, wtf is this, define magic

Lexem allLexemsArr[] = {
    #include "../include/codeGen/allLexems.hpp"
};

#undef GENERAL_LEXEM_DEF

const size_t LEXEM_ARR_SIZE = sizeof(allLexemsArr) / sizeof(*allLexemsArr);

// super slow TODO:
LexemsRealizationsErrors initLexemWithString(const char* line, Lexem* lexem) {
    IF_ARG_NULL_RETURN(line);
    IF_ARG_NULL_RETURN(lexem);

    for (int i = 0; i < LEXEM_ARR_SIZE; ++i) {
        bool isSame = strcmp(allLexemsArr[i].strRepr, line) == 0;
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
        lexem->data = -1;
        lexem->strRepr = line; // ASK: is this good?
        assert(false); // not implemented yet
        lexem->type = IDENTIFICATOR_LEXEM_TYPE;
    }

    return LEXEMS_REALIZATIONS_STATUS_OK;
}

isCharLexemDelim

LexemsRealizationsErrors saveLexemToFile(FILE* file, const Lexem* lexem) {
    IF_ARG_NULL_RETURN(file);
    IF_ARG_NULL_RETURN(lexem);

    fprintf(file, "%d", lexem->type);
    switch (lexem->type) {
        case CONST_LEXEM_TYPE:
            fprintf(file, "%f", lexem->doubleData);
        case IDENTIFICATOR_LEXEM_TYPE:
            fprintf(file, "%s", lexem->strRepr);
        case OPERATOR_LEXEM_TYPE:
            fprintf(file, "%d", lexem->data); // general lexem id
        default:
            // invalid lexem type
            assert(false);
    }

    return LEXEMS_REALIZATIONS_STATUS_OK;
}
