#include "../include/codeGenerator.hpp"
#include "../include/commonFileStart.hpp"

const size_t MAX_LINE_LEN = 1 << 10;

char* line = NULL;

CodeGeneratorErrors constructCodeGenerator(const char* sourceFilePath,
                                           const char* destFilePath,
                                           Dumper* dumper,
                                           CodeGenerator* generator) {
    IF_ARG_NULL_RETURN(sourceFilePath);
    IF_ARG_NULL_RETURN(destFilePath);
    IF_ARG_NULL_RETURN(generator);

    *generator = {
        .sourceFilePath = sourceFilePath,
        .destFilePath   = destFilePath,
        .tree           = {},
        .dumper         = dumper
    };
    constructSyntaxTree(&generator->tree, generator->dumper);

    return CODE_GENERATOR_STATUS_OK;
}

CodeGeneratorErrors readCodeGeneratorSyntaxTreeFromFile(CodeGenerator* generator) {
    IF_ARG_NULL_RETURN(generator);

    readSyntaxTreeFromFile(&generator->tree, generator->sourceFilePath);

    return CODE_GENERATOR_STATUS_OK;
}

void clearLine() {
    for (int i = 0; i < MAX_LINE_LEN; ++i) {
        line[i] = '\0';
    }
}

#define ADD2BUFF(format, ...) \
    linePtr += snprintf(linePtr, MAX_LINE_LEN - (linePtr - line), format, ##__VA_ARGS__);

#define CLEAR_LINE() clearLine(), linePtr = line

CodeGeneratorErrors assemblerCodeForConst(const Node* node, char* linePtr) {
    IF_ARG_NULL_RETURN(node);
    IF_ARG_NULL_RETURN(linePtr);

    Lexem lexem = node->lexem;
    ADD2BUFF("push %.2f\n", lexem.doubleData);

    return CODE_GENERATOR_STATUS_OK;
}

CodeGeneratorErrors assemblerCodeForIdentificator(const Node* node, char* linePtr) {
    IF_ARG_NULL_RETURN(node);
    IF_ARG_NULL_RETURN(linePtr);

    Lexem lexem = node->lexem;
    //assert(false);
    ADD2BUFF("push %s\n", lexem.strRepr);

    return CODE_GENERATOR_STATUS_OK;
}

#include "delimsAsmDefines.cpp"
#include "keywordsAsmDefines.cpp"
#include "operatorsAsmDefines.cpp"

CodeGeneratorErrors recursiveGenerationOfAssemblerCode(const SyntaxTree* tree,
                                                       size_t curNodeInd, size_t depthInBlocksOfCode,
                                                       FILE* file) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(file);

    if (!curNodeInd)
        return CODE_GENERATOR_STATUS_OK;

    Node node = *getSyntaxTreeNodePtr(tree, curNodeInd);
    Lexem lexem = node.lexem;
    char* linePtr = line;
    //LOG_DEBUG_VARS(depthInBlocksOfCode);
    LOG_DEBUG_VARS(lexem.strRepr);

    if (lexem.lexemSpecificName == KEYWORD_INT_LEXEM)
        return CODE_GENERATOR_STATUS_OK;

    if (lexem.type == CONST_LEXEM_TYPE) {
        ADD_TABS();
        IF_ERR_RETURN(assemblerCodeForConst(&node, linePtr));
        PRINT();
        CLEAR_LINE();
        return CODE_GENERATOR_STATUS_OK;
    }
    if (lexem.type == IDENTIFICATOR_LEXEM_TYPE) {
        ADD_TABS();
        IF_ERR_RETURN(assemblerCodeForIdentificator(&node, linePtr));
        PRINT();
        CLEAR_LINE();
        return CODE_GENERATOR_STATUS_OK;
    }

    #define GENERAL_LEXEM_DEF(_, specName, ...) \
        case specName:                          \
            specName##_ASM_DEF();               \
            break;                              \

    switch (lexem.lexemSpecificName) {
        #include "../../LexemsRealizations/include/codeGen/allLexems.hpp"
        default:
            assert(false);
    }

    #undef GENERAL_LEXEM_DEF

    CLEAR_LINE();

    return CODE_GENERATOR_STATUS_OK;
}

CodeGeneratorErrors generateAssemblerCodeFromSyntaxTree(CodeGenerator* generator) {
    IF_ARG_NULL_RETURN(generator);

    FILE* file = fopen(generator->destFilePath, "w");
    IF_NOT_COND_RETURN(file != NULL, CODE_GENERATOR_FILE_OPENING_ERROR);

    line = (char*)calloc(MAX_LINE_LEN, sizeof(char));
    IF_NOT_COND_RETURN(line != NULL, CODE_GENERATOR_MEMORY_ALLOCATION_ERROR);

    recursiveGenerationOfAssemblerCode(&generator->tree, generator->tree.root, 0, file);

    fclose(file);

    return CODE_GENERATOR_STATUS_OK;
}

CodeGeneratorErrors destructCodeGenerator(CodeGenerator* generator) {
    IF_ARG_NULL_RETURN(generator);

    destructSyntaxTree(&generator->tree);

    return CODE_GENERATOR_STATUS_OK;
}

