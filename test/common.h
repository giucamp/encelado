
#include <ediacaran/core/ediacaran_common.h>

#define ENCELADO_TEST_ASSERT(expr)                                                                 \
    if (!(expr))                                                                                   \
        assert_failed(__FILE__, __func__, __LINE__, #expr);                                        \
    else                                                                                           \
        (void)0

void assert_failed(
  const char * i_source_file, const char * i_function, int i_line, const char * i_expr);

namespace ediacaran
{
}

namespace ediacaran_test
{
    void remove_noexcept_tests();
    void type_list_tests();
    void qualified_type_ptr_tests();
    void string_tests();
    void type_tests();
    void class_tests();
    void class_templates_tests();
}
