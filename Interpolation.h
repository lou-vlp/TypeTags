#pragma once

// This file defines interpolation behavior for the types that we may want to interpolate,
// as well as a list of allowed parameter types.
//
// The position of a parameter type in this list is used as a type GUID.

template <typename T>
struct Interpolant;

// Rely on std::lerp for floating-point types
template <typename T> requires std::floating_point<T>
struct Interpolant<T>
{
    static T Mix(const T& x, const T& y, float t) noexcept
    {
        return std::lerp(x, y, t);
    }
};

// Integral types are temporarily cast to double and then quantized back.
template <typename T> requires std::integral<T>
struct Interpolant<T>
{
    static T Mix(const T& x, const T& y, float t) noexcept
    {
        return static_cast<T>(std::lerp(static_cast<double>(x), static_cast<double>(y), t));
    }
};

// Enforce globally consistent type indices
static const TypeList<int, float, double> KeyframableTypes;
