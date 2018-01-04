#pragma once

#include "ediacaran/core/array_view.h"
#include "ediacaran/core/ediacaran_common.h"
#include "ediacaran/core/type_list.h"
#include "ediacaran/reflection/action.h"
#include "ediacaran/reflection/property.h"
#include "ediacaran/reflection/type.h"
#include <cstddef>

namespace ediacaran
{
    class class_type;

    struct base_class
    {
      public:
        template <typename DERIVED, typename BASE> constexpr static base_class make() noexcept;

        void * up_cast(void * i_derived) const noexcept { return (*m_up_caster)(i_derived); }

        const void * up_cast(const void * i_derived) const noexcept
        {
            return (*m_up_caster)(const_cast<void *>(i_derived));
        }

        constexpr class_type const & get_class() const noexcept { return m_class; }

      private:
        constexpr base_class(class_type const & i_class, void * (*i_up_caster)(void *)EDIACARAN_NOEXCEPT_FUNCTION_TYPE)
            : m_class(i_class), m_up_caster(i_up_caster)
        {
        }

        template <typename DERIVED, typename BASE> static void * impl_up_cast(void * i_derived) noexcept
        {
            auto const derived = static_cast<DERIVED *>(i_derived);
            EDIACARAN_ASSERT(derived != nullptr);
            return static_cast<BASE *>(derived);
        }

      private:
        class_type const & m_class;
        void * (*const m_up_caster)(void *)EDIACARAN_NOEXCEPT_FUNCTION_TYPE;
    };

    class class_type : public type_t
    {
      public:
        constexpr class_type(const char * const i_name, size_t i_size, size_t i_alignment,
          const special_functions & i_special_functions, const array_view<const base_class> & i_base_classes,
          const array_view<const property> & i_properties, const array_view<const action> & i_actions)
            : type_t(type_kind::is_class, i_name, i_size, i_alignment, i_special_functions),
              m_base_classes(i_base_classes), m_properties(i_properties), m_actions(i_actions)
        {
            check_duplicates();
        }

        constexpr array_view<const base_class> const & base_classes() const noexcept { return m_base_classes; }

        constexpr array_view<const property> const & properties() const noexcept { return m_properties; }

        constexpr array_view<const action> const & actions() const noexcept { return m_actions; }

      private:
        constexpr void check_duplicates() const
        {
            for (size_t i = 0; i < m_properties.size(); i++)
            {
                for (size_t j = i + 1; j < m_properties.size(); j++)
                {
                    if (m_properties[i].name() == m_properties[j].name())
                    {
                        except<std::runtime_error>("duplicate property ", m_properties[i].name(), " in class ", name());
                    }
                }

                for (size_t base_class_index = 0; base_class_index < m_base_classes.size(); base_class_index++)
                {
                    auto & base_props = m_base_classes[base_class_index].get_class().m_properties;
                    for (size_t j = 0; j < base_props.size(); j++)
                    {
                        if (m_properties[i].name() == base_props[j].name())
                        {
                            except<std::runtime_error>("shadowing property ", m_properties[i].name(), " in class ",
                                name(), ", already in ", m_base_classes[base_class_index].get_class().name());
                        }
                    }
                }
            }
        }

      private:
        array_view<const base_class> const m_base_classes;
        array_view<const property> const m_properties;
        array_view<const action> const m_actions;
    };

    namespace detail
    {
        template <typename TYPE> using class_descriptor = decltype(get_type_descriptor(std::declval<TYPE *&>()));

        // base_array - array of base_class, constructed from an input type_list
        template <typename...> struct base_array;
        template <typename CLASS, typename... BASES> struct base_array<CLASS, type_list<BASES...>>
        {
            constexpr static base_class s_bases[sizeof...(BASES)] = {base_class::make<CLASS, BASES>()...};
        };
        template <typename CLASS> struct base_array<CLASS, type_list<>>
        {
            constexpr static array_view<const base_class> s_bases{};
        };

        // makes a list of all the direct and indirect bases of CLASS using class_descriptor<...>::bases as input
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

        template <typename CLASS> struct TemplateParameters
        {
            constexpr static size_t size = 0;
        };

        template <typename FIRST_TYPE, typename... OTHER_TYPES, template <class...> class CLASS>
        struct TemplateParameters<CLASS<FIRST_TYPE, OTHER_TYPES...>>
        {
            constexpr static size_t size = 1 + sizeof...(OTHER_TYPES);
        };

        template <typename FIRST_TYPE, template <class> class CLASS> struct TemplateParameters<CLASS<FIRST_TYPE>>
        {
            constexpr static size_t size = 1;
        };


        struct Edic_Reflect_Defaults
        {
            constexpr static array_view<const property> properties{};
            constexpr static array_view<const action> actions{};
        };

        template <typename CLASS>
            struct TypeInstance<CLASS, std::enable_if_t<std::is_class_v<CLASS>, CLASS>>
        {
            constexpr static class_type instance{ class_descriptor<CLASS>::name, sizeof(CLASS), alignof(CLASS),
                special_functions::make<CLASS>(),
                base_array<CLASS, tl_remove_duplicates_t<typename all_bases<CLASS>::type>>::s_bases,
                class_descriptor<CLASS>::properties, class_descriptor<CLASS>::actions };
        };

        /*template <typename CLASS>
        */
    }

    // get_type
    /*template <typename TYPE, typename = std::enable_if_t<std::is_class_v<TYPE>>>
    constexpr class_type const & get_type() noexcept
    {
        return detail::s_class<TYPE>;
    }*/

    template <typename DERIVED, typename BASE> constexpr base_class base_class::make() noexcept
    {
        return base_class(get_type<BASE>(), &impl_up_cast<DERIVED, BASE>);
    }

} // namespace ediacaran
