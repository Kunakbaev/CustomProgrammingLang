#include "../include/commonFileStart.hpp"
#include "../../Dumper/include/dumper.hpp"

const size_t OUTPUT_BUFFER_SIZE = 1 << 18;

static SyntaxTreeErrors dumpSyntaxTreeInConsoleRecursive(const SyntaxTree* tree, size_t nodeIndex,
                                                       char** outputBuffer, const Node2stringSettings* settings) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(outputBuffer);
    IF_ARG_NULL_RETURN(*outputBuffer);

    if (nodeIndex == 0) {
        strncat(*outputBuffer, "?", OUTPUT_BUFFER_SIZE);
        ++(*outputBuffer);
        return SYNTAX_TREE_STATUS_OK;
    }

    strncat(*outputBuffer, "(", OUTPUT_BUFFER_SIZE);
    ++(*outputBuffer);

    Node node = *getSyntaxTreeNodePtr(tree, nodeIndex);
    IF_ERR_RETURN(dumpSyntaxTreeInConsoleRecursive(tree, node.left, outputBuffer, settings));

    char* nodeDataString = NULL;
    ARIFM_OPS_ERR_CHECK(SyntaxTreeNodeToString(&node, &nodeDataString, settings));
    //LOG_DEBUG_VARS(node.memBuffIndex, nodeDataString);
    (*outputBuffer) += snprintf(*outputBuffer, OUTPUT_BUFFER_SIZE, " %s ", nodeDataString);
    FREE(nodeDataString);

    IF_ERR_RETURN(dumpSyntaxTreeInConsoleRecursive(tree, node.right, outputBuffer, settings));

    strncat(*outputBuffer, ")", OUTPUT_BUFFER_SIZE);
    ++(*outputBuffer);

    return SYNTAX_TREE_STATUS_OK;
}

SyntaxTreeErrors dumpSyntaxTreeInConsole(const SyntaxTree* tree) {
    IF_ARG_NULL_RETURN(tree);

    char* outputBuffer = (char*)calloc(OUTPUT_BUFFER_SIZE, sizeof(char));
    IF_NOT_COND_RETURN(outputBuffer != NULL,
                       ARIFM_TREE_MEMORY_ALLOCATION_ERROR);

    Node2stringSettings nodeDumpSettings = {
        .isNodeTypeNeeded = false,
        .isFuncTypeNeeded = false,
        .isBracketsNeeded = false,
    };

    char* targetPtr = outputBuffer;
    IF_ERR_RETURN(dumpSyntaxTreeInConsoleRecursive(tree, tree->root, &targetPtr, &nodeDumpSettings));
    LOG_DEBUG(outputBuffer);
    DEBUG_MESSAGE_TO_DUMPER_ALL_LOGS_FILE(tree->dumper, outputBuffer);
    FREE(outputBuffer);

    return SYNTAX_TREE_STATUS_OK;
}

