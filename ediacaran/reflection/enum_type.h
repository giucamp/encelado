
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "ediacaran/core/array_view.h"
#include "ediacaran/core/ediacaran_common.h"
#include "ediacaran/reflection/type.h"

namespace edi
{
    template <typename UNDERLYING_TYPE> class enum_member
    {
      public:
        constexpr enum_member(const char * const i_name, UNDERLYING_TYPE i_value) noexcept
            : m_name(i_name), m_value(i_value)
        {
        }

        constexpr string_view name() const noexcept
        {
            // workaround for gcc considering the comparison of two char* non-constexpr
            return string_view(m_name, string_view::traits_type::length(m_name));
        }

        constexpr UNDERLYING_TYPE const value() const noexcept { return m_value; }

      private:
        const char * const    m_name;
        UNDERLYING_TYPE const m_value;
    };

    template <typename UNDERLYING_TYPE> class enum_type : public type
    {
      public:
        constexpr enum_type(
          const char * const                                     i_name,
          size_t                                                 i_size,
          size_t                                                 i_alignment,
          const special_functions &                              i_special_functions,
          const array_view<const enum_member<UNDERLYING_TYPE>> & i_members)
            : type(type_kind::is_enum, i_name, i_size, i_alignment, i_special_functions),
              m_members(i_members)
        {
        }

        constexpr array_view<const enum_member<UNDERLYING_TYPE>> const & members() const noexcept
        {
            return m_members;
        }

      private:
        array_view<const enum_member<UNDERLYING_TYPE>> const m_members;
    };

    template <typename UNDERLYING_TYPE>
    constexpr void enum_to_chars(
      char_writer &                      o_dest,
      const enum_type<UNDERLYING_TYPE> & i_enum,
      UNDERLYING_TYPE                    i_value) noexcept
    {
        bool something_written = false;
        auto remaining_value   = i_value;
        for (auto const & member : i_enum.members())
        {
            auto const memb_val = member.value();
            if ((remaining_value & memb_val) == memb_val)
            {
                if (something_written)
                    o_dest << " | ";
                o_dest << member.name();
                remaining_value &= ~memb_val;
                something_written = true;
            }
        }
        if (remaining_value != 0)
        {
            if (something_written)
                o_dest << " | ";
            o_dest << remaining_value;
        }
    }

    // forward
    template <typename TYPE>
    constexpr const enum_type<std::underlying_type_t<TYPE>> & get_enum_type() noexcept;

    template <typename ENUM, std::enable_if_t<std::is_enum_v<ENUM>> * = nullptr>
    constexpr char_writer & operator<<(char_writer & o_dest, ENUM i_value) noexcept
    {
        enum_to_chars(
          o_dest, get_enum_type<ENUM>(), static_cast<std::underlying_type_t<ENUM>>(i_value));
        return o_dest;
    }

} // namespace edi
