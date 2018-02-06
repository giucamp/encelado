
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../common.h"
#include "ediacaran/core/string_builder.h"
#include "ediacaran/std_refl/string.h"
#include "ediacaran/std_refl/vector.h"
#include "ediacaran/utils/universal_iterator.h"
#include <iostream>

namespace ediacaran_test
{
    void vector_tests()
    {
        using namespace edi;

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
}
