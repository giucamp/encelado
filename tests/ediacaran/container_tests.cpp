
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../common.h"
#include "ediacaran/core/string_builder.h"
#include "ediacaran/std_refl/string.h"
#include "ediacaran/std_refl/vector.h"
#include "ediacaran/utils/universal_iterator.h"
#include <array>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <string>
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

    void vector_tests()
    {
        using namespace edi;

        static_assert(!is_contigous_container_v<int>);
        static_assert(!is_contigous_container_v<int **>);
        static_assert(!is_contigous_container_v<NotAContigousContainer>);

        static_assert(is_contigous_container_v<array_view<int>>);
        static_assert(is_contigous_container_v<std::vector<int>>);
        static_assert(is_contigous_container_v<std::array<int, 10>>);
        static_assert(is_contigous_container_v<std::string>);
        static_assert(!is_contigous_container_v<std::list<int>>);
        static_assert(!is_contigous_container_v<std::map<int, int>>);
        static_assert(!is_contigous_container_v<std::unordered_map<int, int>>);
        static_assert(!is_contigous_container_v<std::set<int, int>>);
        static_assert(!is_contigous_container_v<std::unordered_set<int, int>>);

        static_assert(get_class_type<std::vector<int>>().container() != nullptr);

        auto & vector_t = get_class_type<std::vector<int>>();

        std::vector<int> vector = {1, 2, 3, 4, 5, 6};

        string_builder str;
        for (universal_iterator it(&std::as_const(vector)); !it.is_over(); ++it)
        {
            str << *it << '\n';
        }

        std::cout << str.to_string();
        std::cout << std::endl;
    }

    void container_tests() { vector_tests(); }
} // namespace ediacaran_test
