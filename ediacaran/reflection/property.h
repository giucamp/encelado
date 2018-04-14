
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "ediacaran/core/char_writer.h"
#include "ediacaran/reflection/qualified_type_ptr.h"
#include "ediacaran/reflection/type.h"
#include <cstddef>

namespace edi
{
    class property
    {
      public:
        using getter =
          void (*)(property const & i_property, const void * i_source_object, void * i_value_dest);
        using setter =
          void (*)(property const & i_property, void * i_dest_object, const void * i_value_source);

        constexpr property(
          const char *               i_name,
          const qualified_type_ptr & i_qualified_type,
          const void *               i_user_object,
          getter                     i_getter,
          setter                     i_setter)
            : m_name(i_name), m_user_object(i_user_object),
              m_qualified_type(i_qualified_type), m_accessors{i_getter, i_setter}
        {
            EDIACARAN_ASSERT(i_user_object != nullptr);
            EDIACARAN_ASSERT(i_getter != nullptr);
        }

        constexpr property(
          const char *               i_name,
          const qualified_type_ptr & i_qualified_type,
          size_t                     i_offset) noexcept
            : m_name(i_name), m_user_object(nullptr), m_qualified_type(i_qualified_type),
              m_offset(i_offset)
        {
        }

        constexpr string_view name() const noexcept
        {
            // workaround for gcc considering the comparison of two char* non-constexpr
            return string_view(m_name, string_view::traits_type::length(m_name));
        }

        constexpr qualified_type_ptr const & qualified_type() const noexcept
        {
            return m_qualified_type;
        }

        constexpr bool is_settable() const noexcept { return !m_qualified_type.is_const(0); }

        constexpr bool is_inplace() const noexcept { return m_user_object == nullptr; }

        const void * get_inplace(void const * i_source_object) const noexcept
        {
            EDIACARAN_ASSERT(i_source_object != nullptr);
            if (!is_inplace())
            {
                return nullptr;
            }
            else
            {
                return address_add(i_source_object, m_offset);
            }
        }

        void * edit_inplace(void * i_source_object) const noexcept
        {
            EDIACARAN_ASSERT(i_source_object != nullptr);
            EDIACARAN_ASSERT(is_settable());
            if (!is_inplace())
            {
                return nullptr;
            }
            else
            {
                return address_add(i_source_object, m_offset);
            }
        }

        void get(void const * i_source_object, void * o_value_dest) const
        {
            EDIACARAN_ASSERT(i_source_object != nullptr);
            if (!is_inplace())
            {
                (*m_accessors.m_getter)(*this, i_source_object, o_value_dest);
            }
            else
            {
                auto const data = address_add(i_source_object, m_offset);
                m_qualified_type.primary_type()->copy_construct(o_value_dest, data);
            }
        }

        void set(void * i_dest_object, const void * i_value_source) const
        {
            EDIACARAN_ASSERT(i_dest_object != nullptr);
            EDIACARAN_ASSERT(is_settable());
            if (!is_inplace())
            {
                (*m_accessors.m_setter)(*this, i_dest_object, i_value_source);
            }
            else
            {
                auto const data = address_add(i_dest_object, m_offset);
                m_qualified_type.primary_type()->copy_assign(data, i_value_source);
            }
        }

      private:
        const char * const       m_name;
        const void * const       m_user_object;
        qualified_type_ptr const m_qualified_type;
        struct accessors
        {
            getter m_getter;
            setter m_setter;
        };
        union {
            size_t    m_offset;
            accessors m_accessors;
        };
    };

} // namespace edi
