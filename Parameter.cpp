#include "Utils.h"
#include "Parameter.h"

Parameter Parameter::Mix(const Parameter& a, const Parameter& b, float t)
{
    if (a.info.tag != b.info.tag)
    {
        throw std::invalid_argument("type mismatch");
    }
    return ParameterHelpers::Mix(a, b, t);
}

Parameter::Parameter() noexcept : info(TypeInfo::Invalid)
{
}

Parameter::Parameter(Parameter&& other) noexcept : info(other.info), storage(std::move(other.storage))
{
}

Parameter::Parameter(const Parameter& other)
    : info(other.info)
{
    storage = std::make_unique<std::byte[]>(info.size);
    std::memcpy(storage.get(), other.storage.get(), info.size);
}

Parameter& Parameter::operator=(const Parameter& other)
{
    if (this != &other)
    {
        info = other.info;
        storage = std::make_unique<std::byte[]>(info.size);
        std::memcpy(storage.get(), other.storage.get(), info.size);
    }
    return *this;
}

Parameter& Parameter::operator=(Parameter&& other) noexcept
{
    if (this != &other)
    {
        info = other.info;
        storage = std::move(other.storage);
    }
    return *this;
}

Parameter::~Parameter()
{
    storage.reset();
}
