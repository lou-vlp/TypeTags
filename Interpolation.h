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

// Special treatment for ints
template<>
struct Interpolant<int>
{
    static int Lerp(const int& x, const int& y, float t) noexcept
    {
        return static_cast<int>(t * static_cast<float>(y - x) + float(x));
    }
};

// Enforce globally consistent type indices
const InitGlobalTypeIndices<int, float, double> KeyframableTypes;
