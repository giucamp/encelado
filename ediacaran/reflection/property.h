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

    class property : public symbol
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
            : symbol(i_name), m_flags(property_flags::none), m_qualified_type(i_qualified_type),
              m_accessor(i_accessor)
        {
        }

        constexpr property(
          offset_tag,
          const char *               i_name,
          const qualified_type_ptr & i_qualified_type,
          size_t                     i_offset) noexcept
            : symbol(i_name), m_flags(property_flags::inplace), m_qualified_type(i_qualified_type),
              m_offset(i_offset)
        {
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
        property_flags const     m_flags;
        qualified_type_ptr const m_qualified_type;
        union {
            size_t   m_offset;
            accessor m_accessor;
        };
    };

    namespace detail
    {
        template <
          typename GETTER_TYPE,
          typename SETTER_TYPE,
          GETTER_TYPE GETTER,
          SETTER_TYPE SETTER>
        struct PropertyAccessor;

        template <
          typename CLASS,
          typename GETTER_RETURN_TYPE,
          typename SETTER_PARAM_TYPE,
          GETTER_RETURN_TYPE (CLASS::*GETTER)() const,
          void (CLASS::*SETTER)(SETTER_PARAM_TYPE i_value)>
        struct PropertyAccessor<
          GETTER_RETURN_TYPE (CLASS::*)() const,
          void (CLASS::*)(SETTER_PARAM_TYPE i_value),
          GETTER,
          SETTER>
        {
            using owner_class   = CLASS;
            using property_type = std::decay_t<GETTER_RETURN_TYPE>;
            static_assert(
              std::is_same_v<property_type, std::decay_t<SETTER_PARAM_TYPE>>,
              "inconsistent types between getter and setter");

            static bool func(
              property::operation i_operation,
              void *              i_object,
              void *              i_value,
              char_writer &       /*o_error*/)
            {
                EDIACARAN_ASSERT(i_object != nullptr);
                EDIACARAN_ASSERT(i_value != nullptr);
                auto const object = static_cast<CLASS *>(i_object);
                switch (i_operation)
                {
                case property::operation::get:
                    new (i_value) property_type((object->*GETTER)());
                    return true;

                case property::operation::set:
                    (object->*SETTER)(*static_cast<property_type *>(i_value));
                    return true;

                default:
                    EDIACARAN_ASSERT(false);
                    return false;
                }
            }
        };

        template <
          typename CLASS,
          typename GETTER_RETURN_TYPE,
          GETTER_RETURN_TYPE (CLASS::*GETTER)() const>
        struct PropertyAccessor<GETTER_RETURN_TYPE (CLASS::*)() const, std::nullptr_t, GETTER, nullptr>
        {
            using owner_class   = CLASS;
            using property_type = std::decay_t<GETTER_RETURN_TYPE>;

            static bool func(
              property::operation i_operation,
              void *              i_object,
              void *              i_value,
              char_writer & /*o_error*/)
            {
                EDIACARAN_ASSERT(i_object != nullptr);
                EDIACARAN_ASSERT(i_value != nullptr);
                auto const object = static_cast<CLASS *>(i_object);
                switch (i_operation)
                {
                case property::operation::get:
                    new (i_value) property_type((object->*GETTER)());
                    return true;

                case property::operation::set:
                default:
                    EDIACARAN_ASSERT(false);
                    return false;
                }
            }
        };

        constexpr property make_data_property(
          const char * i_name, const qualified_type_ptr & i_qualified_type, size_t i_offset)
        {
            return property(property::offset_tag{}, i_name, i_qualified_type, i_offset);
        }

        template <typename PROPERTY_ACCESSOR>
        constexpr property make_accessor_property(const char * i_name)
        {
            return property(
              property::accessor_tag{},
              i_name,
              get_qualified_type<typename PROPERTY_ACCESSOR::property_type>(),
              &PROPERTY_ACCESSOR::func);
        }

    } //namespace detail

} // namespace ediacaran
