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
    static Parameter Lerp(const Parameter& a, const Parameter& b, float t)
    {
        assert(a.TypeTag() == b.TypeTag());
        return Lerp(a, b, t, a.TypeTag());
    }

    static std::string Print(const Parameter& param)
    {
        return Print<0>(param);
    }

private:
    template <size_t N = 0>
    static Parameter Lerp(const Parameter& a, const Parameter& b, float t, size_t typeTag)
    {
        if constexpr (N < KeyframableTypes.Count())
        {
            using Type = typename std::tuple_element<N, decltype(KeyframableTypes)::TypeList>::type;
            const auto info = TypeMap::Get<Type>();

            if (info.tag == typeTag)
            {
                const auto val = Interpolant<Type>::Lerp(a.Get<Type>(), b.Get<Type>(), t);
                return Parameter(val);
            }
            return Lerp<N + 1>(a, b, t, typeTag);
        }

        throw std::runtime_error("Type not found");
    }

    template <size_t N>
    static std::string Print(const Parameter& param)
    {
        if constexpr (N < KeyframableTypes.Count())
        {
            using Type = typename std::tuple_element<N, decltype(KeyframableTypes)::TypeList>::type;
            if (TypeMap::Get<Type>().tag == param.TypeTag())
            {
                return std::to_string(param.Get<Type>());
            }
            return Print<N + 1>(param);
        }

        throw std::runtime_error("Type not found");
    }
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

Parameter Parameter::Lerp(const Parameter& a, const Parameter& b, float t)
{
    if (a.info.tag != b.info.tag)
    {
        throw std::invalid_argument("type mismatch");
    }
    return ParameterHelpers::Lerp(a, b, t);
}
