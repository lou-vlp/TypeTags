#pragma once

// Explicitly provide interpolation behavior for things we care to interpolate
template <typename T>
struct Interpolant;

// Rely on std::lerp for floating-point types
template <typename T> requires std::floating_point<T>
struct Interpolant<T>
{
    static T Lerp(const T& x, const T& y, float t) noexcept
    {
        return std::lerp(x, y, t);
    }
};

// Integral types are temporarily cast to double and then quantized back.
template <typename T> requires std::integral<T>
struct Interpolant<T>
{
    static T Lerp(const T& x, const T& y, float t) noexcept
    {
        return static_cast<T>(std::lerp(static_cast<double>(x), static_cast<double>(y), t));
    }
};

// Enforce globally consistent type indices
const InitGlobalTypeIndices<int, float, double> KeyframableTypes;
