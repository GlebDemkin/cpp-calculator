#include "calculator/Lexer.hpp"

#include <stdexcept>

namespace calculator 
{
namespace 
{

bool digit(char value) 
{ 
    return value >= '0' && value <= '9';  // Распознаёт цифры
}

bool letter(char value) 
{
    return (value >= 'a' && value <= 'z') || (value >= 'A' && value <= 'Z') || value == '_'; // Распознаёт буквы и нижнее подчёркивание
}

bool space(char value) 
{
    return value == ' ' || value == '\t' || value == '\r' || value == '\n'; // Распознаёт пробел, табуляцию и переводы строк
}

std::runtime_error error(std::string const& message, std::size_t position) 
{
    return std::runtime_error(message + " (позиция " + std::to_string(position + 1) + ")"); 
}

}

Lexer::Lexer(std::string const& input) : input_(input)
{
}

Token Lexer::next() 
{
    while (position_ < input_.size() && space(input_[position_])) 
    {
        // Пропускаем пробельные символы
        ++position_;
    }

    // Создаётся токен и запоминается его начальная позиция
    Token token;
    token.position = position_;
    if (position_ == input_.size()) 
    {
        return token;
    }

    /*
    Если символы ещё остались:
        Читает цифры до точки
        Если встречает точку — пропускает её
        Читает цифры после точки
        Проверяет, встретилась ли хотя бы одна цифра
    */
    char value = input_[position_];
    if (digit(value) || value == '.') 
    {
        bool hasDigit = false;
        while (position_ < input_.size() && digit(input_[position_])) 
        {
            hasDigit = true;
            ++position_;
        }
        if (position_ < input_.size() && input_[position_] == '.') 
        {
            ++position_;
            while (position_ < input_.size() && digit(input_[position_])) 
            {
                hasDigit = true;
                ++position_;
            }
        }
        if (!hasDigit) 
        {
            throw error("Ожидалось число", token.position);
        }

        // После выделения числа извлекаем часть строки
        token.kind = TokenKind::Number;
        token.text = input_.substr(token.position, position_ - token.position);
        try 
        {
            token.number = std::stod(token.text); // Преобразуем строку в double
        }
        catch (std::exception const&) 
        {
            throw error("Число выходит за допустимый диапазон", token.position);
        }
        return token;
    }

    // Если встретили букву или "_"
    if (letter(value)) 
    {
        ++position_;
        while (position_ < input_.size() && (letter(input_[position_]) || digit(input_[position_]))) 
        {
            ++position_;
        }
        token.kind = TokenKind::Identifier;
        token.text = input_.substr(token.position, position_ - token.position);
        return token;
    }

    // Операции и скобки
    ++position_;
    switch (value) 
    {
    case '+': token.kind = TokenKind::Plus; break;
    case '-': token.kind = TokenKind::Minus; break;
    case '*': token.kind = TokenKind::Multiply; break;
    case '/': token.kind = TokenKind::Divide; break;
    case '^': token.kind = TokenKind::Power; break;
    case '(': token.kind = TokenKind::LeftParenthesis; break;
    case ')': token.kind = TokenKind::RightParenthesis; break;
    default: throw error("Недопустимый символ", token.position);
    }
    return token;
}

}
