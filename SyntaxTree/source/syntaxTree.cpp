#include <stdlib.h>
#include <inttypes.h>

#include "../include/syntaxTree.hpp"
#include "../../Dumper/include/dumper.hpp"
#include "../include/commonFileStart.hpp"

const size_t MIN_MEM_BUFF_SIZE  = 8;
const size_t MAX_OBJ_NAME_LEN   = 30;

static void initMemBuff(SyntaxTree* tree) {
    assert(tree != NULL);

    for (size_t nodeInd = 0; nodeInd < tree->memBuffSize; ++nodeInd) {
        Node* node = &tree->memBuff[nodeInd];
        node->memBuffIndex = nodeInd;
        node->lexem = {
            .type = INVALID_LEXEM_TYPE,
            .strRepr = NULL,
            .lexemSpecificName = INVALID_LEXEM,
        };
    }
}

SyntaxTreeErrors constructSyntaxTree(SyntaxTree* tree, Dumper* dumper) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(dumper);

    tree->root              = 0;
    tree->memBuff           = (Node*)calloc(MIN_MEM_BUFF_SIZE, sizeof(Node));
    IF_NOT_COND_RETURN(tree->memBuff != NULL,
                       SYNTAX_TREE_MEMORY_ALLOCATION_ERROR);
    tree->memBuffSize       = MIN_MEM_BUFF_SIZE;
    tree->freeNodeIndex     = 0; // 0 index is equal to NULL
    tree->dumper            = dumper;
    initMemBuff(tree);

    RETURN_IF_INVALID();
    return SYNTAX_TREE_STATUS_OK;
}

static SyntaxTreeErrors constructNode(SyntaxTree* tree, Node* node, const Lexem* lexem, const NodeDataUnion data) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(node);
    IF_ARG_NULL_RETURN(lexem);

    node->left  = node->right = node->parent = 0;
    node->lexem = *lexem;

    return SYNTAX_TREE_STATUS_OK;
}

SyntaxTreeErrors constructNodeWithKids(SyntaxTree* tree, size_t* newNodeInd, const Lexem* lexem, const NodeDataUnion data,
                                      size_t leftSon,  size_t rightSon) {
    IF_ARG_NULL_RETURN(newNodeInd);
    IF_ARG_NULL_RETURN(tree);

    IF_ERR_RETURN(getNewNode(tree, newNodeInd));
    Node* node = getSyntaxTreeNodePtr(tree, *newNodeInd);

    IF_ERR_RETURN(constructNode(tree, node, lexem, data));
    node->left  = leftSon;
    node->right = rightSon;
    return SYNTAX_TREE_STATUS_OK;
}

size_t constructNodeWithKidsNoErrors(SyntaxTree* tree, const Lexem* lexem, const NodeDataUnion data,
                                     size_t leftSon,  size_t rightSon) {
    size_t newNodeInd = 0;
    SyntaxTreeErrors error = SYNTAX_TREE_STATUS_OK;
    error = getNewNode(tree, &newNodeInd);
    assert(error == SYNTAX_TREE_STATUS_OK);
    Node* node = getSyntaxTreeNodePtr(tree, newNodeInd);

    error = constructNode(tree, node, lexem, data);
    assert(error == SYNTAX_TREE_STATUS_OK);

    node->left  = leftSon;
    node->right = rightSon;
    return newNodeInd;
}

static SyntaxTreeErrors resizeMemBuffer(SyntaxTree* tree, size_t newSize) {
    IF_ARG_NULL_RETURN(tree);

    if (newSize < MIN_MEM_BUFF_SIZE)
        newSize = MIN_MEM_BUFF_SIZE;

    if (tree->memBuffSize == newSize) // nothing to do
        return SYNTAX_TREE_STATUS_OK;

    size_t oldSize   = tree->memBuffSize;
    size_t deltaSize = tree->memBuffSize > newSize
                            ? tree->memBuffSize - newSize
                            : newSize - tree->memBuffSize;
    size_t deltaBytes = deltaSize * sizeof(Node);

    if (oldSize > newSize) {
        memset(tree->memBuff + newSize, 0, deltaBytes);
    }

    //LOG_DEBUG_VARS(oldSize, deltaSize, deltaBytes);
    Node* tmp = (Node*)realloc(tree->memBuff, newSize * sizeof(Node));
    IF_NOT_COND_RETURN(tmp != NULL, SYNTAX_TREE_MEMORY_ALLOCATION_ERROR);
    tree->memBuff     = tmp;
    tree->memBuffSize = newSize;
//     LOG_DEBUG_VARS(newSize);
//
//     LOG_DEBUG_VARS(tmp);

    if (oldSize < newSize) {
        memset(tree->memBuff + oldSize, 0, deltaBytes - 1);
    }

    // if oldSize > newSize, no iterations will be executed
    for (size_t nodeInd = oldSize; nodeInd < newSize; ++nodeInd) {
        Node* node = &tree->memBuff[nodeInd];
        node->memBuffIndex = nodeInd;
        node->lexem = {
            .type = INVALID_LEXEM_TYPE,
            .strRepr = NULL,
            .lexemSpecificName = INVALID_LEXEM,
        };
    }
    //LOG_WARNING("@@@@@@@@@@@@@@@@@@@@@@");

    // for (size_t i = 0; i < newSize; ++i)
    //     LOG_DEBUG_VARS(oldSize, newSize, tree->memBuff[i].data = 10);

    return SYNTAX_TREE_STATUS_OK;
}

