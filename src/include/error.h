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

    // runtime
    Error_InvalidMemAccess,
    Error_NotInteger,
    Error_NotPositiveInteger,
    Error_WrongArgType,
}Error;

#endif
