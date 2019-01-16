#include <iostream>

#include "unit_test.h"
#include "../XCS/condition.h"
#include "../XCS/symbol.h"
#include "../XCSR/condition.h"
#include "../XCSR_CS/symbol.h"
#include "../XCSR_LU/symbol.h"
#include "../XCSR_UB/symbol.h"

int main()
{
    std::cout << "XCS Symbol:" << std::endl;
    {
        XCS::Symbol<bool> symbol('0');
        expect("(char)'0'", !symbol.isDontCare() && symbol.toString() == "0");
    }
    {
        XCS::Symbol<bool> symbol('1');
        expect("(char)'1'", !symbol.isDontCare() && symbol.toString() == "1");
    }
    {
        XCS::Symbol<bool> symbol('#');
        expect("(char)'#'", symbol.isDontCare() && symbol.toString() == "#");
    }
    {
        XCS::Symbol<int> symbol(0);
        expect("(int)0", !symbol.isDontCare() && symbol.toString() == "0");
    }
    {
        XCS::Symbol<int> symbol(1);
        expect("(int)1", !symbol.isDontCare() && symbol.toString() == "1");
    }

    hr();

    std::cout << "XCS Condition:" << std::endl;
    expect("10#010", XCS::Condition<bool, XCS::Symbol<bool>>("10#010").toString() == "10#010");
    expect("#10##01#1##", XCS::Condition<bool, XCS::Symbol<bool>>("#10##01#1##").toString() == "#10##01#1##");

    if (testStatus)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
