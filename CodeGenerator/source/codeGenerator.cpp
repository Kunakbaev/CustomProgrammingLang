#include "../include/codeGenerator.hpp"
#include "../include/commonFileStart.hpp"

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

CodeGeneratorErrors generateAssemblerCodeFromSyntaxTree(CodeGenerator* generator) {
    IF_ARG_NULL_RETURN(generator);

    return CODE_GENERATOR_STATUS_OK;
}

CodeGeneratorErrors destructCodeGenerator(CodeGenerator* generator) {
    IF_ARG_NULL_RETURN(generator);

    destructSyntaxTree(&generator->tree);

    return CODE_GENERATOR_STATUS_OK;
}

