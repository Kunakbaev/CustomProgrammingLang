#include "../include/commonFileStart.hpp"

const size_t MAX_NUM_OF_IDENTIFICATORS = 1 << 10;
const size_t MAX_NUM_OF_NODES_IN_TREE  = 1 << 12;

static SemanticCheckerErrors recursiveFindTinTout(
    SemanticChecker* checker,
    size_t curNodeInd
) {
    IF_ARG_NULL_RETURN(checker);

    if (!curNodeInd)
        return SEMANTIC_CHECKER_STATUS_OK;

    Node node    = *getSyntaxTreeNodePtr(checker->tree, curNodeInd);
    assert(node.memBuffIndex < MAX_NUM_OF_IDENTIFICATORS);
    checker->tinArray[node.memBuffIndex] = checker->timer++;

    IF_ERR_RETURN(recursiveFindTinTout(checker, node.left));
    IF_ERR_RETURN(recursiveFindTinTout(checker, node.right));

    checker->toutArray[node.memBuffIndex] = checker->timer++;

    return SEMANTIC_CHECKER_STATUS_OK;
}

SemanticCheckerErrors constructSemanticChecker(SemanticChecker* checker, const SyntaxTree* tree) {
    IF_ARG_NULL_RETURN(checker);
    IF_ARG_NULL_RETURN(tree);

    checker->tree = tree; // ASK: is this copy ok?
    checker->tableArrLen = 0;
    checker->tableOfVars = (Identificator*)calloc(MAX_NUM_OF_IDENTIFICATORS, sizeof(Identificator));
    IF_NOT_COND_RETURN(checker->tableOfVars != NULL,
                       SEMANTIC_CHECKER_MEMORY_ALLOCATION_ERROR);

    checker->tinArray = (size_t*)calloc(MAX_NUM_OF_NODES_IN_TREE, sizeof(size_t));
    IF_NOT_COND_RETURN(checker->tinArray != NULL,
                       SEMANTIC_CHECKER_MEMORY_ALLOCATION_ERROR);
    checker->toutArray = (size_t*)calloc(MAX_NUM_OF_NODES_IN_TREE, sizeof(size_t));
    IF_NOT_COND_RETURN(checker->toutArray != NULL,
                       SEMANTIC_CHECKER_MEMORY_ALLOCATION_ERROR);

    IF_ERR_RETURN(recursiveFindTinTout(checker, checker->tree->root));

    return SEMANTIC_CHECKER_STATUS_OK;
}









// --------------------------------------     IDENTIFICATORS SCOPES CHECK     ------------------------------------------

SemanticCheckerErrors getIdentificatorByLexem(
    const SemanticChecker* checker,
    const Lexem* lexem,
    Identificator* result
);

static SemanticCheckerErrors isAncestorInSyntaxTree(
    const SemanticChecker*  checker,
    const Node*             father,
    const Node*             son,
    bool*                   isAncestor
) {
    IF_ARG_NULL_RETURN(checker);
    IF_ARG_NULL_RETURN(father);
    IF_ARG_NULL_RETURN(son);
    IF_ARG_NULL_RETURN(isAncestor);

    size_t fatherInd = father->memBuffIndex;
    size_t sonInd    = son->memBuffIndex;
    *isAncestor =
        checker->tinArray[fatherInd] <= checker->tinArray[sonInd] &&
          checker->toutArray[sonInd] <= checker->toutArray[fatherInd];

    return SEMANTIC_CHECKER_STATUS_OK;
}

