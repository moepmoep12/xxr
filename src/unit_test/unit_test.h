#pragma once

#include <iostream>

bool testStatus = true;

void expect(const std::string & testName, bool cond)
{
    if (!cond)
    {
        testStatus = false;
    }
    std::cout << (cond ? "[ PASS ]" : "[  NG  ]") << " " << testName << std::endl;
}