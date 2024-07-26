#pragma once

#include <memory>
#include <bit>
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
    const T& Get() const noexcept;

    template <Interpolatable T>
    T& Get();

    static Parameter Mix(const Parameter& a, const Parameter& b, float t);
    size_t TypeTag() const noexcept { return info.tag; }
    size_t Size() const noexcept { return info.size; }

private:
    TypeInfo info;
    std::unique_ptr<std::byte[]> storage;
};

template<Interpolatable T>
Parameter::Parameter(T x) : info(TypeMap::Get<T>())
{
    storage = std::make_unique<std::byte[]>(info.size);
    std::memcpy(storage.get(), std::bit_cast<std::byte*>(&x), info.size);
}

template <Interpolatable T>
const T& Parameter::Get() const noexcept
{
    assert(info == TypeDescriptor<T>::Info);
    return *std::bit_cast<T*>(storage.get());
}

template <Interpolatable T>
T& Parameter::Get()
{
    if (info != TypeDescriptor<T>::Info)
    {
        throw std::invalid_argument("type mismatch");
    }
    return *std::bit_cast<T*>(storage.get());
}