#ifndef _ERROR_H_
#define _ERROR_H_

typedef enum{
    Ok = 0,

    // lexing
    Error_WrongTokTypeForOp,

    Error_ParseNumError,
    Error_ParseIdxError,
    Error_UnterminatedIdx,
    Error_EmptyIdx,
    Error_MissingVarName,

    Error_EmptyToken,
    Error_UnexpectedDelim,
    Error_NonDelimAfterSymEnd,
    Error_DoubleQuoteInMiddle,
    Error_UnknownEscapeSequence,

    // preprocess
    Error_UndefinedVar,
    Error_UndefinedLabel,
    Error_UnknownOp,

    // runtime
    Error_InvalidMemAccess,
    Error_CannotWriteToNMem,
    Error_NotInteger,
    Error_NotPositiveInteger,
    Error_WrongArgType,
    Error_WrongArgCount,
    Error_CannotOpenFile,
    Error_CannotReadFile,
    Error_ModOperandNotInteger,
    Error_BadFileDescriptor,
    Error_IoError,
    Error_InvalidOpenOption,
    Error_ExceedOpenLimit,
}Error;

// void* -> Code*
void Error_print(Error, void*, int);

#define try(a_) { Error r = (a_); if (r) return r; }
// #define try(a_) a_

#endif
