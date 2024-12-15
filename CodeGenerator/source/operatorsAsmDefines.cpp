#include "commonDefines.cpp"

#define OPERATOR_ADD_LEXEM_ASM_DEF()                        \
    do {                                                    \
        GEN4LEFT();                                         \
        GEN4RIGHT();                                        \
        ADD_TABS();                                         \
        ADD2BUFF("add\n");                                  \
        PRINT();                                            \
    } while (0)

#define OPERATOR_SUB_LEXEM_ASM_DEF()                        \
    do {                                                    \
        GEN4LEFT();                                         \
        GEN4RIGHT();                                        \
        ADD_TABS();                                         \
        ADD2BUFF("sub\n");                                  \
        PRINT();                                            \
    } while (0)

#define OPERATOR_MUL_LEXEM_ASM_DEF()                        \
    do {                                                    \
        GEN4LEFT();                                         \
        GEN4RIGHT();                                        \
        ADD_TABS();                                         \
        ADD2BUFF("mul\n");                                  \
        PRINT();                                            \
    } while (0)

#define OPERATOR_DIV_LEXEM_ASM_DEF()                        \
    do {                                                    \
        GEN4LEFT();                                         \
        GEN4RIGHT();                                        \
        ADD_TABS();                                         \
        ADD2BUFF("div\n");                                  \
        PRINT();                                            \
    } while (0)

#define OPERATOR_EQUAL_LEXEM_ASM_DEF()                      \
    do {                                                    \
        GEN4LEFT();                                         \
        GEN4RIGHT();                                        \
        ADD_TABS();                                         \
        ADD2BUFF("equal\n");                                \
        PRINT();                                            \
    } while (0)


