#include "../include/commonFileStart.hpp"

#define CONSTRUCT_NODE_WITH_LEXEM(lexem, left, right) \
    constructNodeWithKidsNoErrors(&analysator->tree, &lexem, left, right)

#define CONSTRUCT_NODE_WITH_LEXEM_WITH_ARG(analysator, lexem, left, right) \
    constructNodeWithKidsNoErrors(&analysator->tree, &lexem, left, right)

// const node doesn't have any children
#define NEW_CONST_LEXEM_NODE(lexemArg) \
    constructNodeWithKidsNoErrors(&analysator->tree, &lexemArg, 0, 0)

// TODO: for now this is same
#define NEW_IDENTIFICATOR_NODE(lexemArg, left, right) \
    constructNodeWithKidsNoErrors(&analysator->tree, &lexemArg, left, right)

#define SET_NEW_ROOT(newRoot) \
    analysator->tree.root = newRoot

#define ANALYSATOR_ROOT analysator->tree.root
#define MOVE_CUR_LEX_PTR() ++analysator->curLexemInd


SyntaxAnalysatorErrors parseGrammar(SyntaxAnalysator* analysator);
SyntaxAnalysatorErrors parseBlockOfCode(SyntaxAnalysator* analysator);
SyntaxAnalysatorErrors parseSingleCommandLine(SyntaxAnalysator* analysator);

SyntaxAnalysatorErrors parseVariablesDeclaration(SyntaxAnalysator* analysator);
SyntaxAnalysatorErrors parseWhileLoop(SyntaxAnalysator* analysator);
SyntaxAnalysatorErrors parseIfKeyword(SyntaxAnalysator* analysator);
SyntaxAnalysatorErrors parseInputKeyword(SyntaxAnalysator* analysator);
SyntaxAnalysatorErrors parseOutputKeyword(SyntaxAnalysator* analysator);

SyntaxAnalysatorErrors parseReturnKeyword(SyntaxAnalysator* analysator);
SyntaxAnalysatorErrors parseFunctionDeclaration(SyntaxAnalysator* analysator);
SyntaxAnalysatorErrors parseFunctionCall(SyntaxAnalysator* analysator);

// no functions that are called inside boolean condition, only vars, consts and simple identificators
SyntaxAnalysatorErrors parseBooleanCondition(SyntaxAnalysator* analysator);

SyntaxAnalysatorErrors parseAssignOperator(SyntaxAnalysator* analysator);
SyntaxAnalysatorErrors parseCompareOperator(SyntaxAnalysator* analysator);
SyntaxAnalysatorErrors parseAdditionSubtractionOperators(SyntaxAnalysator* analysator);
SyntaxAnalysatorErrors parseMultiplicationDivisionOperators(SyntaxAnalysator* analysator);
SyntaxAnalysatorErrors parseSimpleBracketDelim(SyntaxAnalysator* analysator);
SyntaxAnalysatorErrors parseTerminal(SyntaxAnalysator* analysator);



typedef SyntaxAnalysatorErrors (*parserFunctionPtr)(SyntaxAnalysator* analysator);
typedef bool (*doesLexemSatisfyRuleFuncPtr) (Lexem lexem); // FIXME: Maybe better to add pointer



Lexem getCurrentLexem(const SyntaxAnalysator* analysator) {
    assert(analysator != NULL);
    assert(analysator->curLexemInd < analysator->lenOfArr);

    return analysator->arrOfLexems[analysator->curLexemInd];
}


// TODO: add codegen for such functions

#define GET_CUR_LEXEM() \
    getCurrentLexem(analysator)

#define GENERAL_LEXEM_DEF(lexemType, lexemSpecName, _)                                          \
    bool isLexem_##lexemSpecName(const Lexem* lexem) {                                          \
        assert(lexem      != NULL);                                                             \
                                                                                                \
        return lexem->type              == lexemType &&                                         \
               lexem->lexemSpecificName == lexemSpecName;                                       \
    }                                                                                           \
    bool isCurLexem_##lexemSpecName(const SyntaxAnalysator* analysator) {                       \
        Lexem curLexem = getCurrentLexem(analysator);                                           \
        return isLexem_##lexemSpecName(&curLexem); /* ASK: */                                   \
    }

