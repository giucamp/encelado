#pragma once

#include "ediacaran/core/array_view.h"
#include "ediacaran/core/ediacaran_common.h"
#include "ediacaran/reflection/type.h"

namespace ediacaran
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
            : type(type_kind::is_enum, i_name, i_size, i_alignment, i_special_functions), m_members(i_members)
        {
        }

        constexpr array_view<const enum_member<UNDERLYING_TYPE>> const & members() const noexcept
        {
            return m_members;
        }

      private:
        array_view<const enum_member<UNDERLYING_TYPE>> const m_members;
    };

    // forward
    template <typename TYPE> constexpr const enum_type<std::underlying_type_t<TYPE>> & get_enum_type() noexcept;

    /*template <typename ENUM, std::enable_if_t<std::is_enum_v<ENUM>> * = nullptr>
        constexpr char_writer & operator << (char_writer & o_dest, ENUM i_value)
    {
        constexpr auto & type = get_enum_type<ENUM>();
        bool found = false;
        for (auto const & member : type.members())
        {
            if (member.value() == i_value)
            {
                o_dest << member.name();
                found = true;
                break;
            }
        }
        if(!found)
            o_dest << i_value;
    }*/

} // namespace ediacaran
