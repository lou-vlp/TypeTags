#pragma once

#include <cmath>
#include <memory>
#include <cstring>
#include <cassert>
#include <string>

#include "Parameter.h"
#include "Interpolation.h"

struct ParameterHelpers
{
public:
    static Parameter Mix(const Parameter& a, const Parameter& b, float t);
    static std::string Print(const Parameter& param);

private:
    template <size_t N = 0>
    static Parameter Mix(const Parameter& a, const Parameter& b, float t, size_t typeTag);

    template <size_t N>
    static std::string Print(const Parameter& param);
};

template <size_t N>
static Parameter ParameterHelpers::Mix(const Parameter& a, const Parameter& b, float t, size_t typeTag)
{
    if constexpr (N < KeyframableTypes.Count())
    {
        using Type = typename std::tuple_element<N, decltype(KeyframableTypes)::TypeTuple>::type;
        const auto info = TypeMap::Get<Type>();

        if (info.tag == typeTag)
        {
            const auto val = Interpolant<Type>::Mix(a.Get<Type>(), b.Get<Type>(), t);
            return Parameter(val);
        }
        return Mix<N + 1>(a, b, t, typeTag);
    }

    throw std::runtime_error("Type not found");
}

template <size_t N>
static std::string ParameterHelpers::Print(const Parameter& param)
{
    if constexpr (N < KeyframableTypes.Count())
    {
        using Type = typename std::tuple_element<N, decltype(KeyframableTypes)::TypeTuple>::type;
        if (TypeMap::Get<Type>().tag == param.TypeTag())
        {
            return std::to_string(param.Get<Type>());
        }
        return Print<N + 1>(param);
    }

    throw std::runtime_error("Type not found");
}
