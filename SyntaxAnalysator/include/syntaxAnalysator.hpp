#ifndef SYNTAX_ANALYSATOR_INCLUDE_SYNTAX_ANALYSATOR_HPP
#define SYNTAX_ANALYSATOR_INCLUDE_SYNTAX_ANALYSATOR_HPP

#include "../../LexemsRealizations/include/lexemsRealizations.hpp"
#include "../../SyntaxTree/include/syntaxTree.hpp"
#include "syntaxAnalysatorErrorsHandler.hpp"

struct SyntaxAnalysator {
    const char*         sourceFilePath;
    const char*         destFilePath;
    Lexem*              arrOfLexems;
    size_t              lenOfArr;
    SyntaxTree          tree;
};

SyntaxAnalysatorErrors constructSyntaxAnalysator(const char* sourceFilePath,
                                                 const char* destFilePath,
                                                 SyntaxAnalysator* analysator);
SyntaxAnalysatorErrors readArrOfLexemsFromFile(SyntaxAnalysator* analysator);
SyntaxAnalysatorErrors generateSyntaxTree(SyntaxAnalysator* analysator);
SyntaxAnalysatorErrors saveSyntaxTree2File(SyntaxAnalysator* analysator);
SyntaxAnalysatorErrors destructSyntaxAnalysator(SyntaxAnalysator* analysator)

#endif
