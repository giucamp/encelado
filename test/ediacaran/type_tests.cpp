

#include "../common.h"
#include "ediacaran/reflection/qualified_type_ptr.h"
#include "ediacaran/reflection/type.h"

namespace ediacaran_test
{
    void type_tests()
    {
        using namespace ediacaran;
        static_assert(string_view(get_naked_type<bool>().name()) == "bool");

        static_assert(string_view(get_naked_type<int8_t>().name()) == "int8");
        static_assert(string_view(get_naked_type<uint8_t>().name()) == "uint8");
        static_assert(string_view(get_naked_type<int16_t>().name()) == "int16");
        static_assert(string_view(get_naked_type<uint16_t>().name()) == "uint16");
        static_assert(string_view(get_naked_type<int32_t>().name()) == "int32");
        static_assert(string_view(get_naked_type<uint32_t>().name()) == "uint32");
        static_assert(string_view(get_naked_type<int64_t>().name()) == "int64");
        static_assert(string_view(get_naked_type<uint64_t>().name()) == "uint64");

        static_assert(string_view(get_naked_type<float>().name()) == "float");
        static_assert(string_view(get_naked_type<double>().name()) == "double");
        static_assert(string_view(get_naked_type<long double>().name()) == "long double");
    }
}