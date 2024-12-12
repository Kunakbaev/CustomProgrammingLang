#ifndef SYNTAX_TREE_INCLUDE_COMMON_FILE_START_HPP
#define SYNTAX_TREE_INCLUDE_COMMON_FILE_START_HPP

#include "syntaxTree.hpp"
#include "../../FunctionsRealizations/include/functionsRealizations.hpp"

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, SYNTAX_TREE_INVALID_ARGUMENT, getSyntaxTreeErrorMessage)

#define IF_ERR_RETURN(error) \
    do {\
        COMMON_IF_ERR_RETURN(error, getSyntaxTreeErrorMessage, SYNTAX_TREE_STATUS_OK);\
        /*DEBUG_VARS_TO_DUMPER_ALL_LOGS_FILE((Dumper*)&list->dumper, #error);\*/\
    } while(0) \

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getSyntaxTreeErrorMessage)\

#define DUMPER_ERR_CHECK(error) \
    COMMON_IF_SUBMODULE_ERR_RETURN(error, getDumperErrorMessage, DUMPER_STATUS_OK, SYNTAX_TREE_DUMPER_ERROR);

#define ARIFM_OPS_ERR_CHECK(error) \
    COMMON_IF_SUBMODULE_ERR_RETURN(error, getArifmOperationsErrorMessage, ARIFM_OPERATIONS_STATUS_OK, SYNTAX_TREE_ARIFM_OPS_ERROR);

#define RETURN_IF_INVALID() \
    IF_ERR_RETURN(validateSyntaxTree(tree));

#endif
