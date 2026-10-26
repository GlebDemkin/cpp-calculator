#include "calculator/plugin_api.hpp"

#include <cmath>
#include <stdexcept>

namespace 
{

double evaluateSqrt(double value) 
{
    if (value < 0)
    {
        throw std::domain_error("Для sqrt аргумент должен быть неотрицательным");
    }
    return std::sqrt(value);
}

CalculatorPlugin const plugin{"sqrt", evaluateSqrt};

}

extern "C" __declspec(dllexport) CalculatorPlugin const* calculator_plugin() noexcept 
{
    return &plugin;
}
