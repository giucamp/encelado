
#include "../common.h"
#include "ediacaran/core/char_reader.h"
#include "ediacaran/core/string_builder.h"
#include "ediacaran/reflection/reflection.h"
#include <vector>

namespace ediacaran_test
{
    template <size_t CV_COUNT>
    void qualified_type_ptr_unit_test_cvs(
      ediacaran::qualified_type_ptr const & i_q_type, ediacaran::CV_Flags const (&i_cvs)[CV_COUNT])
    {
        ENCELADO_TEST_ASSERT(i_q_type.indirection_levels() + 1 == CV_COUNT);
        for (size_t i = 0; i < CV_COUNT; i++)
        {
            ENCELADO_TEST_ASSERT(i_q_type.cv_flags(i) == i_cvs[i]);
        }
    }

    template <typename TYPE> void qualified_type_ptr_unit_test_type()
    {
        using namespace ediacaran;

        ENCELADO_TEST_ASSERT(get_qualified_type<TYPE &>() == get_qualified_type<TYPE * const>());

        // test <TYPE>
        {
            const auto q_type_ptr = get_qualified_type<TYPE>();
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_empty());
            ENCELADO_TEST_ASSERT(q_type_ptr.final_type() == &get_type<TYPE>());
            ENCELADO_TEST_ASSERT(q_type_ptr.primary_type() == &get_type<TYPE>());
            ENCELADO_TEST_ASSERT(q_type_ptr.indirection_levels() == 0);
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_const(0));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_volatile(0));
            ENCELADO_TEST_ASSERT(q_type_ptr == q_type_ptr);
            const CV_Flags cvs[] = {CV_Flags::None};
            qualified_type_ptr_unit_test_cvs(q_type_ptr, cvs);
            ENCELADO_TEST_ASSERT(q_type_ptr != qualified_type_ptr());
        }

        // test <volatile TYPE>
        {
            const auto q_type_ptr = get_qualified_type<volatile TYPE>();
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_empty());
            ENCELADO_TEST_ASSERT(q_type_ptr.final_type() == &get_type<TYPE>());
            ENCELADO_TEST_ASSERT(q_type_ptr.primary_type() == &get_type<TYPE>());
            ENCELADO_TEST_ASSERT(q_type_ptr.indirection_levels() == 0);
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_const(0));
            ENCELADO_TEST_ASSERT(q_type_ptr.is_volatile(0));
            ENCELADO_TEST_ASSERT(q_type_ptr == q_type_ptr);
            const CV_Flags cvs[] = {CV_Flags::Volatile};
            qualified_type_ptr_unit_test_cvs(q_type_ptr, cvs);
            ENCELADO_TEST_ASSERT(q_type_ptr != qualified_type_ptr());
        }

        // test <const TYPE &>
        {
            const auto q_type_ptr = get_qualified_type<const TYPE &>();
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_empty());
            ENCELADO_TEST_ASSERT(q_type_ptr.final_type() == &get_type<TYPE>());
            ENCELADO_TEST_ASSERT(q_type_ptr.primary_type() == &get_type<void *>());
            ENCELADO_TEST_ASSERT(q_type_ptr.indirection_levels() == 1);
            ENCELADO_TEST_ASSERT(q_type_ptr.is_const(0));
            ENCELADO_TEST_ASSERT(q_type_ptr.is_const(1));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_volatile(0));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_volatile(1));
            ENCELADO_TEST_ASSERT(q_type_ptr == q_type_ptr);
            const CV_Flags cvs[] = {CV_Flags::Const, CV_Flags::Const};
            qualified_type_ptr_unit_test_cvs(q_type_ptr, cvs);
            ENCELADO_TEST_ASSERT(q_type_ptr != qualified_type_ptr());
        }

        // test <TYPE *const*volatile**>
        {
            const auto q_type_ptr = get_qualified_type<TYPE * const * volatile **&>();
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_empty());
            ENCELADO_TEST_ASSERT(q_type_ptr.final_type() == &get_type<TYPE>());
            ENCELADO_TEST_ASSERT(q_type_ptr.primary_type() == &get_type<void *>());
            ENCELADO_TEST_ASSERT(q_type_ptr.indirection_levels() == 5);
            ENCELADO_TEST_ASSERT(q_type_ptr.is_const(0));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_const(1));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_const(2));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_const(3));
            ENCELADO_TEST_ASSERT(q_type_ptr.is_const(4));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_const(5));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_volatile(0));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_volatile(1));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_volatile(2));
            ENCELADO_TEST_ASSERT(q_type_ptr.is_volatile(3));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_volatile(4));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_volatile(5));
            ENCELADO_TEST_ASSERT(q_type_ptr == q_type_ptr);
            const CV_Flags cvs[] = {CV_Flags::Const,
                                    CV_Flags::None,
                                    CV_Flags::None,
                                    CV_Flags::Volatile,
                                    CV_Flags::Const,
                                    CV_Flags::None};
            qualified_type_ptr_unit_test_cvs(q_type_ptr, cvs);
            ENCELADO_TEST_ASSERT(q_type_ptr != qualified_type_ptr());
        }
    }


    template <typename TYPE> void qualified_type_ptr_string_tests(const char * i_type_str)
    {
        using namespace ediacaran;

        auto const qual_type = get_qualified_type<TYPE>();
        ENCELADO_TEST_ASSERT(qual_type == parse<qualified_type_ptr>(i_type_str));

        auto const stringfied = to_string(qual_type);
        auto const parsed     = parse<qualified_type_ptr>(stringfied);
        ENCELADO_TEST_ASSERT(parsed == qual_type);
    }

    void qualified_type_ptr_tests()
    {
        using namespace ediacaran;

        // test empty
        {
            qualified_type_ptr q_type_ptr;
            ENCELADO_TEST_ASSERT(q_type_ptr.is_empty());
            ENCELADO_TEST_ASSERT(q_type_ptr.final_type() == nullptr);
            ENCELADO_TEST_ASSERT(q_type_ptr.primary_type() == nullptr);
            ENCELADO_TEST_ASSERT(q_type_ptr.indirection_levels() == 0);
            ENCELADO_TEST_ASSERT(q_type_ptr == q_type_ptr);
        }

        // test types
        qualified_type_ptr_unit_test_type<float>();
        //qualified_type_ptr_unit_test_type<std::vector<int>>();

        ENCELADO_TEST_ASSERT(get_qualified_type<const void *>().is_const(1));
        ENCELADO_TEST_ASSERT(get_qualified_type<void * const>().is_const(0));

        {
            qualified_type_ptr q_type_ptr(
              get_type<float>(),
              {CV_Flags::Const | CV_Flags::Volatile, CV_Flags::None, CV_Flags::Volatile});
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_empty());
            ENCELADO_TEST_ASSERT(q_type_ptr.final_type() == &get_type<float>());
            ENCELADO_TEST_ASSERT(q_type_ptr.primary_type() == &get_type<void *>());
            ENCELADO_TEST_ASSERT(q_type_ptr.is_const(0));
            ENCELADO_TEST_ASSERT(q_type_ptr.is_volatile(0));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_const(1));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_volatile(1));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_const(2));
            ENCELADO_TEST_ASSERT(q_type_ptr.is_volatile(2));
            ENCELADO_TEST_ASSERT(q_type_ptr.indirection_levels() == 2);
            ENCELADO_TEST_ASSERT(q_type_ptr.cv_flags(0) == (CV_Flags::Const | CV_Flags::Volatile));
            ENCELADO_TEST_ASSERT(q_type_ptr.cv_flags(1) == CV_Flags::None);
            ENCELADO_TEST_ASSERT(q_type_ptr.cv_flags(2) == CV_Flags::Volatile);
            ENCELADO_TEST_ASSERT(q_type_ptr == q_type_ptr);
            ENCELADO_TEST_ASSERT(q_type_ptr != qualified_type_ptr());
            ENCELADO_TEST_ASSERT(
              q_type_ptr == get_qualified_type<float volatile ** volatile const>());
        }
        {
            ENCELADO_TEST_ASSERT(
              qualified_type_ptr(get_type<void>(), {}) ==
              qualified_type_ptr(get_type<void>(), {CV_Flags::None}));
        }

        {
            qualified_type_ptr q_type_ptr(
              get_type<void>(),
              {CV_Flags::Const | CV_Flags::Volatile, CV_Flags::None, CV_Flags::Volatile});
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_empty());
            ENCELADO_TEST_ASSERT(q_type_ptr.final_type() == &get_type<void>());
            ENCELADO_TEST_ASSERT(q_type_ptr.primary_type() == &get_type<void *>());
            ENCELADO_TEST_ASSERT(q_type_ptr.is_const(0));
            ENCELADO_TEST_ASSERT(q_type_ptr.is_volatile(0));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_const(1));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_volatile(1));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_const(2));
            ENCELADO_TEST_ASSERT(q_type_ptr.is_volatile(2));
            ENCELADO_TEST_ASSERT(q_type_ptr.indirection_levels() == 2);
            ENCELADO_TEST_ASSERT(q_type_ptr.cv_flags(0) == (CV_Flags::Const | CV_Flags::Volatile));
            ENCELADO_TEST_ASSERT(q_type_ptr.cv_flags(1) == CV_Flags::None);
            ENCELADO_TEST_ASSERT(q_type_ptr.cv_flags(2) == CV_Flags::Volatile);
            ENCELADO_TEST_ASSERT(q_type_ptr == q_type_ptr);
            ENCELADO_TEST_ASSERT(q_type_ptr != qualified_type_ptr());
            ENCELADO_TEST_ASSERT(
              q_type_ptr == get_qualified_type<void volatile ** volatile const>());
        }

        // test <void *const*volatile**>
        {
            const auto q_type_ptr = get_qualified_type<void * const * volatile **&>();
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_empty());
            ENCELADO_TEST_ASSERT(q_type_ptr.final_type() == &get_type<void>());
            ENCELADO_TEST_ASSERT(q_type_ptr.primary_type() == &get_type<void *>());
            ENCELADO_TEST_ASSERT(q_type_ptr.indirection_levels() == 5);
            ENCELADO_TEST_ASSERT(q_type_ptr.is_const(0));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_const(1));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_const(2));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_const(3));
            ENCELADO_TEST_ASSERT(q_type_ptr.is_const(4));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_const(5));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_volatile(0));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_volatile(1));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_volatile(2));
            ENCELADO_TEST_ASSERT(q_type_ptr.is_volatile(3));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_volatile(4));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_volatile(5));
            ENCELADO_TEST_ASSERT(q_type_ptr == q_type_ptr);
            ENCELADO_TEST_ASSERT(q_type_ptr != qualified_type_ptr());
        }

        // test <const void *>
        {
            const auto q_type_ptr = get_qualified_type<const void *>();
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_empty());
            ENCELADO_TEST_ASSERT(q_type_ptr.final_type() == &get_type<void>());
            ENCELADO_TEST_ASSERT(q_type_ptr.primary_type() == &get_type<void *>());
            ENCELADO_TEST_ASSERT(q_type_ptr.indirection_levels() == 1);
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_const(0));
            ENCELADO_TEST_ASSERT(q_type_ptr.is_const(1));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_volatile(0));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_volatile(1));
            ENCELADO_TEST_ASSERT(q_type_ptr == q_type_ptr);
            ENCELADO_TEST_ASSERT(q_type_ptr != qualified_type_ptr());
        }

        // test <void * const>
        {
            const auto q_type_ptr = get_qualified_type<void * const>();
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_empty());
            ENCELADO_TEST_ASSERT(q_type_ptr.final_type() == &get_type<void>());
            ENCELADO_TEST_ASSERT(q_type_ptr.primary_type() == &get_type<void *>());
            ENCELADO_TEST_ASSERT(q_type_ptr.indirection_levels() == 1);
            ENCELADO_TEST_ASSERT(q_type_ptr.is_const(0));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_const(1));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_volatile(0));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_volatile(1));
            ENCELADO_TEST_ASSERT(q_type_ptr == q_type_ptr);
            ENCELADO_TEST_ASSERT(q_type_ptr != qualified_type_ptr());
        }

        // test <void *>
        {
            const auto q_type_ptr = get_qualified_type<void *>();
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_empty());
            ENCELADO_TEST_ASSERT(q_type_ptr.final_type() == &get_type<void>());
            ENCELADO_TEST_ASSERT(q_type_ptr.primary_type() == &get_type<void *>());
            ENCELADO_TEST_ASSERT(q_type_ptr.indirection_levels() == 1);
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_const(0));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_const(1));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_volatile(0));
            ENCELADO_TEST_ASSERT(!q_type_ptr.is_volatile(1));
            ENCELADO_TEST_ASSERT(q_type_ptr == q_type_ptr);
            ENCELADO_TEST_ASSERT(q_type_ptr != qualified_type_ptr());
        }

        // test cv qualifiers for float volatile*const volatile*const*
        {
            const auto q_type_ptr = get_qualified_type<float volatile * const volatile * const *>();
            const CV_Flags cvs[]  = {CV_Flags::None,
                                    CV_Flags::Const,
                                    CV_Flags::Const | CV_Flags::Volatile,
                                    CV_Flags::Volatile};
            qualified_type_ptr_unit_test_cvs(q_type_ptr, cvs);
        }

        // test <const int *> and <const * int> (they are the same type)
        ENCELADO_TEST_ASSERT(
          get_qualified_type<const int *>() == get_qualified_type<int const *>());

        /* qualified_type_ptr is documented (as implementation note) to be big as two pointers. This static_assert is here
			to verify the correctness of this note, but if it would ever fail on some compiler, it may safely be removed
			(together with the implementation note), as no other assumptions on the size of qualified_type_ptr are present
			in the library. */
        static_assert(
          sizeof(qualified_type_ptr) == sizeof(void *) * 2,
          "qualified_type_ptr is not big as two pointers, as documented.");


