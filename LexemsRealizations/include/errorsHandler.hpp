#ifndef FUNCTIONS_REALIZATIONS_INCLUDE_ERRORS_HANDLER_HPP
#define FUNCTIONS_REALIZATIONS_INCLUDE_ERRORS_HANDLER_HPP

#include "../../common/include/errorsHandlerDefines.hpp"

#define ERROR_UNPACK(errName, errCode, errMessage) \
    LEXEM_REALIZATIONS_##errName = errCode,

enum LexemsRealizationsErrors {
    LEXEMS_REALIZATIONS_STATUS_OK = 0,
    #include "errorsPlainText.in"
};

#undef ERROR_UNPACK

const char* getLexemsRealizationsErrorMessage(LexemsRealizationsErrors error);

#endif
