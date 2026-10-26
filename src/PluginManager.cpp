#include "calculator/PluginManager.hpp"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace calculator 
{
namespace 
{

bool isDll(std::filesystem::path const& path) 
{
    std::string extension = path.extension().u8string();
    for (char& character : extension) 
    {
        if (character >= 'A' && character <= 'Z') 
        {
            character = static_cast<char>(character - 'A' + 'a');
        }
    }
    return extension == ".dll";
}

}

std::vector<std::string> PluginManager::loadDirectory(std::filesystem::path const& directory) 
{
    std::vector<std::string> messages; // Список сообщений для ошибок
    try 
    {
        if (!std::filesystem::is_directory(directory)) 
        {
            messages.push_back("Папка plugins отсутствует. Доступна базовая арифметика.");
            return messages;
        }

        using EntryPoint = CalculatorPlugin const* (*)() noexcept; // Объявляем тип указателя на служебную функцию плагина

        // Перебираем файлы
        for (auto const& file : std::filesystem::directory_iterator(directory)) 
        {
            if (!file.is_regular_file() || !isDll(file.path())) 
            {
                continue; // Проверки пропускают всё, что не является обычным файлом DLL
            }
            try 
            {
                // Для каждого подходящего файла выполняем:
                SharedLibrary library(file.path()); // Создаётся объект, конструктор которого загружает DLL
                /* 
                symbol() получает адрес экспортированной функции
                reinterpret_cast<EntryPoint> приводит его к нашему типу указателя
                */      
                auto entry = reinterpret_cast<EntryPoint>(library.symbol("calculator_plugin"));
                auto const* plugin = entry(); // Вызываем служебную функцию

                if (plugin == nullptr || plugin->name == nullptr || plugin->name[0] == '\0' || plugin->evaluate == nullptr) 
                {
                    throw std::runtime_error("Плагин вернул некорректное описание");
                }
                std::string name = plugin->name; // Копируем имя в собственную строку
                if (functions_.find(name) != functions_.end()) 
                {
                    throw std::runtime_error("Имя функции уже занято: " + name);
                }
                /*
                Внутри LoadedPlugin: std::move(library) позволяет передать владение библиотекой в сохраняемый объект.
                Указатель на математическую функцию сохраняется рядом с ней.
                После завершения итерации локальная переменная library уничтожится, но DLL останется загруженной: 
                её владельцем уже является объект внутри functions_.
                */
                functions_.emplace(name, LoadedPlugin{std::move(library), plugin->evaluate}); // emplace() добавляет в map пару из имени и данных плагина
            } 
            catch (std::exception const& error) 
            {
                messages.push_back("Пропущен плагин " + file.path().filename().u8string() + ": " + error.what());
            } 
            catch (...) 
            {
                messages.push_back("Пропущен плагин " + file.path().filename().u8string() + ": неизвестное исключение");
            }
        }
        if (functions_.empty()) 
        {
            messages.push_back("Функции из DLL не загружены. Доступна базовая арифметика.");
        }
    } 
    catch (std::filesystem::filesystem_error const& error) 
    {
        messages.push_back(std::string("Не удалось прочитать папку plugins: ") + error.what());
    }
    return messages;
}

double PluginManager::call(std::string const& name, double argument) const 
{
    auto function = functions_.find(name);
    if (function == functions_.end()) 
    {
        throw std::runtime_error("Неизвестная функция: " + name);
    }

    /*
    function->first - Ключ: имя функции.
    function->second - Значение: LoadedPlugin.
    */
    double result = function->second.evaluate(argument);
    if (!std::isfinite(result)) 
    {
        throw std::runtime_error("Функция " + name + " вернула недопустимый результат");
    }
    return result;
}

}
