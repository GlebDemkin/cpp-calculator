#pragma once

#include "calculator/Lexer.hpp"
#include "calculator/PluginManager.hpp"

namespace calculator 
{

class Parser 
{
public:
    Parser(std::string const& input, PluginManager const& plugins);
    double evaluate();

private:
    Lexer lexer_;
    Token current_;
    PluginManager const& plugins_;
    unsigned depth_ = 0;

    void readNextToken() 
    { 
        current_ = lexer_.next();  // Получить следующий токен
    }
    void expect(TokenKind kind, std::string const& message);
    double parseSum();
    double parseProduct();
    double parseUnary();
    double parsePower();
    double parsePrimary();
};

}
