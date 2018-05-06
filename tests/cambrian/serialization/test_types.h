
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../../common.h"
#include "ediacaran/std_refl/vector.h"
#include "ediacaran/utils/dyn_value.h"
#include "vector"

namespace cambrian_test
{
    namespace serialization
    {
        using namespace edi;

        struct TestClass
        {
            int32_t                m_int = 66;
            std::vector<TestClass> m_objects_1;
            double                 m_double = 66;
            std::vector<TestClass> m_objects_2;
        };

        extern const array<property, 4> mak_props;

        constexpr auto reflect(TestClass ** i_ptr)
        {
            using namespace edi;
            char const class_name[] = "";
            using this_class        = std::remove_reference_t<decltype(**i_ptr)>;

            return class_type(
              "cambrian_test::TestClass",
              sizeof(TestClass),
              alignof(TestClass),
              special_functions::make<TestClass>(),
              array<const base_class, 0>{},
              mak_props,
              array<const function, 0>{},
              nullptr);
        }

        dyn_value make_serialization_test_data();

        const array<property, 4> mak_props = make_array(
          make_property<decltype(TestClass::m_int), offsetof(TestClass, m_int)>("int"),
          make_property<decltype(TestClass::m_objects_1), offsetof(TestClass, m_objects_1)>(
            "objects_1"),
          make_property<decltype(TestClass::m_double), offsetof(TestClass, m_double)>("double"),
          make_property<decltype(TestClass::m_objects_2), offsetof(TestClass, m_objects_2)>(
            "objects_2"));

    } // namespace serialization

} // namespace cambrian_test
