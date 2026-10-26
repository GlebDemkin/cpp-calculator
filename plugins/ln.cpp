#include "calculator/plugin_api.hpp"

#include <cmath>
#include <stdexcept>

namespace 
{

double evaluateLn(double value) 
{
    if (value <= 0) 
    {
        throw std::domain_error("Для ln аргумент должен быть больше нуля");
    }
    return std::log(value);
}

CalculatorPlugin const plugin{"ln", evaluateLn};

}

extern "C" __declspec(dllexport) CalculatorPlugin const* calculator_plugin() noexcept 
{
    return &plugin;
}
