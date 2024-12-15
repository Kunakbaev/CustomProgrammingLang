#define GEN4LEFT() \
    IF_ERR_RETURN(recursiveGenerationOfAssemblerCode(tree, node.left , depthInBlocksOfCode, file))

#define GEN4RIGHT() \
    IF_ERR_RETURN(recursiveGenerationOfAssemblerCode(tree, node.right, depthInBlocksOfCode, file))

#define PRINT() fprintf(file, "%s", line)

#define ADD_TABS()                                          \
    do {                                                    \
        for (int i = 0; i < depthInBlocksOfCode; ++i)       \
            ADD2BUFF("\t");                                 \
    } while (0)
