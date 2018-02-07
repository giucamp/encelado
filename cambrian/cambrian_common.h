
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "ediacaran/core/ediacaran_common.h"
#include <limits>

#define CAMBRIAN_ASSERT EDIACARAN_ASSERT

namespace cambrian
{
    using namespace edi;

    using bit_index = uint_fast16_t;

    template <typename UINT>
    constexpr UINT uint_mask(bit_index i_first_bit, bit_index i_bit_count = 1) noexcept
    {
        static_assert(
          std::numeric_limits<UINT>::is_integer && !std::numeric_limits<UINT>::is_signed &&
          std::numeric_limits<UINT>::radix == 2);
        CAMBRIAN_ASSERT(
          i_first_bit + i_bit_count >= i_first_bit &&                    // check for overflow
          i_first_bit + i_bit_count <= std::numeric_limits<UINT>::digits // check bit range
        );

        if (i_bit_count == std::numeric_limits<UINT>::digits)
        {
            // (1 << i_bit_count) would overflow (and i_first_bit must be zero)
            return std::numeric_limits<UINT>::max();
        }
        else
        {
            auto const base_mask = (static_cast<UINT>(1) << i_bit_count) - 1;
            auto const mask      = base_mask << i_first_bit;
            return mask;
        }
    }

    template <typename UINT>
    constexpr UINT uint_mask_rev(bit_index i_first_bit, bit_index i_bit_count = 1) noexcept
    {
        CAMBRIAN_ASSERT(
          i_first_bit + i_bit_count >= i_first_bit &&                    // check for overflow
          i_first_bit + i_bit_count <= std::numeric_limits<UINT>::digits // check bit range
        );

        return uint_mask<UINT>(
          static_cast<bit_index>(std::numeric_limits<UINT>::digits - i_first_bit - i_bit_count),
          i_bit_count);
    }

    using hash_t = uint32_t;

    constexpr hash_t hash(const string_view & i_source) noexcept
    {
        // http://www.cse.yorku.ca/~oz/hash.html - djb2
        uint32_t hash = 5381;
        for (char c : i_source)
        {
            hash = hash * 33 + c;
        }
        return hash;
    }
}
