#pragma once

#include "ediacaran/core/char_writer.h"
#include "ediacaran/reflection/qualified_type_ptr.h"
#include "ediacaran/reflection/type.h"
#include <cstddef>

namespace ediacaran
{
    enum class property_flags
    {
        none    = 0,
        inplace = 1 << 0,
    };

    constexpr property_flags operator|(property_flags i_first, property_flags i_second)
    {
        return static_cast<property_flags>(
          static_cast<std::underlying_type_t<property_flags>>(i_first) |
          static_cast<std::underlying_type_t<property_flags>>(i_second));
    }

    constexpr property_flags operator&(property_flags i_first, property_flags i_second)
    {
        return static_cast<property_flags>(
          static_cast<std::underlying_type_t<property_flags>>(i_first) &
          static_cast<std::underlying_type_t<property_flags>>(i_second));
    }

    class property
    {
      public:
        enum class operation
        {
            get,
            set,
        };

        enum class accessor_tag
        {
        };

        enum class offset_tag
        {
        };

        using accessor =
          bool (*)(operation i_operation, void * i_object, void * i_value, char_writer & o_error);

        constexpr property(
          accessor_tag,
          const char *               i_name,
          const qualified_type_ptr & i_qualified_type,
          accessor                   i_accessor)
            : m_name(i_name), m_flags(property_flags::none), m_qualified_type(i_qualified_type),
              m_accessor(i_accessor)
        {
        }

        constexpr property(
          offset_tag,
          const char *               i_name,
          const qualified_type_ptr & i_qualified_type,
          size_t                     i_offset) noexcept
            : m_name(i_name), m_flags(property_flags::inplace), m_qualified_type(i_qualified_type),
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

        constexpr bool is_gettable() const noexcept { return true; }

        constexpr bool is_settable() const noexcept { return !m_qualified_type.is_const(0); }

        constexpr bool is_inplace() const noexcept
        {
            return (m_flags & property_flags::inplace) == property_flags::inplace;
        }

        const void * get_inplace(const void * i_source_object) const noexcept
        {
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
            if (!is_inplace() || !is_settable())
            {
                return nullptr;
            }
            else
            {
                return address_add(i_source_object, m_offset);
            }
        }

        bool get(const void * i_source_object, void * o_dest_value, char_writer & o_error) const
        {
            if (!is_inplace())
            {
                return (*m_accessor)(
                  operation::get, const_cast<void *>(i_source_object), o_dest_value, o_error);
            }
            else
            {
                auto const data = address_add(i_source_object, m_offset);
                m_qualified_type.primary_type()->copy_construct(o_dest_value, data);
                return true;
            }
        }

        bool set(void * i_dest_object, const void * i_source_value, char_writer & o_error) const
        {
            if (!is_settable())
            {
                return false;
            }
            if (!is_inplace())
            {
                return (*m_accessor)(
                  operation::set, i_dest_object, const_cast<void *>(i_source_value), o_error);
            }
            else
            {
                auto const data = address_add(i_dest_object, m_offset);
                m_qualified_type.primary_type()->copy_assign(data, i_source_value);
                return true;
            }
        }

      private:
        const char * const       m_name;
        property_flags const     m_flags;
        qualified_type_ptr const m_qualified_type;
        union {
            size_t   m_offset;
            accessor m_accessor;
        };
    };

} // namespace ediacaran
