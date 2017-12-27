#pragma once

#include "ediacaran/core/ediacaran_common.h"
#include "ediacaran/core/array_view.h"
#include "ediacaran/core/type_list.h"
#include "ediacaran/reflection/property.h"
#include "ediacaran/reflection/action.h"
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
          const array_view<const property> & i_properties,
          const array_view<const action> & i_actions)
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
            if (m_properties.size() > 0)
                for (auto prop_it = m_properties.begin(); prop_it != m_properties.end(); prop_it++)
                {
                    for (auto other_prop_it = prop_it + 1; other_prop_it != m_properties.end(); other_prop_it++)
                    {
                        if (prop_it->name() == other_prop_it->name())
                        {
                            char message[512]{};
                            ediacaran::to_chars(message, "duplicate property ", prop_it->name(), " in class ", name());
                            throw std::runtime_error(message);
                        }
                    }

                    if (m_base_classes.size() > 0)
                        for (auto & base : m_base_classes)
                        {
                            auto & base_props = base.get_class().m_properties;
                            if (base_props.size() > 0)
                                for (auto & base_prop : base_props)
                                {
                                    if (prop_it->name() == base_prop.name())
                                    {
                                        char message[512]{};
                                        ediacaran::to_chars(message, "shadowing property ", prop_it->name(),
                                          " in class ", name(), ", already in ", base.get_class().name());
                                        throw std::runtime_error(message);
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

    template <typename TYPE> using class_descriptor = decltype(get_type_descriptor(std::declval<TYPE *&>()));

    template <typename...> struct base_array;

    template <typename CLASS, typename... BASES> struct base_array<CLASS, type_list<BASES...>>
    {
        inline static constexpr base_class s_bases[sizeof...(BASES)] = {base_class::make<CLASS, BASES>()...};
    };

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
    constexpr class_type make_static_class(const char * i_name,
      const array_view<const property> & i_properties,
      const array_view<const action> & i_actions,
      std::enable_if_t<all_bases<CLASS>::type::size == 0> * = nullptr) noexcept
    {
        return class_type(i_name, sizeof(CLASS), alignof(CLASS), special_functions::make<CLASS>(),
          array_view<const base_class>(), i_properties, i_actions);
    }

    template <typename CLASS>
    constexpr class_type make_static_class(const char * i_name,
      const array_view<const property> & i_properties,
      const array_view<const action> & i_actions,
      std::enable_if_t<all_bases<CLASS>::type::size != 0> * = nullptr) noexcept
    {
        return class_type(i_name, sizeof(CLASS), alignof(CLASS), special_functions::make<CLASS>(),
          base_array<CLASS, tl_remove_duplicates_t<typename all_bases<CLASS>::type>>::s_bases, i_properties, i_actions);
    }


    namespace detail
    {
        struct Edic_Reflect_Defaults
        {
            constexpr static array_view<const property> properties{};
            constexpr static array_view<const action> actions{};
        };

        template <typename CLASS> class_type constexpr s_class{make_static_class<CLASS>(class_descriptor<CLASS>::name, class_descriptor<CLASS>::properties, class_descriptor<CLASS>::actions)};
    }

    // get_type
    template <typename TYPE, typename = std::enable_if_t<std::is_class_v<TYPE>>>
    constexpr class_type const & get_type() noexcept
    {
        return detail::s_class<TYPE>;
    }

    template <typename DERIVED, typename BASE> constexpr base_class base_class::make() noexcept
    {
        return base_class(get_type<BASE>(), &impl_up_cast<DERIVED, BASE>);
    }

} // namespace ediacaran

#define REFL_BEGIN_CLASS(Name, Class)                                                                                  \
    struct Edic_Reflect_##Class get_type_descriptor(Class *&);                                                         \
    struct Edic_Reflect_##Class : ediacaran::detail::Edic_Reflect_Defaults                                             \
    {                                                                                                                  \
        constexpr static const char * name = Name;                                                                     \
        using this_class = Class;
#define REFL_BASES(...) using bases = ediacaran::type_list<__VA_ARGS__>;

#define REFL_BEGIN_PROPERTIES constexpr static ediacaran::property properties[] = {

#define REFL_DATA_PROP(Name, DataMember)                                                                               \
    ediacaran::detail::make_data_property(                                                                             \
      std::is_const_v<decltype(this_class::DataMember)>                                                                \
        ? ediacaran::property_flags::gettable                                                                          \
        : (ediacaran::property_flags::gettable | ediacaran::property_flags::settable),                                 \
      Name, ediacaran::get_qualified_type<decltype(this_class::DataMember)>(), offsetof(this_class, DataMember)),

#define REFL_ACCESSOR_PROP(Name, Getter, Setter)                                                                       \
    ediacaran::detail::make_accessor_property<ediacaran::detail::PropertyAccessor<decltype(&this_class::Getter),       \
      decltype(&this_class::Setter), &this_class::Getter, &this_class::Setter>>(                                       \
      ediacaran::property_flags::gettable | ediacaran::property_flags::settable, Name),

#define REFL_ACCESSOR_RO_PROP(Name, Getter)                                                                            \
    ediacaran::detail::make_accessor_property<                                                                         \
      ediacaran::detail::PropertyAccessor<decltype(&this_class::Getter), nullptr_t, &this_class::Getter, nullptr>>(    \
      ediacaran::property_flags::gettable, Name),

#define REFL_ACCESSOR_WO_PROP(Name, Setter)                                                                            \
    ediacaran::detail::make_accessor_property<                                                                         \
      ediacaran::detail::PropertyAccessor<nullptr_t, decltype(&this_class::Setter), nullptr, &this_class::Setter>>(    \
      ediacaran::property_flags::settable, Name),

#define REFL_END_PROPERTIES                                                                                            \
    }                                                                                                                  \
    ;

#define REFL_BEGIN_ACTIONS constexpr static ediacaran::action actions[] = {

#define REFL_ACTION(Name, Method, ParameterNames ) ediacaran::detail::make_action<decltype(&this_class::Method), &this_class::Method, \
        ParameterNames>(Name),

#define REFL_END_ACTIONS                                                                                               \
    }                                                                                                                  \
    ;

#define REFL_END_CLASS                                                                                                 \
    }                                                                                                                  \
    ;