#include "../../LexemsRealizations/include/codeGen/allLexems.hpp"

#undef GENERAL_LEXEM_DEF

#include "../../LexemsRealizations/include/lexemsRealizations.hpp"

// generator of bigass names for funcs
#define GENERAL_LEXEM_DEF(lexemType, lexemSpecName, _) \
    /*consructs new node with specified lexem and given children and than sets it as a new root of syntax tree*/\
    void setNew_##lexemSpecName##_nodeAsRoot(SyntaxAnalysator* analysator, size_t left, size_t right) {\
        size_t node = CONSTRUCT_NODE_WITH_LEXEM_WITH_ARG(analysator, SAMPLE_##lexemSpecName##_NODE, left, right);\
        SET_NEW_ROOT(node);\
    }\

#include "../../LexemsRealizations/include/codeGen/allLexems.hpp"

#undef GENERAL_LEXEM_DEF

#define NEW_OPERATOR_LEXEM_NODE CONSTRUCT_NODE_WITH_LEXEM

#define REQUIRE_LEXEM(lexem) \
    IF_NOT_COND_RETURN(isCurLexem_##lexem(analysator), SYNTAX_ANALYSATOR_INVALID_ARGUMENT)

#define REQUIRE_IDENTIFICATOR_LEXEM(lexem) \
    IF_NOT_COND_RETURN(GET_CUR_LEXEM().type == IDENTIFICATOR_LEXEM_TYPE, SYNTAX_ANALYSATOR_INVALID_ARGUMENT)

#define REQUIRE_CONST_LEXEM(lexem) \
    IF_NOT_COND_RETURN(lexem.type == CONST_LEXEM_TYPE, SYNTAX_ANALYSATOR_INVALID_ARGUMENT)

bool isCurLexemSameAsGiven(const SyntaxAnalysator* analysator, const Lexem* lexem) {
    assert(analysator != NULL);
    assert(lexem != NULL);

    Lexem cur = GET_CUR_LEXEM();
    return cur.type              == lexem->type &&
           cur.lexemSpecificName == lexem->lexemSpecificName;
}

#define IS_CUR_LEXEM_SAME_AS_GIVEN(lexem) \
    isCurLexemSameAsGiven(analysator, &lexem)

SyntaxAnalysatorErrors parseGrammar(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    IF_ERR_RETURN(parseBlockOfCode(analysator));
    IF_NOT_COND_RETURN(analysator->curLexemInd == analysator->lenOfArr,
                       SYNTAX_ANALYSATOR_INVALID_ARGUMENT); // TODO: add error

    return SYNTAX_ANALYSATOR_STATUS_OK;
}

SyntaxAnalysatorErrors parseBlockOfCode(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    if (!isCurLexem_DELIMS_OPEN_CURLY_BRACKET_LEXEM(analysator)) {
        IF_ERR_RETURN(parseSingleCommandLine(analysator));
        return SYNTAX_ANALYSATOR_STATUS_OK;
    }

    // ASK: what happens with empty block of code?
    MOVE_CUR_LEX_PTR();
    // maybe move to common parser
    size_t leftOperand = 0;
    while (analysator->curLexemInd < analysator->lenOfArr &&
           !isCurLexem_DELIMS_CLOSE_CURLY_BRACKET_LEXEM(analysator)) {
        //MOVE_CUR_LEX_PTR();
        IF_ERR_RETURN(parseBlockOfCode(analysator));
        size_t rightOperand = ANALYSATOR_ROOT;

        LOG_DEBUG_VARS(GET_CUR_LEXEM().strRepr);
        REQUIRE_LEXEM(DELIMS_SEMICOLON_LEXEM);
        MOVE_CUR_LEX_PTR();

        LOG_ERROR("block of codes parse");
        if (!leftOperand)
            leftOperand = rightOperand, rightOperand = 0;

        //SET_NEW_ROOT(CONSTRUCT_NODE_WITH_LEXEM(SAMPLE_DELIMS_SEMICOLON_LEXEM_NODE, leftOperand, rightOperand));
        setNew_DELIMS_SEMICOLON_LEXEM_nodeAsRoot(analysator, leftOperand, rightOperand);
        openImageOfCurrentStateSyntaxTree(&analysator->tree);
        leftOperand = ANALYSATOR_ROOT;
    }

    Lexem lexem = getCurrentLexem(analysator);
    LOG_DEBUG_VARS(lexem.strRepr, lexem.lexemSpecificName, lexem.type);
    REQUIRE_LEXEM(DELIMS_CLOSE_CURLY_BRACKET_LEXEM);
    MOVE_CUR_LEX_PTR();

    Lexem blockOfCodeLexem = {.type = DELIM_LEXEM_TYPE, .strRepr = "{}", {.lexemSpecificName = DELIMS_OPEN_CURLY_BRACKET_LEXEM}};
    setNew_DELIMS_OPEN_CURLY_BRACKET_LEXEM_nodeAsRoot(analysator, leftOperand, 0);

    return SYNTAX_ANALYSATOR_STATUS_OK;
}




#define TRY_PARSER_FUNC(function)                               \
    do {                                                        \
        size_t wasLexemInd = analysator->curLexemInd;           \
        size_t wasRoot     = analysator->tree.root;             \
        SyntaxAnalysatorErrors err = function(analysator);      \
        if (err == SYNTAX_ANALYSATOR_STATUS_OK)                 \
            return SYNTAX_ANALYSATOR_STATUS_OK;                 \
        analysator->tree.root = wasRoot;                        \
        analysator->curLexemInd = wasLexemInd;                  \
    } while (0)


SyntaxAnalysatorErrors parseSingleCommandLine(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    TRY_PARSER_FUNC(parseFunctionDeclaration);
    TRY_PARSER_FUNC(parseFunctionCall);
    TRY_PARSER_FUNC(parseReturnKeyword);
    // openImageOfCurrentStateSyntaxTree(&analysator->tree);
    // openImageOfCurrentStateSyntaxTree(&analysator->tree);
    TRY_PARSER_FUNC(parseVariablesDeclaration);

    TRY_PARSER_FUNC(parseAssignOperator);

    TRY_PARSER_FUNC(parseWhileLoop);
    TRY_PARSER_FUNC(parseIfKeyword);

    TRY_PARSER_FUNC(parseInputKeyword);
    TRY_PARSER_FUNC(parseOutputKeyword);

    return SYNTAX_ANALYSATOR_INVALID_ARGUMENT;
}

typedef bool (isCurLexemEq2lexemFuncPtr)(const SyntaxAnalysator* analysator);

static SyntaxAnalysatorErrors parseSequenceOfLexemsDelimetedByComma(
    SyntaxAnalysator*               analysator,
    parserFunctionPtr               nextParseFuncPtr,
    isCurLexemEq2lexemFuncPtr       isCurLexemFinal,
    size_t*                         resultNode
) {
    IF_ARG_NULL_RETURN(analysator);
    IF_ARG_NULL_RETURN(nextParseFuncPtr);
    IF_ARG_NULL_RETURN(isCurLexemFinal);
    IF_ARG_NULL_RETURN(resultNode);

    *resultNode = 0;
    while (analysator->curLexemInd < analysator->lenOfArr &&
           !(*isCurLexemFinal)(analysator)) {
        IF_ERR_RETURN((*nextParseFuncPtr)(analysator));
        size_t rightOperand = ANALYSATOR_ROOT;

        if (!(*resultNode))
            *resultNode = rightOperand, rightOperand = 0;

        if (!(*isCurLexemFinal)(analysator)) {
            REQUIRE_LEXEM(DELIMS_COMMA_LEXEM);
            MOVE_CUR_LEX_PTR();
        }

        setNew_DELIMS_COMMA_LEXEM_nodeAsRoot(analysator, *resultNode, rightOperand);
        *resultNode = ANALYSATOR_ROOT;
    }

    return SYNTAX_ANALYSATOR_STATUS_OK;
}

SyntaxAnalysatorErrors parseVariablesDeclaration(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    REQUIRE_LEXEM(KEYWORD_INT_LEXEM);
    MOVE_CUR_LEX_PTR();

    size_t varDeclOperand = 0;
    IF_ERR_RETURN(parseSequenceOfLexemsDelimetedByComma(
        analysator, parseTerminal, isCurLexem_DELIMS_SEMICOLON_LEXEM, &varDeclOperand));
    IF_NOT_COND_RETURN(varDeclOperand != 0, SYNTAX_ANALYSATOR_INVALID_ARGUMENT); // TODO:

    setNew_KEYWORD_INT_LEXEM_nodeAsRoot(analysator, varDeclOperand, 0);

    openImageOfCurrentStateSyntaxTree(&analysator->tree);

    return SYNTAX_ANALYSATOR_STATUS_OK;
}

SyntaxAnalysatorErrors parseFuncArgs(
    SyntaxAnalysator*       analysator,
    parserFunctionPtr       nextParseFuncPtr,
    size_t*                 resultNode
) {
    IF_ARG_NULL_RETURN(analysator);
    IF_ARG_NULL_RETURN(nextParseFuncPtr);

    REQUIRE_LEXEM(DELIMS_OPEN_SIMPLE_BRACKET_LEXEM);
    MOVE_CUR_LEX_PTR();

    IF_ERR_RETURN(parseSequenceOfLexemsDelimetedByComma(
        analysator, nextParseFuncPtr, isCurLexem_DELIMS_CLOSE_SIMPLE_BRACKET_LEXEM, resultNode));

    REQUIRE_LEXEM(DELIMS_CLOSE_SIMPLE_BRACKET_LEXEM);
    MOVE_CUR_LEX_PTR();

    return SYNTAX_ANALYSATOR_STATUS_OK;
}








SyntaxAnalysatorErrors parseOneFuncArg(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    REQUIRE_LEXEM(KEYWORD_INT_LEXEM); // type of argument
    MOVE_CUR_LEX_PTR();

    REQUIRE_IDENTIFICATOR_LEXEM(); // name of argument, it should be identificator, not const
    parseTerminal(analysator);

    size_t varOperand = ANALYSATOR_ROOT;
    setNew_KEYWORD_INT_LEXEM_nodeAsRoot(analysator, varOperand, 0);

    return SYNTAX_ANALYSATOR_STATUS_OK;
}

SyntaxAnalysatorErrors parseReturnKeyword(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    REQUIRE_LEXEM(KEYWORD_RETURN_LEXEM);
    MOVE_CUR_LEX_PTR();

    size_t resultOperand = 0;
    if (!isCurLexem_DELIMS_SEMICOLON_LEXEM(analysator)) {
        IF_ERR_RETURN(parseAssignOperator(analysator));
        resultOperand = ANALYSATOR_ROOT;
    }

    setNew_KEYWORD_RETURN_LEXEM_nodeAsRoot(analysator, resultOperand, 0);

    return SYNTAX_ANALYSATOR_STATUS_OK;
}

SyntaxAnalysatorErrors parseFunctionDeclaration(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    LOG_ERROR("parse func declaration");
    REQUIRE_LEXEM(KEYWORD_INT_LEXEM); // return type
    //Lexem returnTypeLex = GET_CUR_LEXEM();
    MOVE_CUR_LEX_PTR();

    REQUIRE_IDENTIFICATOR_LEXEM(); // name of func
    Lexem funcNameLexem = GET_CUR_LEXEM();
    MOVE_CUR_LEX_PTR();

    size_t argsOperand = 0;
    IF_ERR_RETURN(parseFuncArgs(analysator, parseOneFuncArg, &argsOperand));

    IF_ERR_RETURN(parseBlockOfCode(analysator));
    size_t blockOfCodeOperand = ANALYSATOR_ROOT;

    SET_NEW_ROOT(NEW_IDENTIFICATOR_NODE(funcNameLexem, argsOperand, blockOfCodeOperand));
    setNew_KEYWORD_INT_LEXEM_nodeAsRoot(analysator, ANALYSATOR_ROOT, 0);

    return SYNTAX_ANALYSATOR_STATUS_OK;
}

SyntaxAnalysatorErrors parseFunctionCall(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    Lexem curLexem = GET_CUR_LEXEM();
    if (curLexem.type != IDENTIFICATOR_LEXEM_TYPE)
        return SYNTAX_ANALYSATOR_INVALID_ARGUMENT;
    MOVE_CUR_LEX_PTR();

    size_t argsOperand = 0;
    IF_ERR_RETURN(parseFuncArgs(analysator, parseCompareOperator, &argsOperand));

    LOG_ERROR("parse func call");
    SET_NEW_ROOT(NEW_IDENTIFICATOR_NODE(curLexem, argsOperand, 0));
    openImageOfCurrentStateSyntaxTree(&analysator->tree);

    return SYNTAX_ANALYSATOR_STATUS_OK;
}

SyntaxAnalysatorErrors parseOutputKeyword(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    REQUIRE_LEXEM(KEYWORD_OUTPUT_LEXEM);
    MOVE_CUR_LEX_PTR();

    size_t varOperand = 0;
    IF_ERR_RETURN(parseFuncArgs(analysator, parseTerminal, &varOperand));

    setNew_KEYWORD_OUTPUT_LEXEM_nodeAsRoot(analysator, varOperand, 0);

    return SYNTAX_ANALYSATOR_STATUS_OK;
}

SyntaxAnalysatorErrors parseInputKeyword(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    REQUIRE_LEXEM(KEYWORD_INPUT_LEXEM);
    MOVE_CUR_LEX_PTR();

    size_t varOperand = 0;
    IF_ERR_RETURN(parseFuncArgs(analysator, parseTerminal, &varOperand));

    setNew_KEYWORD_INPUT_LEXEM_nodeAsRoot(analysator, varOperand, 0);

    return SYNTAX_ANALYSATOR_STATUS_OK;
}



SyntaxAnalysatorErrors parseBooleanCondition(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    IF_ERR_RETURN(parseAssignOperator(analysator));

    return SYNTAX_ANALYSATOR_STATUS_OK;
}


// check for a first keyword (while or if) is already been done in according functions
static SyntaxAnalysatorErrors commonParserForWhileAndIf(SyntaxAnalysator* analysator, bool isWhileKeyword) {
    IF_ARG_NULL_RETURN(analysator);
    //IF_ARG_NULL_RETURN(parseLexem);

    LOG_DEBUG_VARS("ifwhile");
    REQUIRE_LEXEM(DELIMS_OPEN_SIMPLE_BRACKET_LEXEM);
    Lexem curLex = GET_CUR_LEXEM();
    LOG_DEBUG_VARS(curLex.strRepr);
    LOG_ERROR("while boom");
    MOVE_CUR_LEX_PTR();

    // ???
    IF_ERR_RETURN(parseBooleanCondition(analysator));
    size_t bracketsOperand = ANALYSATOR_ROOT;

    REQUIRE_LEXEM(DELIMS_CLOSE_SIMPLE_BRACKET_LEXEM);
    MOVE_CUR_LEX_PTR();

    LOG_ERROR("while1 --------------");
    IF_ERR_RETURN(parseBlockOfCode(analysator));
    size_t blockOfCodeOperand = ANALYSATOR_ROOT;

    if (isWhileKeyword) {
        setNew_KEYWORD_WHILE_LEXEM_nodeAsRoot(analysator, bracketsOperand, blockOfCodeOperand);
    } else {
        setNew_KEYWORD_IF_LEXEM_nodeAsRoot(analysator, bracketsOperand, blockOfCodeOperand);
    }

    return SYNTAX_ANALYSATOR_STATUS_OK;
}


SyntaxAnalysatorErrors parseWhileLoop(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    REQUIRE_LEXEM(KEYWORD_WHILE_LEXEM);
    MOVE_CUR_LEX_PTR();

    IF_ERR_RETURN(commonParserForWhileAndIf(analysator, true));

    return SYNTAX_ANALYSATOR_STATUS_OK;
}

SyntaxAnalysatorErrors parseIfKeyword(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    REQUIRE_LEXEM(KEYWORD_IF_LEXEM);
    MOVE_CUR_LEX_PTR();

    IF_ERR_RETURN(commonParserForWhileAndIf(analysator, false));

    return SYNTAX_ANALYSATOR_STATUS_OK;
}









SyntaxAnalysatorErrors commonParserOfIterativeOperators(SyntaxAnalysator* analysator,
                                                        const doesLexemSatisfyRuleFuncPtr doesSatisfyRule,
                                                        const parserFunctionPtr nextParseFunc) {
    IF_ARG_NULL_RETURN(analysator);
    IF_ARG_NULL_RETURN(doesSatisfyRule);
    IF_ARG_NULL_RETURN(nextParseFunc);

    IF_ERR_RETURN((*nextParseFunc)(analysator));
    while (analysator->curLexemInd < analysator->lenOfArr &&
           (*doesSatisfyRule)(GET_CUR_LEXEM())) {
        Lexem curLexem = GET_CUR_LEXEM();
        LOG_DEBUG_VARS("bruh");
        MOVE_CUR_LEX_PTR();
        size_t leftOperand = ANALYSATOR_ROOT;

        IF_ERR_RETURN((*nextParseFunc)(analysator));
        size_t rightOperand = ANALYSATOR_ROOT;
        LOG_DEBUG_VARS(leftOperand, rightOperand);

        SET_NEW_ROOT(NEW_OPERATOR_LEXEM_NODE(curLexem, leftOperand, rightOperand));
    }

    return SYNTAX_ANALYSATOR_STATUS_OK;
}

bool assignOpRule(Lexem lexem) {
    return isLexem_OPERATOR_ASSIGN_LEXEM(&lexem);
}

SyntaxAnalysatorErrors parseAssignOperator(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    IF_ERR_RETURN(commonParserOfIterativeOperators(analysator, assignOpRule, parseCompareOperator));

    return SYNTAX_ANALYSATOR_STATUS_OK;
}

bool compareOpRule(Lexem lexem) {
    return isLexem_OPERATOR_GREATER_LEXEM(&lexem) ||
           isLexem_OPERATOR_LESS_LEXEM(&lexem) ||
           isLexem_OPERATOR_IS_EQUAL_LEXEM(&lexem) ||
           isLexem_OPERATOR_LESS_OR_EQUAL_LEXEM(&lexem) ||
           isLexem_OPERATOR_GREATER_OR_EQUAL_LEXEM(&lexem);
}

SyntaxAnalysatorErrors parseCompareOperator(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    // ASK: cringe?
    bool was = false;
    while (!isCurLexem_DELIMS_SEMICOLON_LEXEM(analysator)) { // WARNING: be careful
        LOG_ERROR("biba bouba");
        LOG_DEBUG_VARS(GET_CUR_LEXEM().strRepr);
        SyntaxAnalysatorErrors err = parseAdditionSubtractionOperators(analysator);
        // FIXME:
        if (err != SYNTAX_ANALYSATOR_STATUS_OK) {
            if (!was) return SYNTAX_ANALYSATOR_INVALID_ARGUMENT;
            break;
        }

        was = true;
        size_t leftOperand = ANALYSATOR_ROOT;
        Lexem curLexem = GET_CUR_LEXEM();
        //LOG_ERROR("compare -------------------------------");
        //LOG_DEBUG_VARS(curLexem.strRepr, isLexem_OPERATOR_LESS_LEXEM(&curLexem));
        if (!compareOpRule(curLexem)) break;
        MOVE_CUR_LEX_PTR();

        //LOG_ERROR("compare -------------------------------");
        Lexem le = GET_CUR_LEXEM();
        LOG_DEBUG_VARS(le.strRepr);
        IF_ERR_RETURN(parseAdditionSubtractionOperators(analysator));
        size_t rightOperand = ANALYSATOR_ROOT;
        //LOG_DEBUG_VARS(leftOperand, rightOperand);

        size_t newRoot = NEW_OPERATOR_LEXEM_NODE(curLexem, leftOperand, rightOperand);
        SET_NEW_ROOT(newRoot);
        // LOG_DEBUG_VARS(curLexem.strRepr, isLexem_OPERATOR_LESS_LEXEM(&curLexem));
        //openImageOfCurrentStateSyntaxTree(&analysator->tree);
    }

    return SYNTAX_ANALYSATOR_STATUS_OK;
}

bool addSubOpRule(Lexem lexem) {
    return isLexem_OPERATOR_ADD_LEXEM(&lexem) ||
           isLexem_OPERATOR_SUB_LEXEM(&lexem);
}

SyntaxAnalysatorErrors parseAdditionSubtractionOperators(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    IF_ERR_RETURN(commonParserOfIterativeOperators(analysator, addSubOpRule, parseMultiplicationDivisionOperators));

    return SYNTAX_ANALYSATOR_STATUS_OK;
}

static SyntaxAnalysatorErrors parseBracketsOrFuncCalls(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    // WARNING: in different order it doesn't work, bruh
    LOG_DEBUG_VARS(GET_CUR_LEXEM().strRepr);
    LOG_DEBUG_VARS(GET_CUR_LEXEM().strRepr);
    TRY_PARSER_FUNC(parseFunctionCall);
    LOG_DEBUG_VARS(GET_CUR_LEXEM().strRepr);
    TRY_PARSER_FUNC(parseSimpleBracketDelim);

    return SYNTAX_ANALYSATOR_INVALID_ARGUMENT;
}

static bool mulDivOpRule(Lexem lexem) {
    return isLexem_OPERATOR_MUL_LEXEM(&lexem) ||
           isLexem_OPERATOR_DIV_LEXEM(&lexem);
}

SyntaxAnalysatorErrors parseMultiplicationDivisionOperators(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    IF_ERR_RETURN(commonParserOfIterativeOperators(analysator, mulDivOpRule, parseBracketsOrFuncCalls));

    return SYNTAX_ANALYSATOR_STATUS_OK;
}

SyntaxAnalysatorErrors parseSimpleBracketDelim(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    if (!isCurLexem_DELIMS_OPEN_SIMPLE_BRACKET_LEXEM(analysator)) {
        Lexem lex = GET_CUR_LEXEM();
        LOG_DEBUG_VARS("parse terminal in brackets", lex.strRepr);
        IF_ERR_RETURN(parseTerminal(analysator));
        return SYNTAX_ANALYSATOR_STATUS_OK;
    }

    MOVE_CUR_LEX_PTR();
    IF_ERR_RETURN(parseAssignOperator(analysator));
    REQUIRE_LEXEM(DELIMS_CLOSE_SIMPLE_BRACKET_LEXEM);
    MOVE_CUR_LEX_PTR();

    setNew_DELIMS_OPEN_SIMPLE_BRACKET_LEXEM_nodeAsRoot(analysator, ANALYSATOR_ROOT, 0);

    return SYNTAX_ANALYSATOR_STATUS_OK;
}

SyntaxAnalysatorErrors parseTerminal(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    Lexem lexem = GET_CUR_LEXEM();
    //LOG_ERROR("cur lexem is terminal: ");
    //LOG_DEBUG_VARS(lexem.doubleData, lexem.strRepr);

    switch (lexem.type) {
        case CONST_LEXEM_TYPE:
            SET_NEW_ROOT(NEW_CONST_LEXEM_NODE(lexem));
            break;
        case IDENTIFICATOR_LEXEM_TYPE:
            SET_NEW_ROOT(NEW_IDENTIFICATOR_NODE(lexem, 0, 0));
            break;
        default:
            LOG_DEBUG_VARS("invalid terminal type", lexem.strRepr);
            return SYNTAX_ANALYSATOR_INVALID_ARGUMENT;
            assert(false); // TODO: add error
    }

    MOVE_CUR_LEX_PTR();

    return SYNTAX_ANALYSATOR_STATUS_OK;
}





SyntaxAnalysatorErrors generateSyntaxTree(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    IF_ERR_RETURN(parseGrammar(analysator));
    LOG_DEBUG("generated syntax tree -------------------------");

    return SYNTAX_ANALYSATOR_STATUS_OK;
}

