#include "../include/commonFileStart.hpp"

// const node doesn't have any children
#define NEW_CONST_LEXEM_NODE(lexemArg) \
    constructNodeWithKidsNoErrors(&analysator->tree, &lexemArg, 0, 0)

// TODO: for now this is same
#define NEW_IDENTIFICATOR_NODE NEW_CONST_LEXEM_NODE

#define NEW_OPERATOR_LEXEM_NODE(lexemArg, left, right) \
    constructNodeWithKidsNoErrors(&analysator->tree, &lexemArg, left, right)

#define NEW_KEYWORD_LEXEM_NODE NEW_OPERATOR_LEXEM_NODE
#define NEW_DELIM_LEXEM_NODE   NEW_OPERATOR_LEXEM_NODE

#define SET_NEW_ROOT(newRoot) \
    analysator->tree.root = newRoot

#define ANALYSATOR_ROOT analysator->tree.root
#define MOVE_CUR_LEX_PTR() ++analysator->curLexemInd


SyntaxAnalysatorErrors parseGrammar(SyntaxAnalysator* analysator);
SyntaxAnalysatorErrors parseBlockOfCode(SyntaxAnalysator* analysator);
SyntaxAnalysatorErrors parseEqualOperator(SyntaxAnalysator* analysator);
SyntaxAnalysatorErrors parseAdditionSubtractionOperators(SyntaxAnalysator* analysator);
SyntaxAnalysatorErrors parseMultiplicationDivisionOperators(SyntaxAnalysator* analysator);
SyntaxAnalysatorErrors parseTerminal(SyntaxAnalysator* analysator);



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
        IF_ERR_RETURN(parseEqualOperator(analysator));
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

        IF_NOT_COND_RETURN(isCurLexem_DELIMS_SEMICOLON_LEXEM(analysator),
                       SYNTAX_ANALYSATOR_INVALID_ARGUMENT);
        MOVE_CUR_LEX_PTR();

        if (!leftOperand)
            leftOperand = rightOperand, rightOperand = 0;

        Lexem blockOfCodeLexem = {.type = DELIM_LEXEM_TYPE, .strRepr = ";", {.lexemSpecificName = DELIMS_SEMICOLON_LEXEM}};
        SET_NEW_ROOT(NEW_DELIM_LEXEM_NODE(blockOfCodeLexem, leftOperand, rightOperand));
        openImageOfCurrentStateSyntaxTree(&analysator->tree);
        leftOperand = ANALYSATOR_ROOT;
    }

    Lexem lexem = getCurrentLexem(analysator);
    LOG_DEBUG_VARS(lexem.strRepr, lexem.lexemSpecificName, lexem.type);
    IF_NOT_COND_RETURN(isCurLexem_DELIMS_CLOSE_CURLY_BRACKET_LEXEM(analysator),
                       SYNTAX_ANALYSATOR_INVALID_ARGUMENT);
    MOVE_CUR_LEX_PTR();

    Lexem blockOfCodeLexem = {.type = DELIM_LEXEM_TYPE, .strRepr = "{}", {.lexemSpecificName = DELIMS_OPEN_CURLY_BRACKET_LEXEM}};
    SET_NEW_ROOT(NEW_DELIM_LEXEM_NODE(blockOfCodeLexem, leftOperand, 0));

    return SYNTAX_ANALYSATOR_STATUS_OK;
}








typedef SyntaxAnalysatorErrors (*parserFunctionPtr)(SyntaxAnalysator* analysator);
typedef bool (*doesLexemSatisfyRuleFuncPtr) (Lexem lexem); // FIXME: Maybe better to add pointer

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
        MOVE_CUR_LEX_PTR();
        size_t leftOperand = ANALYSATOR_ROOT;

        IF_ERR_RETURN((*nextParseFunc)(analysator));
        size_t rightOperand = ANALYSATOR_ROOT;

        SET_NEW_ROOT(NEW_OPERATOR_LEXEM_NODE(curLexem, leftOperand, rightOperand));
    }

    return SYNTAX_ANALYSATOR_STATUS_OK;
}

bool equalOpRule(Lexem lexem) {
    return isLexem_OPERATOR_EQUAL_LEXEM(&lexem);
}

SyntaxAnalysatorErrors parseEqualOperator(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    Lexem lexem = {
        .type = OPERATOR_LEXEM_TYPE,
        "=",
        {.lexemSpecificName = OPERATOR_EQUAL_LEXEM},
    };
    IF_ERR_RETURN(commonParserOfIterativeOperators(analysator, equalOpRule, parseAdditionSubtractionOperators));

    return SYNTAX_ANALYSATOR_STATUS_OK;
}

bool addSubOpRule(Lexem lexem) {
    return isLexem_OPERATOR_ADD_LEXEM(&lexem) ||
           isLexem_OPERATOR_SUB_LEXEM(&lexem);
}

SyntaxAnalysatorErrors parseAdditionSubtractionOperators(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    Lexem lexem = {
        .type = OPERATOR_LEXEM_TYPE,
        "+",
        {.lexemSpecificName = OPERATOR_ADD_LEXEM},
    };
    IF_ERR_RETURN(commonParserOfIterativeOperators(analysator, addSubOpRule, parseMultiplicationDivisionOperators));

    return SYNTAX_ANALYSATOR_STATUS_OK;
}

bool mulDivOpRule(Lexem lexem) {
    return isLexem_OPERATOR_MUL_LEXEM(&lexem) ||
           isLexem_OPERATOR_DIV_LEXEM(&lexem);
}

SyntaxAnalysatorErrors parseMultiplicationDivisionOperators(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    Lexem lexem = {
        .type = OPERATOR_LEXEM_TYPE,
        "*",
        {.lexemSpecificName = OPERATOR_MUL_LEXEM},
    };
    IF_ERR_RETURN(commonParserOfIterativeOperators(analysator, mulDivOpRule, parseTerminal));

    return SYNTAX_ANALYSATOR_STATUS_OK;
}

SyntaxAnalysatorErrors parseTerminal(SyntaxAnalysator* analysator) {
    IF_ARG_NULL_RETURN(analysator);

    Lexem lexem = getCurrentLexem(analysator);
    LOG_ERROR("cur lexem is terminal: ");
    LOG_DEBUG_VARS(lexem.doubleData, lexem.strRepr);

    switch (lexem.type) {
        case CONST_LEXEM_TYPE:
            SET_NEW_ROOT(NEW_CONST_LEXEM_NODE(lexem));
            break;
        case IDENTIFICATOR_LEXEM_TYPE:
            SET_NEW_ROOT(NEW_IDENTIFICATOR_NODE(lexem));
            break;
        default:
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

