#pragma once

#include "ediacaran/core/array_view.h"
#include "ediacaran/core/type_list.h"
#include "ediacaran/reflection/type.h"
#include "ediacaran/reflection/property.h"

namespace ediacaran
{
    class class_type;

    struct base_class
    {
        const class_type * m_class = nullptr;
        size_t m_offset = 0; /**< offset of the subobject from the most derived type */
    };

    class class_type : public type_t
    {
      public:

        constexpr class_type(const char * const i_name, size_t i_size, size_t i_alignment,
          const ediacaran::special_functions & i_special_functions,
          const array_view<const base_class> & i_base_classes,
          const array_view<const property> & i_properties) noexcept
            : type_t(i_name, i_size, i_alignment, i_special_functions),
              m_base_classes(i_base_classes), m_properties(i_properties)
        {
        }

        constexpr array_view<const base_class> const & base_classes() noexcept { return m_base_classes; }

        constexpr array_view<const property> const & properties() noexcept { return m_properties; }

      private:
        array_view<const base_class> const m_base_classes;
        array_view<const property> const m_properties;
    };

    template <typename CLASS> struct class_descriptor;

    // makes a list of all the direct and indirect bases of CLASS
    template <typename...> struct all_bases;
    template <typename CLASS> // this expands <CLASS> to <CLASS, type_list<BASES...>>
    struct all_bases<CLASS>
    {
        using bases = typename class_descriptor<CLASS>::bases;
        using type = typename all_bases<CLASS, bases>::type;
    };
    template <typename CLASS, typename... BASES> struct all_bases<CLASS, type_list<BASES...>>
    {
        using type = tl_push_back_t<type_list<BASES...>, typename all_bases<BASES>::type...>;
    };

    template <typename CLASS>
    class_type make_static_class(
      const char * i_name, std::enable_if_t<all_bases<CLASS>::type::size == 0> * = nullptr) noexcept
    {
        return class_type(i_name, sizeof(CLASS), alignof(CLASS), special_functions::make<CLASS>(),
            array_view<const base_class>(),
            array_view<const property>());
    }

    template <typename CLASS>
    class_type make_static_class(
      const char * i_name, std::enable_if_t<all_bases<CLASS>::type::size != 0> * = nullptr) noexcept
    {
        return class_type(i_name, sizeof(CLASS), alignof(CLASS), special_functions::make<CLASS>(),
          base_array<CLASS, typename all_bases<CLASS>::type>::s_bases,
          array_view<const property>());
    }


    namespace detail
    {
        template <typename CLASS>
            constexpr class_type create_class()
        {
            return make_static_class<CLASS>(class_descriptor<CLASS>::name);
        }

        template <typename CLASS> const class_type s_class{create_class<CLASS>()};
    }

    // get_naked_type
    template <typename TYPE, typename = std::enable_if_t<std::is_class_v<TYPE>>>
    constexpr const class_type & get_naked_type() noexcept
    {
        return detail::s_class<TYPE>;
    }

    template <typename...> struct base_array;

    template <typename CLASS, typename... BASES> struct base_array<CLASS, type_list<BASES...>>
    {
        inline static const base_class s_bases[sizeof...(BASES)] = {{&get_naked_type<BASES>(), 0}...};
    };

    /*template <typename CLASS> - Not working with Visual Stdio: it seems that base_array is specialized anyway
        class_type make_static_class(const char * i_name)
    {
        if constexpr(all_bases<CLASS>::type::size == 0)
        {
            return class_type(i_name, sizeof(CLASS), alignof(CLASS), special_functions::make<CLASS>(),
                array_view<const base_class>());
        }
        else
        {
            return class_type(i_name, sizeof(CLASS), alignof(CLASS), special_functions::make<CLASS>(),
                base_array<CLASS, typename all_bases<CLASS>::type>::s_bases);
        }
    }*/

} // namespace ediacaran