#include "calculator/Application.hpp"
#include "calculator/Parser.hpp"

#include <iomanip>
#include <iostream>
#include <stdexcept>

namespace calculator
{
namespace 
{

std::filesystem::path executableDirectory() 
{
    wchar_t path[MAX_PATH];
    DWORD length = GetModuleFileNameW(nullptr, path, MAX_PATH);
    if (length == 0 || length >= MAX_PATH) 
    {
        throw std::runtime_error("Не удалось получить путь к calc.exe");
    }
    return std::filesystem::path(path).parent_path();
}

}

int Application::run() 
{
    // Настройка кодировки консоли для ввода и вывода
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    try 
    {
        // Загрузка плагинов
        for (auto const& message : plugins_.loadDirectory(executableDirectory() / "plugins")) 
        {
            std::cerr << message << '\n'; // Выводим ошибки, если они есть
        }

        std::cout << "Введите выражение. Для выхода введите exit.\n";
        std::cout << std::setprecision(15); // Устанавливаем точность вывода чисел до 15 значащих цифр

        std::string line;
        while (std::cout << "> " && std::getline(std::cin, line)) 
        {
            if (line == "exit") 
            {
                break;
            }
            
            if (line.find_first_not_of(" \t\r\n") == std::string::npos) 
            {
                continue; // Пропускаем пустой ввод
            }
            
            try 
            {
                Parser parser(line, plugins_);
                double result = parser.evaluate();
                std::cout << "= " << result << '\n';
            } 
            catch (std::exception const& error) 
            {
                std::cerr << "Ошибка: " << error.what() << '\n';
            } 
            catch (...) 
            {
                std::cerr << "Ошибка: неизвестное исключение\n";
            }
        }
        if (std::cin.bad()) 
        {
            throw std::runtime_error("Ошибка чтения ввода");
        }
        return 0;
    } 
    catch (std::exception const& error) 
    {
        std::cerr << "Не удалось запустить калькулятор: " << error.what() << '\n';
    } 
    catch (...) 
    {
        std::cerr << "Не удалось запустить калькулятор: неизвестное исключение\n";
    }
    return 1;
}

}
