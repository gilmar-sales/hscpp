#pragma once

#include <vector>
#include <string>
#include <limits>

namespace hscpp
{

    class LangError
    {
    public:
        static constexpr size_t NO_VALUE = (std::numeric_limits<size_t>::max)();

        enum class Code
        {
            Success,                                                                //
                                                                                    //
            Lexer_UnterminatedString,                                               // (expected, saw)
                                                                                    //
            Parser_FailedToParsePrefixExpression,                                   // (op)
            Parser_FailedToParseInfixExpression,                                    // (op)
            Parser_FailedToParseNumber,                                             // (number_val)
            Parser_NumberIsOutOfRange,                                              // (number_val)
            Parser_GroupExpressionMissingClosingParen,                              // ()
            Parser_IncludeMissingPath,                                              // ()
            Parser_HscppIfStmtMissingHscppEnd,                                      // (if_stmt_type)
            Parser_HscppStmtMissingOpeningParen,                                    // (stmt_type)
            Parser_HscppStmtMissingClosingParen,                                    // (stmt_type)
            Parser_HscppStmtArgumentMustBeStringLiteral,                            // (stmt_type)
            Parser_HscppRequireExpectedStringLiteralInArgumentList,                 // (require_type)
            Parser_HscppRequireExpectedStringLiteralOrIdentifierInArgumentList,     // (stmt_type)
            Parser_HscppRequireMissingCommaInArgumentList,                          // (require_type)
                                                                                    //
            Interpreter_UnableToResolveName,                                        // (name)
                                                                                    //
            Variant_OperandMustBeNumber,                                            // (op)
            Variant_OperandsDifferInType,                                           // (op, left, right)
                                                                                    //
            InternalError,                                                          // ()
        };

        LangError(Code errorCode);
        LangError(Code errorCode, size_t line, size_t column, const std::vector<std::string>& args);
        LangError(Code errorCode, size_t line, const std::vector<std::string>& args);
        LangError(Code errorCode, const std::vector<std::string>& args);

        bool IsSuccess();
        bool IsFail();

        std::string ToString();
        std::string GetArg(size_t i);

    private:
        Code m_ErrorCode;
        size_t m_Line = NO_VALUE;
        size_t m_Column = NO_VALUE;

        std::vector<std::string> m_Args;
    };

}