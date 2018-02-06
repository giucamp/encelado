
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../common.h"
#include "ediacaran/core/remove_noexcept.h"

namespace ediacaran_test
{
    void remove_noexcept_tests()
    {
        using namespace edi;

        struct Struct
        {
        };

#define REMOVE_NOEXCEPT_TESTS(Signature)                                                           \
    static_assert(std::is_same_v<Signature, remove_noexcept_t<Signature noexcept>>);

        REMOVE_NOEXCEPT_TESTS(void());
        REMOVE_NOEXCEPT_TESTS(void (*)());
        REMOVE_NOEXCEPT_TESTS(void(int));
        REMOVE_NOEXCEPT_TESTS(void (*)(int));
        REMOVE_NOEXCEPT_TESTS(void (Struct::*)());
        REMOVE_NOEXCEPT_TESTS(void (Struct::*)() const);
        REMOVE_NOEXCEPT_TESTS(void (Struct::*)() volatile);
        REMOVE_NOEXCEPT_TESTS(void (Struct::*)() const volatile);
        REMOVE_NOEXCEPT_TESTS(void (Struct::*)() &);
        REMOVE_NOEXCEPT_TESTS(void (Struct::*)() const &);
        REMOVE_NOEXCEPT_TESTS(void (Struct::*)() volatile &);
        REMOVE_NOEXCEPT_TESTS(void (Struct::*)() const volatile &);
        REMOVE_NOEXCEPT_TESTS(void (Struct::*)() &&);
        REMOVE_NOEXCEPT_TESTS(void (Struct::*)() const &&);
        REMOVE_NOEXCEPT_TESTS(void (Struct::*)() volatile &&);
        REMOVE_NOEXCEPT_TESTS(void (Struct::*)() const volatile &&);

        REMOVE_NOEXCEPT_TESTS(int());
        REMOVE_NOEXCEPT_TESTS(int (*)());
        REMOVE_NOEXCEPT_TESTS(int(int));
        REMOVE_NOEXCEPT_TESTS(int (*)(int));
        REMOVE_NOEXCEPT_TESTS(int (Struct::*)());
        REMOVE_NOEXCEPT_TESTS(int (Struct::*)() const);
        REMOVE_NOEXCEPT_TESTS(int (Struct::*)() volatile);
        REMOVE_NOEXCEPT_TESTS(int (Struct::*)() const volatile);
        REMOVE_NOEXCEPT_TESTS(int (Struct::*)() &);
        REMOVE_NOEXCEPT_TESTS(int (Struct::*)() const &);
        REMOVE_NOEXCEPT_TESTS(int (Struct::*)() volatile &);
        REMOVE_NOEXCEPT_TESTS(int (Struct::*)() const volatile &);
        REMOVE_NOEXCEPT_TESTS(int (Struct::*)() &&);
        REMOVE_NOEXCEPT_TESTS(int (Struct::*)() const &&);
        REMOVE_NOEXCEPT_TESTS(int (Struct::*)() volatile &&);
        REMOVE_NOEXCEPT_TESTS(int (Struct::*)() const volatile &&);

#undef REMOVE_NOEXCEPT_TESTS
    }
}