static SemanticCheckerErrors recursiveCheckForIdentficatorsScopes(
    SemanticChecker* checker,
    size_t curNodeInd
) {
    IF_ARG_NULL_RETURN(checker);

    if (!curNodeInd)
        return SEMANTIC_CHECKER_STATUS_OK;

    Node node   = *getSyntaxTreeNodePtr(checker->tree, curNodeInd);
    Lexem lexem = node.lexem;

    size_t left  = node.left;
    size_t right = node.right;

    LOG_DEBUG_VARS(node.memBuffIndex, left, right);
    if (lexem.type == IDENTIFICATOR_LEXEM_TYPE) {
        Identificator id = {};
        LOG_DEBUG_VARS(lexem.strRepr);
        IF_ERR_RETURN(getIdentificatorByLexem(checker, &lexem, &id));
        if (id.declNodeInd != curNodeInd) { // that's not a declaration, that's a usage of some identificator
            bool isAncestor = false;
            //LOG_DEBUG_VARS(node.memBuffIndex, id.scopeNode->memBuffIndex);
            LOG_DEBUG_VARS(id.lexem.strRepr, id.scopeNode->memBuffIndex, id.declNodeInd);
            LOG_ERROR("var check");
            IF_ERR_RETURN(isAncestorInSyntaxTree(checker, id.scopeNode, &node, &isAncestor));
            IF_NOT_COND_RETURN(isAncestor, SEMANTIC_CHECKER_IDENTIFICATOR_USAGE_OUTSIDE_ITS_SCOPE);
            bool isAfterDecl = checker->tinArray[id.declNodeInd] <=
                               checker->tinArray[node.memBuffIndex];
            LOG_DEBUG_VARS(checker->tinArray[id.declNodeInd], checker->tinArray[node.memBuffIndex]);
            LOG_DEBUG_VARS(checker->toutArray[id.declNodeInd], checker->toutArray[node.memBuffIndex]);
            IF_NOT_COND_RETURN(isAfterDecl, SEMANTIC_CHECKER_IDENTIFICATOR_USAGE_BEFORE_ITS_DECLARATION);
        }
    }

    IF_ERR_RETURN(recursiveCheckForIdentficatorsScopes(checker, left));
    IF_ERR_RETURN(recursiveCheckForIdentficatorsScopes(checker, right));

    return SEMANTIC_CHECKER_STATUS_OK;
}


SemanticCheckerErrors semanticCheckOfSyntaxTree(SemanticChecker* checker) {
    IF_ARG_NULL_RETURN(checker);

    IF_ERR_RETURN(recursiveCheckForIdentficatorsScopes(checker, checker->tree->root));

    return SEMANTIC_CHECKER_STATUS_OK;
}





















SemanticCheckerErrors getIdentificatorByLexem(
    const SemanticChecker* checker,
    const Lexem*           lexem,
    Identificator*         result
) {
    LOG_DEBUG_VARS(lexem->strRepr);
    IF_ARG_NULL_RETURN(checker);
    IF_ARG_NULL_RETURN(lexem);
    IF_ARG_NULL_RETURN(result);
    IF_NOT_COND_RETURN(lexem->type == IDENTIFICATOR_LEXEM_TYPE,
                       SEMANTIC_CHECKER_INVALID_ARGUMENT);

    for (size_t arrInd = 0; arrInd < checker->tableArrLen; ++arrInd) {
        Identificator id = checker->tableOfVars[arrInd];
        bool isCmp = strcmp(id.lexem.strRepr, lexem->strRepr) == 0;
        if (isCmp) {
            *result = id;
            return SEMANTIC_CHECKER_STATUS_OK;
        }
    }

    return SEMANTIC_CHECKER_IDENTIFICATOR_NOT_FOUND;
}

static SemanticCheckerErrors addNewIdentificator2Table(
    SemanticChecker*     checker,
    const Identificator* id
) {
    IF_ARG_NULL_RETURN(checker);
    IF_ARG_NULL_RETURN(id);

    Identificator resId = {};
    SemanticCheckerErrors err = getIdentificatorByLexem(checker, &id->lexem, &resId);
    IF_NOT_COND_RETURN(err == SEMANTIC_CHECKER_IDENTIFICATOR_NOT_FOUND,
                       SEMANTIC_CHECKER_IDENTIFICATOR_IS_ALREADY_DECLARED);

    IF_NOT_COND_RETURN(checker->tableArrLen < MAX_NUM_OF_IDENTIFICATORS,
                       SEMANTIC_CHECKER_INVALID_ARGUMENT);
    checker->tableOfVars[checker->tableArrLen] = *id;
    ++checker->tableArrLen;
    //checker->tableOfVars[checker->tableArrLen].arrInd = checker->tableArrLen++;

    return SEMANTIC_CHECKER_STATUS_OK;
}

