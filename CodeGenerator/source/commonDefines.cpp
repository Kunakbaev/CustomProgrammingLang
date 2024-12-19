#define GEN4LEFT() \
    IF_ERR_RETURN(recursiveGenerationOfAssemblerCode(generator, node.left , depthInBlocksOfCode, file, id))

#define GEN4RIGHT() \
    IF_ERR_RETURN(recursiveGenerationOfAssemblerCode(generator, node.right, depthInBlocksOfCode, file, id))

#define PRINT() fprintf(file, "%s", line)

#define ADD_TABS()                                          \
    do {                                                    \
        for (int i = 0; i < depthInBlocksOfCode; ++i)       \
            ADD2BUFF("\t");                                 \
    } while (0)