SyntaxTreeErrors linkNewNodeToParent(SyntaxTree* tree, size_t parentInd, bool isLeftSon,
                                    size_t* newNodeInd, const char* substr) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(newNodeInd);
    IF_ARG_NULL_RETURN(substr);

    *newNodeInd = 0;
    IF_ERR_RETURN(getNewNode(tree, newNodeInd));
    if (tree->root == 0) // tree is empty
        tree->root = *newNodeInd;
    Node* node = getSyntaxTreeNodePtr(tree, *newNodeInd);

    if (parentInd != 0) {
        node->parent = parentInd;
        Node* parent = getSyntaxTreeNodePtr(tree, parentInd);
        if (isLeftSon)
            parent->left  = *newNodeInd;
        else
            parent->right = *newNodeInd;
    }

    LEXEMS_REALIZATIONS_ERR_CHECK(initLexemWithString(substr, &node->lexem));
    //LOG_DEBUG_VARS((node)->data);

    return SYNTAX_TREE_STATUS_OK;
}

SyntaxTreeErrors getNewNode(SyntaxTree* tree, size_t* newNodeIndex) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(newNodeIndex);

    if (tree->freeNodeIndex + 1 >= tree->memBuffSize) {
        //LOG_DEBUG_VARS(tree->freeNodeIndex + 1, tree->memBuffSize, "resize");
        IF_ERR_RETURN(resizeMemBuffer(tree, tree->memBuffSize * 2));
        //LOG_DEBUG_VARS(tree->memBuffSize);
    }
    assert(tree->freeNodeIndex < tree->memBuffSize);
    *newNodeIndex = ++tree->freeNodeIndex;

    return SYNTAX_TREE_STATUS_OK;
}

Node* getSyntaxTreeNodePtr(const SyntaxTree* tree, size_t nodeInd) {
    assert(tree    != NULL);
    assert(nodeInd <= tree->freeNodeIndex);
    assert(nodeInd != 0);

    //LOG_DEBUG_VARS(tree, nodeInd, tree->freeNodeIndex, tree->memBuffSize);
    return &tree->memBuff[nodeInd];
}

size_t getCopyOfSubtree(const SyntaxTree* tree, SyntaxTree* destTree,
                        size_t srcNodeInd) {
    if (!srcNodeInd)
        return 0;

    assert(tree     != NULL);
    assert(destTree != NULL);

    size_t dest = 0;
    IF_ERR_RETURN(getNewNode(destTree, &dest));
    LOG_DEBUG_VARS(dest, destTree->freeNodeIndex, destTree->memBuffSize);
    Node* node = getSyntaxTreeNodePtr(destTree, dest);
    LOG_DEBUG_VARS(srcNodeInd);
    Node   old = *getSyntaxTreeNodePtr(tree, srcNodeInd);
    //LOG_DEBUG_VARS(old.data, old.nodeType, old.memBuffIndex);

    node->lexem = old.lexem;

    // FIXME: кажется копипаст, переписать. Выполнять для себя, вызывать для детей
    LOG_DEBUG_VARS(dest, destTree->memBuffSize);
    size_t cop  = getCopyOfSubtree(tree, destTree, old.left);
    destTree->memBuff[dest].left = cop;
    LOG_DEBUG_VARS(dest, destTree->memBuffSize, destTree->memBuff[dest].right, old.right);
    destTree->memBuff[dest].right = 0;
    cop = getCopyOfSubtree(tree, destTree, old.right);
    destTree->memBuff[dest].right = cop;

    return dest;
}

SyntaxTreeErrors getCopyOfTree(const SyntaxTree* source, SyntaxTree* dest) {
    IF_ARG_NULL_RETURN(source);
    IF_ARG_NULL_RETURN(dest);

    constructSyntaxTree(dest, source->dumper);
    dest->root = getCopyOfSubtree(source, dest, source->root);

    return SYNTAX_TREE_STATUS_OK;
}

#include "treeSimplification.cpp"
#include "treeDumperFuncs.cpp"
#include "treeValidationFuncs.cpp"

SyntaxTreeErrors destructSyntaxTree(SyntaxTree* tree) {
    IF_ARG_NULL_RETURN(tree);

    LOG_DEBUG_VARS(&tree->dumper);
    FREE(tree->memBuff);

    tree->memBuffSize   = 0;
    tree->freeNodeIndex = 0;

    return SYNTAX_TREE_STATUS_OK;
}
