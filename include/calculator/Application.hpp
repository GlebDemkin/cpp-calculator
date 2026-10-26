#pragma once

#include "calculator/PluginManager.hpp"

namespace calculator 
{

class Application 
{
public:
    int run();

private:
    PluginManager plugins_; // Отвечает за загрузку библиотек и поиск функций по имени
};

}
