#pragma once

#include <compare>
#include <concepts>
#include <stdexcept>
#include <unordered_map>

// Explicitly provide interpolation behavior for things we care to interpolate
template <typename T>
struct Interpolant;

// Constrain operations to things that are interpolatable
template<typename T>
concept Interpolatable = requires(const T& a, const T& b, float t)
{
    { Interpolant<T>::Lerp(a, b, t) } -> std::same_as<T>;
};

// Type-indexed type descriptor
struct TypeInfo
{
    static const TypeInfo Invalid;
    size_t tag;
    size_t size;

    friend constexpr auto operator<=>(TypeInfo, TypeInfo) = default;
};
const TypeInfo TypeInfo::Invalid = { static_cast<size_t>(-1), 0 };

// C++ type to TypeInfo mapping
template<Interpolatable T>
class TypeDescriptor
{
public:
    inline static TypeInfo Info;
};

// Keeps track of supported types.
class TypeMap
{
public:
    template <Interpolatable T>
    static TypeInfo Get()
    {
        const auto& info = TypeDescriptor<T>::Info;
        if (!Types.contains(typeid(T).hash_code()))
        {
            throw std::runtime_error("undefined type");
        }
        return info;
    }

private:
    template <Interpolatable... Types>
    friend class InitGlobalTypeIndices;

    template <Interpolatable T>
    static TypeInfo Fresh()
    {
        TypeInfo& info = TypeDescriptor<T>::Info;
        auto hash = typeid(T).hash_code();
        if (!Types.contains(hash))
        {
            info.tag = Count++;
            info.size = sizeof(T);
            Types[hash] = info;
        }
        return info;
    }

    inline static int Count = 0;
    inline static std::unordered_map<size_t, TypeInfo> Types;
};

// Type registration.  Used to generate type GUIDs.
template <Interpolatable... Types>
class InitGlobalTypeIndices
{
public:
    InitGlobalTypeIndices()
    {
        AddTypes();
    }

    static constexpr size_t Count() { return sizeof...(Types); }
    using TypeList = std::tuple<Types...>;

private:
    template <size_t N = 0>
    void AddTypes()
    {
        if constexpr (N < Count())
        {
            using Type = typename std::tuple_element<N, TypeList>::type;
            TypeDescriptor<Type>::Info = TypeMap::Fresh<Type>();
            AddTypes<N + 1>();
        }
    }
};
