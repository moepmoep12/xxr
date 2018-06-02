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

    virtual std::string toString() const = 0;

    friend std::ostream & operator<< (std::ostream & os, const AbstractSymbol & obj)
    {
        return os << obj.toString();
    }

    virtual bool contains(const Derived & symbol) const = 0;
};

// The standard symbol for XCS (with "don't care")
template <typename T>
class Symbol : public AbstractSymbol<Symbol<T>>
{
private:
    T m_value;
    bool m_isDontCare;

public:
    Symbol(const T & value) : m_value(value), m_isDontCare(false) {}

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
            return std::to_string(m_value);
    }

    friend bool operator== (const Symbol<T> & lhs, const Symbol<T> & rhs)
    {
        return lhs.isDontCare() == rhs.isDontCare() && lhs.m_value == rhs.m_value;
    }

    friend bool operator!= (const Symbol<T> & lhs, const Symbol<T> & rhs)
    {
        return lhs.isDontCare() != rhs.isDontCare() || lhs.m_value != rhs.m_value;
    }

    Symbol<T> & operator= (const Symbol<T> & obj)
    {
        m_value = obj.m_value;
        m_isDontCare = obj.m_isDontCare;
        return *this;
    }

    bool contains(const Symbol<T> & symbol) const override
    {
        return isDontCare() || (m_value == symbol.m_value && isDontCare() == symbol.isDontCare());
    }

    bool equals(const T & value) const
    {
        return !isDontCare() && m_value == value;
    }

    void generalize()
    {
        m_isDontCare = true;
    }
};