SemanticCheckerErrors isLexemFunction(const SemanticChecker* checker, const Lexem* lexem, bool* isLexFunc) {
    IF_ARG_NULL_RETURN(checker);
    IF_ARG_NULL_RETURN(lexem);
    IF_ARG_NULL_RETURN(isLexFunc);

    Identificator id = {};
    SemanticCheckerErrors err = getIdentificatorByLexem(checker, lexem, &id);
    *isLexFunc = err     == SEMANTIC_CHECKER_STATUS_OK &&
                 id.type == FUNCTION_IDENTIFICATOR;

    return SEMANTIC_CHECKER_STATUS_OK;
}

static SemanticCheckerErrors findBlockOfCodeNodeForIdentificator(const SemanticChecker* checker, Node** node) {
    IF_ARG_NULL_RETURN(checker);
    IF_ARG_NULL_RETURN(node);
    IF_ARG_NULL_RETURN(*node);

    do {
        size_t parentInd = (*node)->parent;
        //LOG_DEBUG_VARS((*node)->memBuffIndex, parentInd);
        *node = getSyntaxTreeNodePtr(checker->tree, parentInd);

        if ((*node)->lexem.lexemSpecificName == DELIMS_OPEN_CURLY_BRACKET_LEXEM)
            break;

        if ((*node)->lexem.type != IDENTIFICATOR_LEXEM_TYPE)
            continue;
        bool isLexFunc = false;
        IF_ERR_RETURN(isLexemFunction(checker, &(*node)->lexem, &isLexFunc));
        if (isLexFunc) {
            IF_NOT_COND_RETURN((*node)->right != 0, SEMANTIC_CHECKER_INVALID_ARGUMENT);
            *node = getSyntaxTreeNodePtr(checker->tree, (*node)->right);
            IF_NOT_COND_RETURN((*node)->lexem.lexemSpecificName == DELIMS_OPEN_CURLY_BRACKET_LEXEM,
                               SEMANTIC_CHECKER_INVALID_ARGUMENT);
            break;
        }
    } while (true);

    return SEMANTIC_CHECKER_STATUS_OK;
}

SemanticCheckerErrors addNewIdentificator(
    SemanticChecker*         checker,
    Node*                    node,
    IdentificatorType        idType
) {
    Node* scopeNode = node;
    IF_ERR_RETURN(findBlockOfCodeNodeForIdentificator(checker, &scopeNode));
    Identificator id = {
        .declNodeInd = node->memBuffIndex,
        .lexem       = node->lexem,
        .type        = idType,
        .scopeNode   = scopeNode,
        .arrInd      = checker->tableArrLen,
    };
    LOG_ERROR("i am func");
    LOG_DEBUG_VARS(checker->tableArrLen);
    IF_ERR_RETURN(addNewIdentificator2Table(checker, &id));

    return SEMANTIC_CHECKER_STATUS_OK;
}

