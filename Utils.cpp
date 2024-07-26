#include "Utils.h"

Parameter ParameterHelpers::Mix(const Parameter& a, const Parameter& b, float t)
{
    assert(a.TypeTag() == b.TypeTag());
    return Mix(a, b, t, a.TypeTag());
}

std::string ParameterHelpers::Print(const Parameter& param)
{
    return Print<0>(param);
}

