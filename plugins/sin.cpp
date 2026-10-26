#include "calculator/plugin_api.hpp"

#include <cmath>

namespace 
{

double evaluateSin(double degrees) 
{
    constexpr double pi = 3.141592653589793;
    return std::sin(std::remainder(degrees, 360.0) * pi / 180.0); // Стандартная std::sin() принимает радианы, поэтому мы переводим градусы
}

CalculatorPlugin const plugin{"sin", evaluateSin};

}

extern "C" __declspec(dllexport) CalculatorPlugin const* calculator_plugin() noexcept 
{
    return &plugin;
}
