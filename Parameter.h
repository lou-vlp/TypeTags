#pragma once

#include <any>
#include "TypeInfo.h"

// Interpolatable parameters
class Parameter
{
public:
    Parameter() noexcept;
    Parameter(Parameter&& other) noexcept;
    Parameter(const Parameter& other);

    template<Interpolatable T>
    explicit Parameter(T x);

    ~Parameter();

    Parameter& operator=(const Parameter& other);
    Parameter& operator=(Parameter&& other) noexcept;

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

template<Interpolatable T>
Parameter::Parameter(T x) : info(TypeMap::Get<T>())
{
    value = x;
}

template <Interpolatable T>
const T Parameter::Get() const noexcept
{
    assert(info == TypeDescriptor<T>::Info);
    return std::any_cast<T>(value);
}

template <Interpolatable T>
T& Parameter::Get()
{
    if (info != TypeDescriptor<T>::Info)
    {
        throw std::invalid_argument("type mismatch");
    }
    return std::any_cast<T>(value);
}