SemanticCheckerErrors recursiveAddOfIdentificatorsFromTree(
    SemanticChecker* checker,
    size_t curNodeInd,
    Lexems identificatorDataType
) {
    IF_ARG_NULL_RETURN(checker);

    if (!curNodeInd)
        return SEMANTIC_CHECKER_STATUS_OK;

    Node node   = *getSyntaxTreeNodePtr(checker->tree, curNodeInd);
    Lexem lexem = node.lexem;

    if (lexem.type == IDENTIFICATOR_LEXEM_TYPE && identificatorDataType != INVALID_LEXEM) {
        IF_ERR_RETURN(addNewIdentificator(checker, &node, VARIABLE_IDENTIFICATOR));
        return SEMANTIC_CHECKER_STATUS_OK;
    }

    size_t left = node.left;
    size_t right = node.right;

    // if node is not a data type
    if (lexem.lexemSpecificName != KEYWORD_INT_LEXEM) {
        IF_ERR_RETURN(recursiveAddOfIdentificatorsFromTree(checker, left,  identificatorDataType));
        IF_ERR_RETURN(recursiveAddOfIdentificatorsFromTree(checker, right, identificatorDataType));
        return SEMANTIC_CHECKER_STATUS_OK;
    }

    if (left != 0 && right != 0)
        return SEMANTIC_CHECKER_STATUS_OK;

    if (!left)
        left = right, right = 0; // swap, we want left not to be 0

    LOG_ERROR("int");
    LOG_DEBUG_VARS(left, right);
    Node childNode = *getSyntaxTreeNodePtr(checker->tree, left);
    LOG_DEBUG_VARS("bruh");
    // this is declaration of variable
    if ((   childNode.lexem.type == IDENTIFICATOR_LEXEM_TYPE &&
            childNode.left == 0 && childNode.right == 0) ||
            childNode.lexem.lexemSpecificName == DELIMS_COMMA_LEXEM) {
        IF_ERR_RETURN(recursiveAddOfIdentificatorsFromTree(checker, left,  KEYWORD_INT_LEXEM));
        IF_ERR_RETURN(recursiveAddOfIdentificatorsFromTree(checker, right, KEYWORD_INT_LEXEM));
        return SEMANTIC_CHECKER_STATUS_OK;
    }

    // otherwise, this is function declaration
    LOG_DEBUG_VARS("be");
    if (childNode.lexem.type == IDENTIFICATOR_LEXEM_TYPE) {
        IF_ERR_RETURN(addNewIdentificator(checker, &childNode, FUNCTION_IDENTIFICATOR));

        IF_ERR_RETURN(recursiveAddOfIdentificatorsFromTree(checker, childNode.left,  identificatorDataType));
        IF_ERR_RETURN(recursiveAddOfIdentificatorsFromTree(checker, childNode.right, identificatorDataType));
    }

    return SEMANTIC_CHECKER_STATUS_OK;
}
//
// SemanticCheckerErrors recursiveAddArgsForFunction(
//     SemanticChecker* checker,
//     size_t curNodeInd,
//
// ) {
//     IF_ARG_NULL_RETURN(checker);
//
//     if (!curNodeInd) return
//
//     return SEMANTIC_CHECKER_STATUS_OK;
// }

static SemanticCheckerErrors findLocalVarsForFuncRecursive(
    SemanticChecker* checker,
    size_t curNodeInd,
    FunctionIdentificator* function
) {
    IF_ARG_NULL_RETURN(checker);
    IF_ARG_NULL_RETURN(function);

    if (!curNodeInd)
        return SEMANTIC_CHECKER_STATUS_OK;

    Node node = *getSyntaxTreeNodePtr(checker->tree, curNodeInd);
    Lexem lexem = node.lexem;

    if (lexem.type == IDENTIFICATOR_LEXEM_TYPE) {
        Identificator id = {};
        getIdentificatorByLexem(checker, &lexem, &id);

        if (id.type == VARIABLE_IDENTIFICATOR &&
            id.declNodeInd == curNodeInd) { // declaration of var
            function->arrOfLocalVars[function->numOfLocalVars++] = id.arrInd;
            checker->tableOfVars[id.arrInd].localVarInd = function->numOfLocalVars - 1;
        }
    }

    IF_ERR_RETURN(findLocalVarsForFuncRecursive(checker, node.left,  function));
    IF_ERR_RETURN(findLocalVarsForFuncRecursive(checker, node.right, function));

    return SEMANTIC_CHECKER_STATUS_OK;
}

