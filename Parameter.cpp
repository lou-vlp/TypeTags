#include "Utils.h"
#include "Parameter.h"

Parameter Parameter::Lerp(const Parameter& a, const Parameter& b, float t)
{
    if (a.info.tag != b.info.tag)
    {
        throw std::invalid_argument("type mismatch");
    }
    return ParameterHelpers::Lerp(a, b, t);
}

Parameter::Parameter() noexcept : info(TypeInfo::Invalid)
{
}

Parameter::Parameter(Parameter&& other) noexcept : info(other.info), value(std::move(other.value))
{
}

Parameter::Parameter(const Parameter& other)
    : info(other.info)
    , value(other.value)
{
}

Parameter& Parameter::operator=(const Parameter& other)
{
    if (this != &other)
    {
        info = other.info;
        value = other.value;
    }
    return *this;
}

Parameter& Parameter::operator=(Parameter&& other) noexcept
{
    if (this != &other)
    {
        info = other.info;
        value = std::move(other.value);
    }
    return *this;
}

Parameter::~Parameter()
{
    value.reset();
}
