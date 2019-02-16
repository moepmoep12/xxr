#include <iostream>
#include <xxr/environment/checkerboard_environment.hpp>

#include "unit_test.hpp"

using namespace xxr;

int main()
{
    std::cout << "chk(2, 2):" << std::endl;
    {
        CheckerboardEnvironment env(2, 2);
        expect("(0.0, 0.0)", env.getAnswer({ 0.0, 0.0 }) == 0);
        expect("(1.0, 0.0)", env.getAnswer({ 1.0, 0.0 }) == 1);
        expect("(0.0, 1.0)", env.getAnswer({ 0.0, 1.0 }) == 1);
        expect("(1.0, 1.0)", env.getAnswer({ 1.0, 1.0 }) == 0);
        expect("(0.0, 0.49)", env.getAnswer({ 0.0, 0.49 }) == 0);
        expect("(0.0, 0.51)", env.getAnswer({ 0.0, 0.51 }) == 1);
        expect("(0.49, 0.51)", env.getAnswer({ 0.49, 0.51 }) == 1);
        expect("(0.51, 0.51)", env.getAnswer({ 0.51, 0.51 }) == 0);
        for (int x = 0; x < 15; ++x)
        {
            for (int y = 0; y < 15; ++y)
            {
                std::cout << env.getAnswer({static_cast<double>(x) / 15, static_cast<double>(y) / 15}) ? "1" : "0";
            }
            std::cout << std::endl;
        }
    }
    hr();

    std::cout << "chk(2, 5):" << std::endl;
    {
        CheckerboardEnvironment env(2, 5);
        expect("(0.0, 0.0)", env.getAnswer({ 0.0, 0.0 }) == 0);
        expect("(0.21, 0.0)", env.getAnswer({ 0.21, 0.0 }) == 1);
        expect("(0.0, 0.21)", env.getAnswer({ 0.0, 0.21 }) == 1);
        expect("(0.39, 0.0)", env.getAnswer({ 0.39, 0.0 }) == 1);
        expect("(0.41, 0.0)", env.getAnswer({ 0.41, 0.0 }) == 0);
        expect("(0.41, 0.39)", env.getAnswer({ 0.41, 0.39 }) == 1);
        expect("(0.1, 0.79)", env.getAnswer({ 0.1, 0.79 }) == 1);
        expect("(0.1, 0.81)", env.getAnswer({ 0.1, 0.81 }) == 0);
        expect("(0.0, 1.0)", env.getAnswer({ 0.0, 1.0 }) == 0);
        expect("(1.0, 0.0)", env.getAnswer({ 1.0, 0.0 }) == 0);
        expect("(0.79, 0.79)", env.getAnswer({ 0.79, 0.79 }) == 0);
        expect("(0.79, 0.81)", env.getAnswer({ 0.79, 0.81 }) == 1);
        expect("(0.81, 0.81)", env.getAnswer({ 0.81, 0.81 }) == 0);
        expect("(1.0, 1.0)", env.getAnswer({ 1.0, 1.0 }) == 0);
        for (int x = 0; x < 15; ++x)
        {
            for (int y = 0; y < 15; ++y)
            {
                std::cout << env.getAnswer({static_cast<double>(x) / 15, static_cast<double>(y) / 15}) ? "1" : "0";
            }
            std::cout << std::endl;
        }
    }
    hr();

    std::cout << "chk(3, 3):" << std::endl;
    {
        CheckerboardEnvironment env(3, 3);
        expect("(0.0, 0.0, 0.0)", env.getAnswer({ 0.0, 0.0, 0.0 }) == 0);
        expect("(0.5, 0.0, 0.0)", env.getAnswer({ 0.5, 0.0, 0.0 }) == 1);
        expect("(0.0, 0.5, 0.0)", env.getAnswer({ 0.0, 0.5, 0.0 }) == 1);
        expect("(0.0, 0.0, 0.5)", env.getAnswer({ 0.0, 0.0, 0.5 }) == 1);
        expect("(0.5, 0.0, 0.5)", env.getAnswer({ 0.5, 0.0, 0.5 }) == 0);
        expect("(0.5, 0.5, 0.5)", env.getAnswer({ 0.5, 0.5, 0.5 }) == 1);
        expect("(1.0, 0.5, 0.5)", env.getAnswer({ 1.0, 0.5, 0.5 }) == 0);
        expect("(0.5, 1.0, 0.5)", env.getAnswer({ 0.5, 1.0, 0.5 }) == 0);
        expect("(0.5, 0.5, 1.0)", env.getAnswer({ 0.5, 0.5, 1.0 }) == 0);
        expect("(1.0, 1.0, 0.5)", env.getAnswer({ 1.0, 1.0, 0.5 }) == 1);
        expect("(1.0, 1.0, 1.0)", env.getAnswer({ 1.0, 1.0, 1.0 }) == 0);
        expect("(1.0, 0.0, 1.0)", env.getAnswer({ 1.0, 0.0, 1.0 }) == 0);
        expect("(1.0, 0.0, 0.5)", env.getAnswer({ 1.0, 0.0, 0.5 }) == 1);
        expect("(1.0, 0.0, 0.0)", env.getAnswer({ 1.0, 0.0, 0.0 }) == 0);
        expect("(0.2, 0.3, 0.4)", env.getAnswer({ 0.2, 0.3, 0.4 }) == 1);
        expect("(0.8, 0.7, 0.6)", env.getAnswer({ 0.8, 0.7, 0.6 }) == 1);
        expect("(0.3, 0.7, 0.9)", env.getAnswer({ 0.3, 0.7, 0.9 }) == 0);
        expect("(0.9, 0.3, 0.7)", env.getAnswer({ 0.9, 0.3, 0.7 }) == 0);
        expect("(0.67, 0.33, 0.66)", env.getAnswer({ 0.67, 0.33, 0.66 }) == 1);
        expect("(0.34, 0.66, 0.33)", env.getAnswer({ 0.34, 0.66, 0.33 }) == 0);
        for (int x = 0; x < 15; ++x)
        {
            for (int y = 0; y < 15; ++y)
            {
                std::cout << env.getAnswer({static_cast<double>(x) / 15, static_cast<double>(y) / 15, 0.0}) ? "1" : "0";
            }
            std::cout << std::endl;
        }
    }

    if (testStatus)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
