#ifndef SEMANTIC_CHECKER_INCLUDE_SEMANTIC_CHECKER_HPP
#define SEMANTIC_CHECKER_INCLUDE_SEMANTIC_CHECKER_HPP

#include "semanticCheckerErrorsHandler.hpp"
#include "../../SyntaxTree/include/syntaxTree.hpp"
#include "../../LexemsRealizations/include/lexemsRealizations.hpp"

enum IDENTIFICATOR_TYPE {
    INVALID                     = 0,
    VARIABLE_IDENTIFICATOR      = 1,
    FUNCTION_IDENTIFICATOR      = 2,
};

struct Identificator {
    size_t                      declNodeInd;
    Lexem                       lexem;
    IDENTIFICATOR_TYPE          type;
    Node*                       scopeNode;
};

struct SemanticChecker {
    size_t                      tableArrLen;
    Identificator*              tableOfVars;
    const SyntaxTree*           tree;
    size_t*                     tinArray;
    size_t*                     toutArray;
    size_t                      timer;
};

SemanticCheckerErrors constructSemanticChecker(SemanticChecker* checker, const SyntaxTree* tree);
SemanticCheckerErrors semanticCheckOfSyntaxTree(SemanticChecker* checker);
SemanticCheckerErrors buildTableOfIdentificators(SemanticChecker* checker);
SemanticCheckerErrors dumpTableOfIdentificators(SemanticChecker* checker);
SemanticCheckerErrors destructSemanticChecker(SemanticChecker* checker);

#endif
