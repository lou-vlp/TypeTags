#include <iostream>
#include <vector>
#include <unordered_map>
#include <bit>
#include <any>
#include <cmath>
#include <memory>
#include <cstring>
#include <cassert>
#include <string>

#include "TypeTags.h"

// misc hacky helper/viz stuff
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
