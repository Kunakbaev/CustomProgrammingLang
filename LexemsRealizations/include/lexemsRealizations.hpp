#ifndef ARIFM_OPERATIONS_INCLUDE_ARIFM_OPERATIONS_HPP
#define ARIFM_OPERATIONS_INCLUDE_ARIFM_OPERATIONS_HPP

#include "errorsHandler.hpp"
#include "../../Dumper/include/dumperStruct.hpp"
//#include "../../SyntaxTree/include/syntaxTree.hpp"

/*



*/

enum LexemType {
    INVALID_LEXEM_TYPE       = 0,
    CONST_LEXEM_TYPE         = 1,
    IDENTIFICATOR_LEXEM_TYPE = 2,
    KEYWORD_LEXEM_TYPE       = 3,
    DELIM_LEXEM_TYPE         = 4,
    OPERATOR_LEXEM_TYPE      = 5,
};

#define GENERAL_LEXEM_DEF(enumName, ...) \
    enumName,

enum Lexems {
    #include "codeGen/allLexems.hpp"
};

struct Lexem {
    LexemType   type;
    const char* strRepr; // str repr can be get from data (var index, or operator index or keyword index), so we would just have table with names and addition getter funcs
    // general lexem identificator or number (const)
    union {
        size_t   data;
        double   doubleData;
    };
};

struct Lexem2stringSettings {
    bool isLexemTypeNeeded;
    bool isFuncTypeNeeded;
    bool isBracketsNeeded;
};

LexemsRealizationsErrors initLexemWithString(const char* line, Lexem* lexem);
LexemsRealizationsErrors isCharLexemDelim(const char ch, bool* isDelim);
LexemsRealizationsErrors saveLexemToFile(FILE* file, const Lexem* lexem);

#endif
