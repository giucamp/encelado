

#include "../common.h"
#include "ediacaran/std_refl/string.h"
#include "ediacaran/std_refl/vector.h"
#include "ediacaran/utils/iterator.h"
#include <iostream>

namespace ediacaran_test
{
    void vector_tests()
    {
        using namespace ediacaran;

        static_assert(get_class_type<std::vector<int>>().container() != nullptr);

        auto & vector_t = get_class_type<std::vector<int>>();

        std::vector<int> vector = {1, 2, 3, 4, 5, 6};


        iterator it{raw_ptr(&vector)};
    }

    void container_tests() { vector_tests(); }
}
