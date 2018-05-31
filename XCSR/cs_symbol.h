#pragma once

#include "../XCS/symbol.h"

class CSSymbol : public AbstractSymbol<CSSymbol>
{
private:
    double m_c;
    double m_s;

public:
    CSSymbol(double c, double s) : m_c(c), m_s(s) {}

    bool isDontCare() const override
    {
        return false;
    }
};