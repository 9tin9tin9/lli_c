#include "include/error.h"
#include "include/code.h"
#include <stdio.h>

#define eprintf(...) fprintf(stderr, __VA_ARGS__)

#define error_case(e_) \
    case e_: \
        eprintf(#e_ "\n"); \
        break;

void
Error_print(Error r, void* _c, int level)
{
    // cast type
    Code* c = _c;

    switch (r) {
        error_case(Error_WrongTokTypeForOp);

        error_case(Error_ParseNumError);
        error_case(Error_ParseIdxError);
        error_case(Error_UnterminatedIdx);
        error_case(Error_EmptyIdx);
        error_case(Error_MissingVarName);

        error_case(Error_EmptyToken);
        error_case(Error_UnexpectedDelim);
        error_case(Error_NonDelimAfterSymEnd);
        error_case(Error_DoubleQuoteInMiddle);
        error_case(Error_UnknownEscapeSequence);

        error_case(Error_UndefinedVar);
        error_case(Error_UndefinedLabel);
        error_case(Error_UnknownOp);

        error_case(Error_InvalidMemAccess);
        error_case(Error_CannotWriteToNMem);
        error_case(Error_NotInteger);
        error_case(Error_NotPositiveInteger);
        error_case(Error_WrongArgType);
        error_case(Error_WrongArgCount);
        error_case(Error_CannotOpenFile);
        error_case(Error_CannotReadFile);

        default:
            break;
    }
}
