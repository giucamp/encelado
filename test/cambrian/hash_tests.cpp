
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../common.h"
#include "cambrian/data/path.h"
#include "ediacaran/core/string_utils.h"
#include "iliade.h"
#include <iostream>

namespace cambrian_test
{

    void hash_tests()
    {
        volatile auto f = &hash_tests;

        using namespace cambrian;
        hash(text);

        constexpr string_view tx(text);
        for (auto token : split(tx, ' '))
        {
            std::cout << "* " << std::string(token) << "\n";
        }

        std::cout <<std::endl;
    }
}
