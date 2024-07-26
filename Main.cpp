#include <iostream>
#include <vector>
#include "Utils.h"

// misc hacky helper/viz stuff
namespace std
{
    ostream& operator<<(ostream& str, const Parameter& param)
    {
        return str << ParameterHelpers::Print(param);
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

    std::cout << "sizeof(unique_ptr<byte[]>) = " << sizeof(std::unique_ptr<std::byte[]>) << std::endl;
    std::cout << "sizeof(Parameter) = " << sizeof(Parameter) << std::endl;

    for (int i = 0; i < A.size(); i++)
    {
        std::cout << "A = " << A[i] << ", " << "B = " << B[i] << ", "
            << "lerp(A,B,0.25) = " << Parameter::Mix(A[i], B[i], 0.25f) << std::endl;
    }
}
