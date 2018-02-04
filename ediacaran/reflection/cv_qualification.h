
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.

#pragma once
#include "ediacaran/core/ediacaran_common.h"
#include "ediacaran/reflection/type.h"
#include <ediacaran/core/array.h>
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace ediacaran
{
    /** Scoped enum that stores a combination of cv-qualifiers. cv_qualification can be combined and subtracted with the overloaded bitwise operators | and &. */
    enum class cv_qualification
    {
        no_q       = 0,      /**< No flags */
        const_q    = 1 << 0, /**< Set for const types */
        volatile_q = 1 << 1, /**< Set for volatile types */
    };

    constexpr cv_qualification
      operator|(cv_qualification i_first, cv_qualification i_seconds) noexcept
    {
        using underlying_type = std::underlying_type<cv_qualification>::type;
        return static_cast<cv_qualification>(
          static_cast<underlying_type>(i_first) | static_cast<underlying_type>(i_seconds));
    }

    constexpr cv_qualification
      operator&(cv_qualification i_first, cv_qualification i_seconds) noexcept
    {
        using underlying_type = std::underlying_type<cv_qualification>::type;
        return static_cast<cv_qualification>(
          static_cast<underlying_type>(i_first) & static_cast<underlying_type>(i_seconds));
    }

    constexpr bool is_const(cv_qualification i_qualification) noexcept
    {
        return (i_qualification & cv_qualification::const_q) != cv_qualification::no_q;
    }

    constexpr bool is_volatile(cv_qualification i_qualification) noexcept
    {
        return (i_qualification & cv_qualification::volatile_q) != cv_qualification::no_q;
    }

} // namespace ediacaran
