#pragma once

#include <cassert>

template <typename T>
class AbstractFilter
{
public:
	virtual T operator()(T value) = 0;
};

template <typename T>
class UnrecursiveFilter : public Filter<T>
{
protected:
	const unsigned int m_order; // How many samples to use
	unsigned int m_cursor;
	unsigned int m_valueCount;

    // Ring buffer (Use dynamic allocation to place in the heap area in case of a large order)
	T *m_pBuffer;

	explicit UnrecursiveFilter(unsigned int order) : m_order(order), m_cursor(0), m_valueCount(0)
	{
		// Filter order must not be zero
		assert(m_order > 0);

		m_pBuffer = new T[m_order];
	}

	~UnrecursiveFilter()
	{
		delete[] m_pBuffer;
	}

	// Make sure to all this first in operator()
	void storeValue(T value)
	{
		// Store the given value to the buffer
		m_pBuffer[m_cursor] = value;

		// Increment the value count
		if (m_valueCount < m_order)
			++m_valueCount;

		// Move the cursor to the next
		if (++m_cursor >= m_order)
			m_cursor = 0;
	}

public:
	UnrecursiveFilter(const UnrecursiveFilter &) = delete;
	UnrecursiveFilter & operator=(const UnrecursiveFilter &) = delete;

	virtual T operator()(T value) = 0;
};

// Simple Moving Average
template <typename T>
class SimpleMovingAverage : public UnrecursiveFilter<T>
{
private:
	using UnrecursiveFilter<T>::m_valueCount;
	using UnrecursiveFilter<T>::m_pBuffer;

public:
	explicit AverageFilter(unsigned int order) : UnrecursiveFilter<T>(order) {}

	T operator()(T value) override
	{
		this->storeValue(value);

		// Total sum
		T sum = m_pBuffer[0];
		for (int i = 1; i < m_valueCount; ++i)
		{
			sum += m_pBuffer[i];
		}

		// Return the average
		return sum / m_valueCount;
	}
};