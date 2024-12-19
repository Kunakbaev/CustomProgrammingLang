#ifndef SEMANTIC_CHECKER_INCLUDE_SEMANTIC_CHECKER_HPP
#define SEMANTIC_CHECKER_INCLUDE_SEMANTIC_CHECKER_HPP

#include "semanticCheckerErrorsHandler.hpp"
#include "../../SyntaxTree/include/syntaxTree.hpp"
#include "../../LexemsRealizations/include/lexemsRealizations.hpp"

enum IdentificatorType {
     INVALID_IDENTIFICATOR      = 0,
    VARIABLE_IDENTIFICATOR      = 1,
    FUNCTION_IDENTIFICATOR      = 2,
};

struct FunctionIdentificator {
    size_t          numOfLocalVars;
    size_t*         arrOfLocalVars;
    size_t          minRamIndex;
};

struct Identificator {
    size_t                      declNodeInd;
    Lexem                       lexem;
    IdentificatorType           type;
    Node*                       scopeNode;
    size_t                      arrInd;
    size_t                      localVarInd;
    FunctionIdentificator       function;
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
SemanticCheckerErrors getIdentificatorByLexem(
    const SemanticChecker* checker,
    const Lexem*           lexem,
    Identificator*         result
);
SemanticCheckerErrors dumpTableOfIdentificators(SemanticChecker* checker);
SemanticCheckerErrors destructSemanticChecker(SemanticChecker* checker);

#endif
