#include <errno.h>

#include "../include/commonFileStart.hpp"

static const size_t MAX_LINE_LEN = 1 << 6;

static SyntaxTreeErrors readSyntaxTreeFromFileRecursive(SyntaxTree* tree,
                                                        FILE* file,
                                                        char* lineBuffer) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(file);
    IF_ARG_NULL_RETURN(lineBuffer);

    size_t newNodeInd = 0;
    IF_ERR_RETURN(getNewNode(tree, &newNodeInd));
    Node* node = NULL;
    node = getSyntaxTreeNodePtr(tree, newNodeInd);

    LEXEMS_REALIZATIONS_ERR_CHECK(readLexemFromFile(file, &node->lexem, lineBuffer, MAX_LINE_LEN));

    fgets(lineBuffer, MAX_LINE_LEN, file); // reads empty line that delims two different nodes
    assert(lineBuffer != NULL);
    //LOG_DEBUG_VARS(lineBuffer);
    errno = 0;
    char* endPtr = NULL;
    int numOfChildren = strtol(lineBuffer, &endPtr, 10);
    //LOG_DEBUG_VARS(numOfChildren);

    fgets(lineBuffer, MAX_LINE_LEN, file); // reads empty line that delims two different nodes
    assert(lineBuffer != NULL);
    //LOG_DEBUG_VARS(lineBuffer);

    IF_NOT_COND_RETURN(numOfChildren == 0 || numOfChildren == 1 || numOfChildren == 2,
                       SYNTAX_TREE_INVALID_ARGUMENT); // TODO: add error

    if (numOfChildren >= 1) {
        IF_ERR_RETURN(readSyntaxTreeFromFileRecursive(tree, file, lineBuffer));
        node = getSyntaxTreeNodePtr(tree, newNodeInd);
        node->left = tree->root;
    }
    if (numOfChildren == 2) {
        IF_ERR_RETURN(readSyntaxTreeFromFileRecursive(tree, file, lineBuffer));
        node = getSyntaxTreeNodePtr(tree, newNodeInd);
        node->right = tree->root;
    }
    tree->root = node->memBuffIndex;

    return SYNTAX_TREE_STATUS_OK;
}

SyntaxTreeErrors readSyntaxTreeFromFile(SyntaxTree* tree, const char* sourceFilePath) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(sourceFilePath);

    FILE* file = fopen(sourceFilePath, "r");
    IF_NOT_COND_RETURN(file != NULL, SYNTAX_TREE_FILE_OPENING_ERROR);

    char* lineBuffer = (char*)calloc(MAX_LINE_LEN, sizeof(char));
    IF_NOT_COND_RETURN(lineBuffer != NULL, SYNTAX_TREE_MEMORY_ALLOCATION_ERROR);

    IF_ERR_RETURN(readSyntaxTreeFromFileRecursive(tree, file, lineBuffer));
    FREE(lineBuffer);

    fclose(file);

    return SYNTAX_TREE_STATUS_OK;
}

