#pragma once

using CalculatorEvaluate = double (*)(double);

struct CalculatorPlugin 
{
    const char* name;
    CalculatorEvaluate evaluate;
};
