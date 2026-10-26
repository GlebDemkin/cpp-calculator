#include "calculator/Parser.hpp"

#include <cmath>
#include <stdexcept>

namespace calculator 
{
namespace 
{

std::runtime_error error(std::string const& message, std::size_t position) 
{
    return std::runtime_error(message + " (позиция " + std::to_string(position + 1) + ")");
}

double checked(double value, std::size_t position)  // Проверяет, что результат конечный: не получилась бесконечность или NaN
{
    if (!std::isfinite(value)) 
    {
        throw error("Недопустимый результат или переполнение", position);
    }
    return value;
}

}

Parser::Parser(std::string const& input, PluginManager const& plugins) : lexer_(input), plugins_(plugins) 
{
    readNextToken(); // Первый вызов в конструкторе нужен, чтобы к началу вычисления current_ уже содержал первый токен
}

void Parser::expect(TokenKind kind, std::string const& message) 
{
    if (current_.kind != kind) 
    {
        throw error(message, current_.position);
    }
    readNextToken();
}

double Parser::evaluate() 
{
    double result = parseSum();
    if (current_.kind != TokenKind::End) 
    {
        throw error("Лишние символы или пропущен знак операции", current_.position);
    }
    return result;
}

double Parser::parseSum() 
{
    double result = parseProduct();
    while (current_.kind == TokenKind::Plus || current_.kind == TokenKind::Minus) 
    {
        Token operation = current_;
        readNextToken();
        double right = parseProduct();
        if (operation.kind == TokenKind::Plus) 
        {
            result = checked(result + right, operation.position);
        } 
        else 
        {
            result = checked(result - right, operation.position);
        }
    }
    return result;
}

double Parser::parseProduct() 
{
    double result = parseUnary();
    while (current_.kind == TokenKind::Multiply || current_.kind == TokenKind::Divide) 
    {
        Token operation = current_;
        readNextToken();
        double right = parseUnary();
        if (operation.kind == TokenKind::Multiply) 
        {
            result = checked(result * right, operation.position);
        } 
        else 
        {
            if (right == 0) 
            {
                throw error("Деление на ноль", operation.position);
            }
            result = checked(result / right, operation.position);
        }
    }
    return result;
}

double Parser::parseUnary() 
{
    // Ограничение защищает стек вызовов при слишком глубокой вложенности
    if (++depth_ > 256) // depth_ считает незавершённые вызовы parseUnary()
    {
        throw error("Слишком большая вложенность выражения", current_.position);
    }

    double result;
    if (current_.kind == TokenKind::Plus) 
    {
        readNextToken();
        result = parseUnary(); // Метод вызывает самого себя, чтобы поддерживать последовательность знаков
    }
    else if (current_.kind == TokenKind::Minus) 
    {
        readNextToken();
        result = -parseUnary();
    }
    else
    {
        result = parsePower();
    }
    --depth_; // Перед нормальным выходом из метода счётчик уменьшается
    return result;
}

double Parser::parsePower() 
{
    double base = parsePrimary(); // Получаем основание степени
    if (current_.kind != TokenKind::Power) 
    {
        return base;
    }

    std::size_t position = current_.position;
    readNextToken();
    double exponent = parseUnary(); // Показатель читается через parseUnary(), поэтому допустим отрицательный знак
    if (std::trunc(exponent) != exponent) // std::trunc() отбрасывает дробную часть
    {
        throw error("Показатель степени должен быть целым", position);
    }
    if (base == 0 && exponent <= 0) 
    {
        throw error("Нельзя вычислить 0^0 или ноль в отрицательной степени", position);
    }
    return checked(std::pow(base, exponent), position);
}

double Parser::parsePrimary()
{
    if (current_.kind == TokenKind::Number) 
    {
        double value = current_.number;
        readNextToken();
        return value;
    }
    if (current_.kind == TokenKind::LeftParenthesis) 
    {
        readNextToken();
        double value = parseSum();
        expect(TokenKind::RightParenthesis, "Ожидалась закрывающая скобка ')'"); // Проверяет закрывающую скобку и переходит дальше
        return value;
    }
    if (current_.kind == TokenKind::Identifier) 
    {
        std::string name = current_.text;
        readNextToken();
        expect(TokenKind::LeftParenthesis, "После имени функции ожидалась скобка '('");
        double argument = parseSum();
        expect(TokenKind::RightParenthesis, "После аргумента ожидалась скобка ')'");
        return plugins_.call(name, argument); // Менеджер плагинов находит функцию и вызывает её
    }
    throw error("Ожидалось число, функция или выражение в скобках", current_.position);
}

}
