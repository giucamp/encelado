#pragma once

#include "ediacaran/core/char_writer.h"
#include "ediacaran/reflection/qualified_type_ptr.h"
#include "ediacaran/reflection/type.h"

namespace ediacaran
{
    class property : public symbol_t
    {
      public:

        using getter = bool (*)(
            const void * i_source_object, void * i_dest_value, char_writer & o_error);
        using setter = bool (*)(
            void * i_dest_object, const void * i_source_value, char_writer & o_error);

        constexpr property(const char * i_name, const qualified_type_ptr & i_qualified_type,
                getter i_getter, setter i_setter) noexcept
            : symbol_t(i_name), m_qualified_type(i_qualified_type), m_getter(i_getter), m_setter(i_setter)
        {
            
        }

        bool set(void * i_dest_object, const void * i_source_value, char_writer & o_error) const
        {
            return (*m_setter)(i_dest_object, i_source_value, o_error);
        }

        bool get(const void * i_source_object, void * o_dest_value, char_writer & o_error) const
        {
            return (*m_getter)(i_source_object, o_dest_value, o_error);
        }

        constexpr qualified_type_ptr const & qualified_type() const noexcept { return m_qualified_type; }

      private:
        qualified_type_ptr const m_qualified_type;
        getter const m_getter;
        setter const m_setter;
    };

    namespace detail
    {
        template <typename>
            struct DataMemberTraits
        {
        };

        template <typename CLASS, typename PROP_TYPE>
            struct DataMemberTraits<PROP_TYPE (CLASS::*)>
        {
            using type = PROP_TYPE;
            using owner_class = CLASS;
        };

        template <typename CLASS, typename PROP_TYPE, PROP_TYPE (CLASS::*MEMBER)>
            constexpr bool impl_data_get(const void * i_source_object, void * i_dest_value, char_writer & o_error)
        {
            auto const & source = static_cast<const CLASS*>(i_source_object)->*MEMBER;
            new(i_dest_value) PROP_TYPE(source);
            return true;
        }

        template <typename CLASS, typename PROP_TYPE, PROP_TYPE (CLASS::*MEMBER)>
            constexpr bool impl_data_set(void * i_dest_object, const void * i_source_value, char_writer & o_error)
        {
            auto & dest = static_cast<CLASS*>(i_dest_object)->*MEMBER;
            auto const & source = *static_cast<const PROP_TYPE*>(i_source_value);
            dest = source;
            return true;
        }
    }

    template <typename POINTER_TO_MEMBER, POINTER_TO_MEMBER MEMB, typename = typename detail::DataMemberTraits<POINTER_TO_MEMBER>::type>
        constexpr property make_property(const char * i_name)
    {
        using traits = detail::DataMemberTraits<POINTER_TO_MEMBER>;
        return property(i_name, get_qualified_type<traits::type>(),
            &detail::impl_data_get<traits::owner_class, traits::type, MEMB>,
            &detail::impl_data_set<traits::owner_class, traits::type, MEMB>);
    }

    /*template <typename CLASS, typename PROP_TYPE>
        constexpr property make_property(const char * i_name, 
            PROP_TYPE (CLASS::*i_getter)() const,
            void (CLASS::*i_setter)(const PROP_TYPE & i_value) = nullptr )
    {
        return property(i_name, get_qualified_type<PROP_TYPE>(), i_getter, i_setter);
    }*/

} // namespace ediacaran