static SemanticCheckerErrors findArgsForFunctions(
    SemanticChecker* checker
) {
    IF_ARG_NULL_RETURN(checker);

    // works in O(n^2) (n - number of identificators) but it can be done faster: in O(nums in syntax tree),
    // recursive function from each function node, but I am to lazy
    for (size_t funcInd = 0; funcInd < checker->tableArrLen; ++funcInd) {
        Identificator func = checker->tableOfVars[funcInd];
        if (func.type != FUNCTION_IDENTIFICATOR)
            continue;

        Node funcNode      = *getSyntaxTreeNodePtr(checker->tree, func.declNodeInd);
        Node argsScopeNode = *getSyntaxTreeNodePtr(checker->tree, funcNode.right);

        const size_t MAX_NUM_OF_LOCAL_VARS = 1 << 10; // FIXME: 2 passes also is cringe, vector?
        FunctionIdentificator function = {};
        function.arrOfLocalVars = (size_t*)calloc(MAX_NUM_OF_LOCAL_VARS, sizeof(size_t));
        IF_NOT_COND_RETURN(function.arrOfLocalVars != NULL,
                            SEMANTIC_CHECKER_MEMORY_ALLOCATION_ERROR);

        IF_ERR_RETURN(findLocalVarsForFuncRecursive(checker, func.declNodeInd, &function));

        checker->tableOfVars[funcInd].function = function;
    }

    return SEMANTIC_CHECKER_STATUS_OK;
}

SemanticCheckerErrors buildTableOfIdentificators(SemanticChecker* checker) {
    IF_ARG_NULL_RETURN(checker);

    IF_ERR_RETURN(recursiveAddOfIdentificatorsFromTree(
                    checker, checker->tree->root, INVALID_LEXEM));
    IF_ERR_RETURN(findArgsForFunctions(checker));

    return SEMANTIC_CHECKER_STATUS_OK;
}

SemanticCheckerErrors dumpTableOfIdentificators(SemanticChecker* checker) {
    IF_ARG_NULL_RETURN(checker);

    LOG_DEBUG_VARS("-----------------------------");
    LOG_DEBUG_VARS("checker table of identificators");
    for (size_t arrInd = 0; arrInd < checker->tableArrLen; ++arrInd) {
        Identificator id = checker->tableOfVars[arrInd];
        char* dbgLine = NULL;
        Lexem2stringSettings dumpSettings = {
            .isLexemTypeNeeded = true,
            .isBracketsNeeded = true,
        };
        getLexemDataString(&id.lexem, &dumpSettings, &dbgLine);
        LOG_DEBUG_VARS(id.arrInd, id.type == VARIABLE_IDENTIFICATOR,
            dbgLine, id.declNodeInd, id.scopeNode->memBuffIndex);
        FREE(dbgLine);
    }

    LOG_DEBUG_VARS("------------------------");

    return SEMANTIC_CHECKER_STATUS_OK;
}

SemanticCheckerErrors destructSemanticChecker(SemanticChecker* checker) {
    IF_ARG_NULL_RETURN(checker);

    for (size_t arrInd = 0; arrInd < checker->tableArrLen; ++arrInd) {
        Identificator func = checker->tableOfVars[arrInd];
        if (func.type != FUNCTION_IDENTIFICATOR)
            continue;
        FREE(func.function.arrOfLocalVars);
    }

    FREE(checker->tableOfVars);
    FREE(checker->tinArray);
    FREE(checker->toutArray);
    //destructSyntaxTree(checker->tree);
    *checker = {
        .tableArrLen = 0,
        .tableOfVars = NULL,
        .tree        = NULL,
        .tinArray    = NULL,
        .toutArray   = NULL,
        .timer       = 0
    };

    return SEMANTIC_CHECKER_STATUS_OK;
}
