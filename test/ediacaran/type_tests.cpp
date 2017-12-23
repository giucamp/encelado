

#include "../common.h"
#include "ediacaran/core/string_builder.h"
#include "ediacaran/reflection/qualified_type_ptr.h"
#include "ediacaran/reflection/type.h"
#include "ediacaran/utils/raw_ptr.h"

namespace ediacaran_test
{
    void type_tests()
    {
        using namespace ediacaran;

        static_assert(get_type<bool>().name() == "bool");

        static_assert(get_type<int8_t>().name() == "int8");
        static_assert(get_type<uint8_t>().name() == "uint8");
        static_assert(get_type<int16_t>().name() == "int16");
        static_assert(get_type<uint16_t>().name() == "uint16");
        static_assert(get_type<int32_t>().name() == "int32");
        static_assert(get_type<uint32_t>().name() == "uint32");
        static_assert(get_type<int64_t>().name() == "int64");
        static_assert(get_type<uint64_t>().name() == "uint64");

        static_assert(get_type<float>().name() == "float");
        static_assert(get_type<double>().name() == "double");
        static_assert(get_type<long double>().name() == "long_double");

        const float f = 1234.5f;
        auto pf = &f;
        auto const * ppf = &pf;
        auto pppf = &ppf;
        raw_ptr ptr(&pppf);
        auto type_name = to_string(ptr.type());
        ENCELADO_TEST_ASSERT(type_name == "float const * const * *");

        auto indirect = ptr.full_indirection();
        ENCELADO_TEST_ASSERT(indirect.object() == &f);

        auto final_type_name = to_string(indirect.type());
        ENCELADO_TEST_ASSERT(final_type_name == "float const");
    }
}
