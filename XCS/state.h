#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstddef>
#include <cassert>

#include "symbol.h"

template <class S = BinarySymbol>
class State
{
private:
	std::vector<S> m_symbols;

public:
	State(const std::vector<S> symbols) : m_symbols(symbols) {}

	State(const std::string symbols)
	{
		for (const char symbol : symbols)
		{
			m_symbols.push_back(symbol);
		}
	}

	std::string toString() const
	{
		std::string str;
		for (auto && symbol : m_symbols)
		{
			str += symbol.toString();
		}
		return str;
	}

	friend std::ostream & operator<< (std::ostream & os, const State & obj)
	{
		return os << obj.toString();
	}

	friend bool operator== (const State & lhs, const State & rhs)
	{
		return lhs.m_symbols == rhs.m_symbols;
	}

	friend bool operator!= (const State & lhs, const State & rhs)
	{
		return lhs.m_symbols != rhs.m_symbols;
	}

	bool contains(const State & classifier) const
	{
		assert(std::size(m_symbols) == std::size(classifier.m_symbols));

		for (size_t i = 0; i < std::size(m_symbols); ++i)
		{
			if (!m_symbols[i].contains(classifier.m_symbols[i]))
			{
				return false;
			}
		}
		
		return true;
	}

	size_t size() const
	{
		return std::size(m_symbols);
	}
};