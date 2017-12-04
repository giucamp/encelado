

#define ENCELADO_TEST_ASSERT(expr)        if(!(expr)) assert_failed(__FILE__, __func__, __LINE__, #expr); else (void)0

void assert_failed(const char * i_source_file, const char * i_function, int i_line, const char * i_expr);

namespace ediacaran
{
}

namespace ediacaran_test
{
    using namespace ediacaran;
    void qualified_type_ptr_tests();
    void core_tests();
}
