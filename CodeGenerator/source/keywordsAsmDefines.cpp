#include "commonDefines.cpp"
//
// #define KEYWORD_IF_LEXEM_ASM_DEF()
// #define KEYWORD_WHILE_LEXEM_ASM_DEF()
//

#define KEYWORD_INT_LEXEM_ASM_DEF()

#define KEYWORD_IF_LEXEM_ASM_DEF()                          \
    do {                                                    \
        GEN4LEFT();                                         \
        \
        CLEAR_LINE();\
        ADD_TABS();\
        ADD2BUFF("push 0\n");\
        ADD_TABS();\
        ADD2BUFF("je endif:\n");\
        PRINT();\
        \
        GEN4RIGHT();                                        \
        \
        CLEAR_LINE();\
        ADD_TABS();\
        ADD2BUFF("endif:\n");\
        PRINT();\
    } while (0)

#define KEYWORD_WHILE_LEXEM_ASM_DEF()                       \
    do {                                                    \
        ADD_TABS();                                         \
        ADD2BUFF("whileStart:\n");                          \
        PRINT();                                            \
                                                            \
        GEN4LEFT();                                         \
                                                            \
        CLEAR_LINE();                                       \
        ADD_TABS();                                         \
        ADD2BUFF("push 0\n");                               \
        ADD_TABS();                                         \
        ADD2BUFF("je endWhile:\n");                         \
                                                            \
        GEN4RIGHT();                                        \
                                                            \
        CLEAR_LINE();                                       \
        ADD_TABS();                                         \
        ADD2BUFF("jmp whileStart:\n");                      \
        ADD_TABS();                                         \
        ADD2BUFF("endWhile:\n");                            \
        PRINT();                                            \
    } while (0)

#define KEYWORD_INPUT_LEXEM_ASM_DEF()                       \
    do {                                                    \
        Identificator id = {};                                           \
        Node left = *getSyntaxTreeNodePtr(&generator->tree, node.left); \
        Node leftleft = *getSyntaxTreeNodePtr(&generator->tree, left.left); \
        getIdentificatorByLexem(&generator->checker, &leftleft.lexem, &id);  \
        CLEAR_LINE();                                       \
        ADD_TABS();                                         \
        ADD2BUFF("in\n");                                   \
        ADD_TABS();                                         \
        ADD2BUFF("pop %s\n", left.lexem.strRepr);           \
        PRINT();                                            \
    } while (0)

#define KEYWORD_OUTPUT_LEXEM_ASM_DEF()                      \
    do {                                                    \
        Identificator id = {};                                           \
        Node left = *getSyntaxTreeNodePtr(&generator->tree, node.left); \
        Node leftleft = *getSyntaxTreeNodePtr(&generator->tree, left.left); \
        getIdentificatorByLexem(&generator->checker, &leftleft.lexem, &id);  \
        CLEAR_LINE();                                       \
        ADD_TABS();                                         \
        ADD2BUFF("push [%d]\n", id.arrInd);          \
        ADD_TABS();                                         \
        ADD2BUFF("out\n");                                  \
        PRINT();\
    } while (0)

#define KEYWORD_RETURN_LEXEM_ASM_DEF()                      \
    do {                                                    \
        GEN4LEFT();\
        GEN4RIGHT();\
        ADD_TABS();\
        ADD2BUFF("ret\n");\
        PRINT();\
    } while (0)
