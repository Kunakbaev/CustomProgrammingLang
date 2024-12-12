#ifndef LEXEM_ANALYSATOR_INCLUDE_LEXEM_ANALYSATOR_HPP
#define LEXEM_ANALYSATOR_INCLUDE_LEXEM_ANALYSATOR_HPP

#include "../../SyntaxTree/include/syntaxTreeNodeStruct.hpp"
#include "errorsHandler.hpp"

struct LexemAnalysator {
    const char*     sourceFilePath;
    const char*     destFilePath;
    size_t          arrLen;
    Lexem*          array;
    const char*     inputString;
    size_t          inputStringLen;
};

LexemAnalysatorErrors constructLexemAnalysator(const char* sourceFilePath,
                                               const char* destFilePath,
                                               LexemAnalysator* analysator);
LexemAnalysatorErrors dumpLexemAnalysator(LexemAnalysator* analysator);
LexemAnalysatorErrors openCurImageOfLexemAnalysator(LexemAnalysator* analysator);
LexemAnalysatorErrors processSourceFile(LexemAnalysator* analysator);
LexemAnalysatorErrors saveArrayOfLexems2File(LexemAnalysator* analysator);

#endif