#define CHECK_TYPE(TYPE) qualified_type_ptr_string_tests<TYPE>(#TYPE)
        CHECK_TYPE(float);
        CHECK_TYPE(const float);
        CHECK_TYPE(volatile float const);
        CHECK_TYPE(const float ***);
        CHECK_TYPE(volatile float * const volatile * const *);
        CHECK_TYPE(float * const volatile ***);
        CHECK_TYPE(float * volatile **);
        CHECK_TYPE(float &);
        CHECK_TYPE(const float &);
        CHECK_TYPE(volatile float const &);
        CHECK_TYPE(const float ***&);
        CHECK_TYPE(volatile float * const volatile * const *&&);
        CHECK_TYPE(float * const volatile ***&);
        CHECK_TYPE(float * volatile **&&);
        CHECK_TYPE(float &&);
        CHECK_TYPE(const float **);
        CHECK_TYPE(volatile float ** const);
        CHECK_TYPE(const float *** const);
        CHECK_TYPE(volatile const float * const volatile * const *);
        CHECK_TYPE(float * const volatile ***);
        CHECK_TYPE(float * volatile **);
        CHECK_TYPE(float **&&);
        CHECK_TYPE(const float ****&);
        CHECK_TYPE(volatile float * const &);
        CHECK_TYPE(const float *&&);
        CHECK_TYPE(volatile float * const volatile * const *&&);
        CHECK_TYPE(float * const volatile ***&);
        CHECK_TYPE(float * volatile **&&);

        CHECK_TYPE(void);
        CHECK_TYPE(const void);
        CHECK_TYPE(volatile void const);
        CHECK_TYPE(const void ***);
        CHECK_TYPE(volatile void * const volatile * const *);
        CHECK_TYPE(void * const volatile ***);
        CHECK_TYPE(void * volatile **);
        CHECK_TYPE(const void ***&);
        CHECK_TYPE(volatile void * const volatile * const *&&);
        CHECK_TYPE(void * const volatile ***&);
        CHECK_TYPE(void * volatile **&&);
        CHECK_TYPE(const void **);
        CHECK_TYPE(volatile void ** const);
        CHECK_TYPE(const void *** const);
        CHECK_TYPE(volatile const void * const volatile * const *);
        CHECK_TYPE(void * const volatile ***);
        CHECK_TYPE(void * volatile **);
        CHECK_TYPE(void **&&);
        CHECK_TYPE(const void ****&);
        CHECK_TYPE(volatile void * const &);
        CHECK_TYPE(const void *&&);
        CHECK_TYPE(volatile void * const volatile * const *&&);
        CHECK_TYPE(void * const volatile ***&);
        CHECK_TYPE(void * volatile **&&);

#undef CHECK_TYPE
    }
}