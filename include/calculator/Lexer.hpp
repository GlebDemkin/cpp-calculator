#pragma once

#include <cstddef>
#include <string>

namespace calculator 
{

enum class TokenKind 
{
    Number, Identifier, Plus, Minus, Multiply, Divide, Power,
    LeftParenthesis, RightParenthesis, End
};

struct Token 
{
    TokenKind kind = TokenKind::End;
    double number  = 0;
    std::string text;
    std::size_t position = 0;
};

class Lexer 
{
public:
    explicit Lexer(std::string const& input);
    Token next();

private:
    std::string input_;
    std::size_t position_ = 0;
};

}
