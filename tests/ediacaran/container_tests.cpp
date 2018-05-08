
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../common.h"
#include "ediacaran/core/array.h"
#include "ediacaran/core/array_view.h"
#include "ediacaran/core/ediacaran_common.h"
#include "ediacaran/core/string_builder.h"
#include "ediacaran/std_refl/list.h"
#include "ediacaran/std_refl/string.h"
#include "ediacaran/std_refl/vector.h"
#include "ediacaran/utils/universal_iterator.h"
#include <array>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace ediacaran_test
{
    struct NotAContigousContainer
    {
        using value_type = int;

        double * data() const { return nullptr; }
        size_t   size() const { return 0; }
    };

    static_assert(!edi::is_container_v<int>);
    static_assert(edi::is_container_v<std::vector<int>>);
    static_assert(edi::is_container_v<std::list<int>>);
    static_assert(edi::is_container_v<edi::array_view<int>>);
    static_assert(edi::is_container_v<edi::array<int, 2>>);
    static_assert(!edi::is_container_v<std::vector<int> *>);

    static_assert(edi::detail::IsValueTypeComparable<std::vector<int>>::value);
    static_assert(!edi::detail::IsValueTypeComparable<std::vector<NotAContigousContainer>>::value);

    static_assert(edi::is_comparable_v<int>);
    static_assert(!edi::is_comparable_v<std::vector<NotAContigousContainer>>);
    static_assert(edi::is_comparable_v<std::vector<int>>);
    static_assert(!edi::is_comparable_v<std::list<NotAContigousContainer>>);
    static_assert(edi::is_comparable_v<std::list<int>>);

    void vector_tests()
    {
        using namespace edi;

        // check array_view constructiblity

        static_assert(std::is_constructible_v<array_view<int>, std::array<int, 3>>);
        static_assert(std::is_constructible_v<array_view<int>, std::vector<int>>);
        static_assert(std::is_constructible_v<array_view<char>, std::string>);
        static_assert(!std::is_constructible_v<array_view<int>, std::list<int>>);

        static_assert(std::is_constructible_v<array_view<const type>, std::array<const type, 6>>);
        static_assert(std::is_constructible_v<array_view<const type>, std::array<type, 6>>);
        static_assert(!std::is_constructible_v<array_view<type>, std::array<const type, 6>>);
        static_assert(
          !std::is_constructible_v<array_view<const type>, std::array<const class_type, 6>>);
        static_assert(!std::is_constructible_v<array_view<const type>, std::array<class_type, 6>>);

        static_assert(!is_contiguous_container_v<int>);
        static_assert(!is_contiguous_container_v<int **>);
        static_assert(!is_contiguous_container_v<NotAContigousContainer>);

        static_assert(is_contiguous_container_v<array_view<int>>);
        static_assert(is_contiguous_container_v<std::vector<int>>);
        static_assert(is_contiguous_container_v<std::array<int, 10>>);
        static_assert(is_contiguous_container_v<std::string>);
        static_assert(!is_contiguous_container_v<std::list<int>>);
        static_assert(!is_contiguous_container_v<std::map<int, int>>);
        static_assert(!is_contiguous_container_v<std::unordered_map<int, int>>);
        static_assert(!is_contiguous_container_v<std::set<int, int>>);
        static_assert(!is_contiguous_container_v<std::unordered_set<int>>);

        constexpr auto & vector_t = get_class_type<std::vector<int>>();
        static_assert(vector_t.container() != nullptr);
        static_assert(vector_t.container()->capabilities() == container::capability::contiguous);

        constexpr auto & list_t = get_class_type<std::list<int>>();
        static_assert(list_t.container() != nullptr);
        static_assert(list_t.container()->capabilities() == container::capability::none);

        string_builder str_builder;

        std::vector<int> vector = {1, 2, 3, 4, 5, 6};
        for (universal_iterator it(&std::as_const(vector)); !it.is_over(); ++it)
        {
            str_builder << *it << ", ";
        }
        auto str = str_builder.to_string();
        ENCELADO_TEST_ASSERT(str == "1, 2, 3, 4, 5, 6, ");

        str_builder         = {};
        std::list<int> list = {1, 2, 3, 4, 5, 6};
        for (universal_iterator it(&std::as_const(list)); !it.is_over(); ++it)
        {
            str_builder << *it << ", ";
        }
        str = str_builder.to_string();
        ENCELADO_TEST_ASSERT(str == "1, 2, 3, 4, 5, 6, ");
    }

    void container_tests() { vector_tests(); }
} // namespace ediacaran_test
