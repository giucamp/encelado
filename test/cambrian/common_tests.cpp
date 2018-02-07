
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../common.h"
#include "cambrian/data/directory.h"
#include "ediacaran/core/string_utils.h"
#include "iliade.h"
#include <iostream>

namespace cambrian_test
{

    void common_tests()
    {
        using namespace cambrian;

        auto a = uint_mask<uint8_t>(7, 1);
        static_assert(uint_mask<uint8_t>(0, 1) == 0B0000'0001);
        static_assert(uint_mask<uint8_t>(0, 2) == 0B0000'0011);
        static_assert(uint_mask<uint8_t>(7, 1) == 0B1000'0000);
        static_assert(uint_mask<uint8_t>(6, 2) == 0B1100'0000);
        static_assert(uint_mask<uint8_t>(0, 8) == 0B1111'1111);

        static_assert(uint_mask_rev<uint8_t>(0, 1) == 0B1000'0000);
        static_assert(uint_mask_rev<uint8_t>(0, 2) == 0B1100'0000);
        static_assert(uint_mask_rev<uint8_t>(7, 1) == 0B0000'0001);
        static_assert(uint_mask_rev<uint8_t>(6, 2) == 0B0000'0011);
        static_assert(uint_mask_rev<uint8_t>(0, 8) == 0B1111'1111);

        static_assert(
          uint_mask_rev<uint64_t>(0, 64) ==
          0B11111111'11111111'11111111'11111111'11111111'11111111'11111111'11111111);
        static_assert(
          uint_mask_rev<uint64_t>(1, 63) ==
          0B01111111'11111111'11111111'11111111'11111111'11111111'11111111'11111111);

        hash(text);

        constexpr string_view tx(text);
        for (auto token : split(tx, ' '))
        {
            std::cout << "* " << std::string(token) << "\n";
        }

        std::cout << std::endl;
    }
}
