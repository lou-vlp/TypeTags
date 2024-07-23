#pragma once

#include <any>
#include "TypeInfo.h"

// Interpolatable parameters
class Parameter
{
public:
    Parameter() noexcept : info(TypeInfo::Invalid)
    {
    }

    Parameter(Parameter&& other) noexcept : info(other.info), value(std::move(other.value))
    {
    }

    Parameter(const Parameter& other)
        : info(other.info)
        , value(other.value)
    {
    }

    Parameter& operator=(const Parameter& other)
    {
        if (this != &other)
        {
            info = other.info;
            value = other.value;
        }
        return *this;
    }

    Parameter& operator=(Parameter&& other) noexcept
    {
        if (this != &other)
        {
            info = other.info;
            value = std::move(other.value);
        }
        return *this;
    }

    template<Interpolatable T>
    explicit Parameter(T x);

    ~Parameter()
    {
        value.reset();
    }

    template <Interpolatable T>
    const T Get() const noexcept;

    template <Interpolatable T>
    T& Get();

    static Parameter Lerp(const Parameter& a, const Parameter& b, float t);
    size_t TypeTag() const noexcept { return info.tag; }
    size_t Size() const noexcept { return info.size; }

private:
    TypeInfo info;
    std::any value;
};
