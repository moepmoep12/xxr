#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

template <class Derived>
class AbstractSymbol
{
public:
    virtual ~AbstractSymbol() = default;

    virtual bool isDontCare() const = 0;

    virtual std::string toString() const = 0;

    friend std::ostream & operator<< (std::ostream & os, const AbstractSymbol & obj)
    {
        return os << obj.toString();
    }

    virtual bool contains(const Derived & symbol) const = 0;

    virtual void generalize() = 0;
};

class BinarySymbol : public AbstractSymbol<BinarySymbol>
{
private:
    enum class Tribool {
        Zero = 0,
        One,
        DontCare
    } m_condition;

public:
    BinarySymbol(char c) : m_condition(c == '0' ? Tribool::Zero : c == '#' ? Tribool::DontCare : Tribool::One) {}

    BinarySymbol(int b) : m_condition(b ? Tribool::One : Tribool::Zero) {}

    BinarySymbol(const std::string & c) : m_condition(c == "0" ? Tribool::Zero : c == "#" ? Tribool::DontCare : Tribool::One) {}

    bool isDontCare() const override
    {
        return m_condition == Tribool::DontCare;
    }

    char toChar() const
    {
        if (isDontCare())
            return '#';
        else if (m_condition == Tribool::Zero)
            return '0';
        else
            return '1';
    }

    std::string toString() const override
    {
        if (isDontCare())
            return "#";
        else if (m_condition == Tribool::Zero)
            return "0";
        else
            return "1";
    }

    friend bool operator== (const BinarySymbol & lhs, const BinarySymbol & rhs)
    {
        return lhs.m_condition == rhs.m_condition;
    }

    friend bool operator!= (const BinarySymbol & lhs, const BinarySymbol & rhs)
    {
        return rhs != lhs;
    }

    bool contains(const BinarySymbol & symbol) const override
    {
        return isDontCare() || m_condition == symbol.m_condition;
    }

    void generalize() override
    {
        m_condition = Tribool::DontCare;
    }
};