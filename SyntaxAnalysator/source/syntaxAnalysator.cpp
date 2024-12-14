#include <errno.h>

#include "../include/commonFileStart.hpp"


const size_t MAX_NUM_OF_LINES_IN_INPUT = 1 << 10;



SyntaxAnalysatorErrors constructSyntaxAnalysator(const char* sourceFilePath,
                                                 const char* destFilePath,
                                                 SyntaxAnalysator* analysator,
                                                 Dumper* dumper) {
    IF_ARG_NULL_RETURN(sourceFilePath);
    IF_ARG_NULL_RETURN(destFilePath);
    IF_ARG_NULL_RETURN(analysator);

    *analysator = {
        .sourceFilePath = sourceFilePath,
        .destFilePath   = destFilePath,
        .arrOfLexems    = NULL,
        .lenOfArr       = 0,
        .tree           = {},
        .dumper         = dumper
    };

    constructSyntaxTree(&analysator->tree, analysator->dumper);

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
    LOG_DEBUG_VARS(numOfLines);
    assert(numOfLines % 3 == 0);
    assert(numOfLines > 0);

    analysator->lenOfArr = numOfLines / 3;
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
        //LOG_DEBUG_VARS(lineBuffer, lexemType);

        assert(elemInd < analysator->lenOfArr);
        Lexem* lexem = &analysator->arrOfLexems[elemInd];

        fgets(lineBuffer, MAX_NUM_OF_LINES_IN_INPUT, file);
        assert(lineBuffer != NULL);

        //LOG_DEBUG_VARS(lineBuffer);
        initLexemFromFileFormat(lexemType, lineBuffer, lexem);
        //char* lexemDbgStr = NULL;
        //getLexemDebugString(lexem, &lexemDbgStr);
        //const char* type = getLexemTypeString(lexem->type);
        //LOG_DEBUG_VARS(type, lexemDbgStr, lexem->doubleData);
        //FREE(lexemDbgStr);
        ++elemInd;

        fgets(lineBuffer, MAX_NUM_OF_LINES_IN_INPUT, file); // reads empty line that separates 2 different lexems
    }

    return SYNTAX_ANALYSATOR_STATUS_OK;
}

SyntaxAnalysatorErrors dumpSyntaxAnalysatorArrOfLexems(const SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    LOG_DEBUG_VARS("---------------------");
    LOG_DEBUG_VARS("syntax analysator array of lexems:");
    for (size_t arrInd = 0; arrInd < analysator->lenOfArr; ++arrInd) {
        Lexem lexem = analysator->arrOfLexems[arrInd];

        char* lexemDbgStr = NULL;
        getLexemDebugString(&lexem, &lexemDbgStr);
        const char* type = getLexemTypeString(lexem.type);
        LOG_DEBUG_VARS(lexem.type, lexem.lexemSpecificName, type, lexemDbgStr, lexem.doubleData);
        FREE(lexemDbgStr);
    }
    LOG_DEBUG_VARS("---------------------");

    return SYNTAX_ANALYSATOR_STATUS_OK;
}

SyntaxAnalysatorErrors dumpSyntaxAnalysatorTreeInConsole(const SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    dumpSyntaxTreeInConsole(&analysator->tree);

    return SYNTAX_ANALYSATOR_STATUS_OK;
}

#include "generateSyntaxTree.cpp"

SyntaxAnalysatorErrors saveSyntaxTree2File(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);



    return SYNTAX_ANALYSATOR_STATUS_OK;
}

SyntaxAnalysatorErrors destructSyntaxAnalysator(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    for (size_t arrInd = 0; arrInd < analysator->lenOfArr; ++arrInd) {
        Lexem lexem = analysator->arrOfLexems[arrInd];
        if (lexem.type == IDENTIFICATOR_LEXEM_TYPE ||
            lexem.type == CONST_LEXEM_TYPE) {
            FREE(lexem.strRepr);
        }
    }

    destructSyntaxTree(&analysator->tree);
    FREE(analysator->arrOfLexems);
    analysator->lenOfArr = 0;

    return SYNTAX_ANALYSATOR_STATUS_OK;
}
