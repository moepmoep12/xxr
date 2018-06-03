#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <cassert>

template <class T>
class AbstractSymbol
{
public:
    virtual ~AbstractSymbol() = default;

    virtual std::string toString() const = 0;

    friend std::ostream & operator<< (std::ostream & os, const AbstractSymbol & obj)
    {
        return os << obj.toString();
    }

    virtual bool matches(const T & symbol) const = 0;
};

// The standard symbol for XCS (with "don't care")
template <typename T>
class Symbol : public AbstractSymbol<T>
{
private:
    T m_value;
    bool m_isDontCare;
    T value() const
    {
        assert(!m_isDontCare);
        return m_value;
    }

public:
    Symbol(const T & value) : m_value(value), m_isDontCare(false) {}

    Symbol(char c);

    Symbol(const Symbol<T> & obj) : m_value(obj.m_value), m_isDontCare(obj.m_isDontCare) {}

    virtual ~Symbol() = default;

    virtual bool isDontCare() const
    {
        return m_isDontCare;
    }

    std::string toString() const override
    {
        if (isDontCare())
            return "#";
        else
            return std::to_string(value());
    }

    friend bool operator== (const Symbol<T> & lhs, const Symbol<T> & rhs)
    {
        return lhs.isDontCare() == rhs.isDontCare() && (lhs.isDontCare() || lhs.value() == rhs.value());
    }

    friend bool operator!= (const Symbol<T> & lhs, const Symbol<T> & rhs)
    {
        return !(lhs == rhs);
    }

    Symbol<T> & operator= (const Symbol<T> & obj)
    {
        m_value = obj.m_value;
        m_isDontCare = obj.m_isDontCare;
        return *this;
    }

    bool matches(const T & value) const override
    {
        return isDontCare() || this->value() == value;
    }

    void generalize()
    {
        m_isDontCare = true;
    }
};

Symbol<bool>::Symbol(char c) : m_value(c != '0'), m_isDontCare(c == '#') {}