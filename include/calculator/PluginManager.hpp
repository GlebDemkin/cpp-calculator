#pragma once

#include "calculator/SharedLibrary.hpp"
#include "calculator/plugin_api.hpp"

#include <map>
#include <string>
#include <vector>

namespace calculator 
{

class PluginManager 
{
public:
    std::vector<std::string> loadDirectory(std::filesystem::path const& directory);
    double call(std::string const& name, double argument) const;

private:
    struct LoadedPlugin  // Объединяет владельца DLL и указатель на математическую функцию
    {
        SharedLibrary library;
        CalculatorEvaluate evaluate;
    };

    std::map<std::string, LoadedPlugin> functions_;
};

}
