#include <errno.h>

#include "../include/syntaxAnalysator.hpp"

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, SYNTAX_ANALYSATOR_INVALID_ARGUMENT, getSyntaxAnalysatorErrorMessage)

#define IF_ERR_RETURN(error) \
    COMMON_IF_ERR_RETURN(error, getSyntaxAnalysatorErrorMessage, SYNTAX_ANALYSATOR_STATUS_OK);\

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getSyntaxAnalysatorErrorMessage)\

#define DUMPER_ERR_CHECK(error) \
    COMMON_IF_SUBMODULE_ERR_RETURN(error, getDumperErrorMessage, DUMPER_STATUS_OK, SYNTAX_ANALYSATOR_DUMPER_ERROR);

#define LEXEMS_REALIZATIONS_ERR_CHECK(error) \
    COMMON_IF_SUBMODULE_ERR_RETURN(error, getLexemsRealizationsErrorMessage, LEXEMS_REALIZATIONS_STATUS_OK, SYNTAX_ANALYSATOR_LEXEMS_REALIZATIONS_ERROR);




const size_t MAX_NUM_OF_LINES_IN_INPUT = 1 << 10;



SyntaxAnalysatorErrors constructSyntaxAnalysator(const char* sourceFilePath,
                                                 const char* destFilePath,
                                                 SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(sourceFilePath);
    IF_ARG_NULL_RETURN(destFilePath);
    IF_ARG_NULL_RETURN(analysator);

    *analysator = {
        .sourceFilePath = sourceFilePath,
        .destFilePath   = destFilePath,
        .arrOfLexems    = NULL,
        .lenOfArr       = 0,
        .tree           = {},
    };

    return SYNTAX_ANALYSATOR_STATUS_OK;
}

size_t getNumOfLines(FILE* file) {
    assert(file != NULL);

    size_t ch = '?';
    size_t numOfLines = 0;
    while ((ch = fgetc(file)) != EOF) {
        numOfLines += ch == '\n';
    }
    rewind(file);

    return numOfLines; // ASK: +1?
}

SyntaxAnalysatorErrors readArrOfLexemsFromFile(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    FILE* file = fopen(analysator->sourceFilePath, "r");
    IF_NOT_COND_RETURN(file != NULL, SYNTAX_ANALYSATOR_FILE_OPENING_ERROR);

    size_t numOfLines = getNumOfLines(file);
    assert(numOfLines % 2 == 0);
    assert(numOfLines > 0);

    analysator->lenOfArr = numOfLines / 1;
    analysator->arrOfLexems = (Lexem*)calloc(analysator->lenOfArr, sizeof(Lexem));
    IF_NOT_COND_RETURN(analysator->arrOfLexems != NULL,
                       SYNTAX_ANALYSATOR_MEMORY_ALLOCATION_ERROR);

    size_t elemInd = 0;
    char lineBuffer[MAX_NUM_OF_LINES_IN_INPUT] = {};
    while (fgets(lineBuffer, MAX_NUM_OF_LINES_IN_INPUT, file)) {
        char* endPtr = 0;
        errno = 0;
        LexemType lexemType = (LexemType)strtol(lineBuffer, &endPtr, 10); // ASK: bad cast?
        assert(errno == 0);

        assert(elemInd < analysator->lenOfArr);
        analysator->arrOfLexems[elemInd] =
        ++elemInd;
    }

    return SYNTAX_ANALYSATOR_STATUS_OK;
}

SyntaxAnalysatorErrors generateSyntaxTree(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);



    return SYNTAX_ANALYSATOR_STATUS_OK;
}

SyntaxAnalysatorErrors saveSyntaxTree2File(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);



    return SYNTAX_ANALYSATOR_STATUS_OK;
}

SyntaxAnalysatorErrors destructSyntaxAnalysator(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    destructSyntaxTree(&analysator->tree);
    FREE(analysator->arrOfLexems);
    analysator->lenOfArr = 0;

    return SYNTAX_ANALYSATOR_STATUS_OK;
}
