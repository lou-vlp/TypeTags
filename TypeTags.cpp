#include <iostream>
#include <vector>
#include <unordered_map>
#include <bit>
#include <cmath>
#include <memory>
#include <cstring>
#include <cassert>
#include <string>

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

// Constrain operations to things that are interpolatable
template<typename T>
concept Interpolatable = requires(const T & a, const T & b, float t)
{
    { Interpolant<T>::Lerp(a, b, t) } -> std::convertible_to<T>;
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



template<Interpolatable T>
class TypeDescriptor
{
public:
    inline static TypeInfo Info;
};

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

template <Interpolatable... Types>
class InitGlobalTypeIndices
{
public:
    InitGlobalTypeIndices()
    {
        Add();
    }

    static constexpr size_t Count() { return sizeof...(Types); }
    using TypeList = std::tuple<Types...>;

private:
    template <size_t N = 0>
    void Add()
    {
        if constexpr (N < Count())
        {
            using Type = typename std::tuple_element<N, TypeList>::type;
            TypeDescriptor<Type>::Info = TypeMap::Fresh<Type>();
            Add<N + 1>();
        }
    }
};

// Enforce globally consistent type indices
const InitGlobalTypeIndices<int, float, double> KeyframableTypes;

// Interpolatable parameters
class Parameter
{
public:
    Parameter() noexcept : info(TypeInfo::Invalid), storage(nullptr)
    {
    }

    Parameter(Parameter&& other) noexcept : info(other.info), storage(std::move(other.storage))
    {
    }

    Parameter(const Parameter& other)
        : info(other.info)
        , storage(std::make_unique<std::byte[]>(other.info.size))
    {
        std::memcpy(storage.get(), other.storage.get(), info.size);
    }

    Parameter& operator=(const Parameter& other)
    {
        if (this != &other)
        {
            info = other.info;
            storage = std::make_unique<std::byte[]>(info.size);
            std::memcpy(storage.get(), other.storage.get(), info.size);
        }
        return *this;
    }

    Parameter& operator=(Parameter&& other) noexcept
    {
        if (this != &other)
        {
            info = other.info;
            storage.reset();
            storage = std::move(other.storage);
        }
        return *this;
    }

    template<Interpolatable T>
    explicit Parameter(T x);

    ~Parameter()
    {
        storage.reset();
    }

    template <Interpolatable T>
    const T& Get() const noexcept;

    template <Interpolatable T>
    T& Get();

    static Parameter Lerp(const Parameter& a, const Parameter& b, float t);
    size_t TypeTag() const noexcept { return info.tag; }
    size_t Size() const noexcept { return info.size; }

private:
    TypeInfo info;
    std::unique_ptr<std::byte[]> storage;
};

struct ParameterHelpers
{
public:
    static Parameter Lerp(const Parameter& a, const Parameter& b, float t)
    {
        assert(a.TypeTag() == b.TypeTag());
        return Apply(a, b, t, a.TypeTag());
    }

    static std::string Print(const Parameter& param)
    {
        return Print<0>(param);
    }

private:
    template <size_t N = 0>
    static Parameter Apply(const Parameter& a, const Parameter& b, float t, size_t typeTag)
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
            return Apply<N + 1>(a, b, t, typeTag);
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
    storage = std::make_unique<std::byte[]>(info.size);
    *std::bit_cast<T*>(storage.get()) = x;
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

Parameter Parameter::Lerp(const Parameter& a, const Parameter& b, float t)
{
    if (a.info.tag != b.info.tag)
    {
        throw std::invalid_argument("type mismatch");
    }
    return ParameterHelpers::Lerp(a, b, t);
}

namespace std
{
    inline string to_string(const Parameter& param)
    {
        return ParameterHelpers::Print(param);
    }

    ostream& operator<<(ostream& str, const Parameter& param)
    {
        return str << to_string(param);
    }
}

int main()
{
    std::vector<Parameter> A;
    std::vector<Parameter> B;

    A.emplace_back(42);
    A.emplace_back(3.1415f);
    A.emplace_back(10.0);
    A.emplace_back(5);

    B.emplace_back(5);
    B.emplace_back(125.0f);
    B.emplace_back(512.0);
    B.emplace_back(9);

    for (int i = 0; i < A.size(); i++)
    {
        std::cout << "A = " << A[i] << ", " << "B = " << B[i] << ", "
            << "lerp(A,B,0.25) = " << Parameter::Lerp(A[i], B[i], 0.25f) << std::endl;
    }
}