SyntaxTreeErrors dumpSyntaxTree(SyntaxTree* tree) {
    IF_ARG_NULL_RETURN(tree);

    LOG_DEBUG("--------------------------------------\n");
    LOG_DEBUG("arifmetic tree:\n");
    DEBUG_MESSAGE_TO_DUMPER_ALL_LOGS_FILE(tree->dumper, "--------------------------------------\n");
    DEBUG_MESSAGE_TO_DUMPER_ALL_LOGS_FILE(tree->dumper, "arifmetic tree:\n");

    Node2stringSettings nodeDumpSettings = {
        .isNodeTypeNeeded = false,
        .isFuncTypeNeeded = false,
        .isBracketsNeeded = false,
    };

    size_t diffNodeTypesPtr[4] = {};
    size_t* diffNodeTypes[4] = {};
    for (size_t i = 1; i <= 3; ++i) {
        diffNodeTypes[i] = (size_t*)calloc(MAX_NUM_OF_NODES_IN_ONE_COLOR_WITH_NODES_STRUCT, sizeof(size_t));
        IF_NOT_COND_RETURN(diffNodeTypes[i] != NULL,
                           ARIFM_TREE_MEMORY_ALLOCATION_ERROR);
    }
    for (size_t nodeInd = 1; nodeInd <= tree->freeNodeIndex; ++nodeInd) {
        Node node = *getSyntaxTreeNodePtr(tree, nodeInd);
        if (node.nodeType == ARIFM_TREE_INVALID_NODE)
            continue;
        char* data = NULL;
        ARIFM_OPS_ERR_CHECK(SyntaxTreeNodeToString(&node, &data, &nodeDumpSettings));

        size_t parent = node.parent;
        size_t left   = node.left;
        size_t right  = node.right;
        //LOG_DEBUG_VARS(nodeInd, data, parent, left, right);
        DEBUG_VARS_TO_DUMPER_ALL_LOGS_FILE(tree->dumper, nodeInd, data, parent, left, right);
        FREE(data);

        assert(1 <= node.nodeType && node.nodeType <= 3);
        LOG_DEBUG_VARS(diffNodeTypesPtr[node.nodeType]);
        // if (diffNodeTypesPtr[node.nodeType] < MAX_NUM_OF_NODES_IN_ONE_COLOR_WITH_NODES_STRUCT) {
        //     fprintf(stderr, "%zu bruh %zu\n", diffNodeTypesPtr[node.nodeType], MAX_NUM_OF_NODES_IN_ONE_COLOR_WITH_NODES_STRUCT);
        // }
        assert(diffNodeTypesPtr[node.nodeType] < MAX_NUM_OF_NODES_IN_ONE_COLOR_WITH_NODES_STRUCT);
        diffNodeTypes[node.nodeType][diffNodeTypesPtr[node.nodeType]++] = nodeInd;
    }

    IF_ERR_RETURN(dumpSyntaxTreeInConsole(tree));

    size_t nodesArr[MAX_NUM_OF_NODES_IN_ONE_COLOR_WITH_NODES_STRUCT] = {}; // ASK: is it ok?
    nodesArr[0] = {tree->freeNodeIndex};
    NodesWithColor rule1 = {
        .color       = "#004000",
        .borderColor = "#006000",
        .numOfNodes  = (size_t)1,
        .nodes       = nodesArr,
    };

    #define ARR_LEN(arr) sizeof((arr)) / sizeof(*(arr))
    #define NODE_RULE(name, colorVar, borderColorVar, ind)     \
        NodesWithColor rule4##name = {                         \
            .color       = colorVar,                           \
            .borderColor = borderColorVar,                     \
            .numOfNodes  = diffNodeTypesPtr[ind],              \
            .nodes       = diffNodeTypes[ind],                 \
        }                                                      \

    NODE_RULE(nums,  "#603000", "#704000", ARIFM_TREE_NUMBER_NODE);
    NODE_RULE(vars,  "#400040", "#600060", ARIFM_TREE_VAR_NODE);
    NODE_RULE(funcs, "#600000", "#800000", ARIFM_TREE_FUNC_NODE);

    NodesWithColor coloringRule[] = {
        rule1, rule4nums, rule4vars, rule4funcs
    };
    DumperSettings settings = {
        .coloringRule           = coloringRule,
        .coloringRuleLen        = ARR_LEN(coloringRule),
        .isMemIndexesInfoNeeded = false,
        .node2stringSettings    = nodeDumpSettings,
    };
    DUMPER_ERR_CHECK(dumperDumpSyntaxTree(tree->dumper, tree, &settings));
    for (int i = 1; i <= 3; ++i)
        FREE(diffNodeTypes[i]);

    return SYNTAX_TREE_STATUS_OK;
}

SyntaxTreeErrors openImageOfCurrentStateSyntaxTree(SyntaxTree* tree) {
    IF_ARG_NULL_RETURN(tree);
    //RETURN_IF_INVALID();
    LOG_WARNING("--------------@@@@@@@@@@@@@@@@@@@@@");

    // first we need to create image of current state of tree
    IF_ERR_RETURN(dumpSyntaxTree(tree));

    // FIXME: be carefull with nasty commands
    const char* fileName = getLastImageFileName(tree->dumper);
    const size_t TMP_LEN = 100;
    char tmp[TMP_LEN] = {};
    snprintf(tmp, TMP_LEN, "xdg-open %s", fileName);
    system(tmp);

    //RETURN_IF_INVALID();
    return SYNTAX_TREE_STATUS_OK;
}
