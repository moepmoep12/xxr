#pragma once

#include <unordered_set>

#include "classifier.h"

template <class Symbol, typename Action>
class ClassifierPtrSet
{
private:
    using ClassifierPtr = std::shared_ptr<Classifier<Symbol, Action>>;
    std::unordered_set<ClassifierPtr> m_set;

public:
    ClassifierPtrSet() = default;

    ClassifierPtrSet(const ClassifierPtrSet<Symbol, Action> & obj) : m_set(obj.m_set) {}

    explicit ClassifierPtrSet(const std::unordered_set<ClassifierPtr> & obj) : m_set(obj) {}

    virtual ~ClassifierPtrSet() = default;

    auto empty() const noexcept
    {
        return m_set.empty();
    }

    auto size() const noexcept
    {
        return m_set.size();
    }

    auto begin() const noexcept
    {
        return m_set.begin();
    }

    auto end() const noexcept
    {
        return m_set.end();
    }

    auto cbegin() const noexcept
    {
        return m_set.cbegin();
    }

    auto cend() const noexcept
    {
        return m_set.cend();
    }

    template <class... Args>
    auto insert(Args... args)
    {
        return m_set.insert(args...);
    }

    template <class... Args>
    auto erase(Args... args)
    {
        return m_set.erase(args...);
    }

    void clear() noexcept
    {
        m_set.clear();
    }

    template <class... Args>
    void swap(Args... args)
    {
        m_set.swap(args...);
    }

    template <class... Args>
    auto find(Args... args) const
    {
        return m_set.find(args...);
    }

    template <class... Args>
    auto count(Args... args) const
    {
        return m_set.count(args...);
    }
};