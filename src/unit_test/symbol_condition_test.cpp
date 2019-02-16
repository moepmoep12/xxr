#include <iostream>
#include <xxr/xcs/condition.hpp>
#include <xxr/xcs/symbol.hpp>
#include <xxr/xcsr/condition.hpp>
#include <xxr/xcsr/csr/symbol.hpp>
#include <xxr/xcsr/obr/symbol.hpp>
#include <xxr/xcsr/ubr/symbol.hpp>

#include "unit_test.hpp"

using namespace xxr;

int main()
{
    std::cout << "XCS Symbol:" << std::endl;
    {
        xcs_impl::Symbol<bool> symbol('0');
        expect("(char)'0'", !symbol.isDontCare() && symbol.toString() == "0");
    }
    {
        xcs_impl::Symbol<bool> symbol('1');
        expect("(char)'1'", !symbol.isDontCare() && symbol.toString() == "1");
    }
    {
        xcs_impl::Symbol<bool> symbol('#');
        expect("(char)'#'", symbol.isDontCare() && symbol.toString() == "#");
    }
    {
        xcs_impl::Symbol<int> symbol(0);
        expect("(int)0", !symbol.isDontCare() && symbol.toString() == "0");
    }
    {
        xcs_impl::Symbol<int> symbol(1);
        expect("(int)1", !symbol.isDontCare() && symbol.toString() == "1");
    }

    hr();

    std::cout << "XCS Condition:" << std::endl;
    expect("10#010", xcs_impl::Condition<bool, xcs_impl::Symbol<bool>>("10#010").toString() == "10#010");
    expect("#10##01#1##", xcs_impl::Condition<bool, xcs_impl::Symbol<bool>>("#10##01#1##").toString() == "#10##01#1##");

    if (testStatus)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
