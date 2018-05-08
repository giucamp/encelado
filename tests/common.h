
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ediacaran/core/ediacaran_common.h>

#define ENCELADO_TEST_ASSERT(expr)                                                                 \
    if (!(expr))                                                                                   \
        assert_failed(__FILE__, __func__, __LINE__, #expr);                                        \
    else                                                                                           \
        (void)0

void assert_failed(
  const char * i_source_file, const char * i_function, int i_line, const char * i_expr);

namespace ediacaran_test
{
    void remove_noexcept_tests();
    void type_list_tests();
    void qualified_type_ptr_tests();
    void string_tests();
    void type_tests();
    void class_tests();
    void class_templates_tests();
    void container_tests();
    void animalia_tests();
} // namespace ediacaran_test

namespace cambrian_test
{
    void common_tests();

    namespace serialization
    {
        void tests();
    }

} // namespace cambrian_test
