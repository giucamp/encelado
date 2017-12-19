#pragma once

#include  <cstddef>
#include "ediacaran/core/array_view.h"
#include "ediacaran/core/type_list.h"
#include "ediacaran/reflection/type.h"
#include "ediacaran/reflection/property.h"
#include "ediacaran/reflection/special_functions.h"

namespace ediacaran
{
    class class_type;

    struct base_class
    {
    public:

        template <typename DERIVED, typename BASE>
            constexpr static base_class make() noexcept
        {
            return base_class(get_naked_type<BASE>(), &impl_up_cast<DERIVED,BASE>);
        }

        void * up_cast(void * i_derived) const noexcept
        {
            return (*m_up_caster)(i_derived);
        }

        const void * up_cast(const void * i_derived) const noexcept
        {
            return (*m_up_caster)(const_cast<void*>(i_derived));
        }

        constexpr class_type const & get_class() const noexcept { return m_class; }

    private:

        base_class(class_type const & i_class, void * (*i_up_caster)(void*) EDIACARAN_NOEXCEPT_FUNCTION_TYPE)
            : m_class(i_class), m_up_caster(i_up_caster)
        {
        }

        template <typename DERIVED, typename BASE>
            static void * impl_up_cast(void * i_derived) noexcept
        {
            auto const derived = static_cast<DERIVED*>(i_derived);
            return static_cast<BASE*>(derived);
        }

    private:
        class_type const & m_class;
        void * (* const m_up_caster)(void*) EDIACARAN_NOEXCEPT_FUNCTION_TYPE;
    };

    class class_type : public type_t
    {
      public:

        constexpr class_type(const char * const i_name, size_t i_size, size_t i_alignment,
          const special_functions & i_special_functions,
          const array_view<const base_class> & i_base_classes,
          const array_view<const property> & i_properties) noexcept
            : type_t(i_name, i_size, i_alignment, i_special_functions),
              m_base_classes(i_base_classes), m_properties(i_properties)
        {
        }

        constexpr array_view<const base_class> const & base_classes() const noexcept { return m_base_classes; }

        constexpr array_view<const property> const & properties() const noexcept { return m_properties; }

      private:
        array_view<const base_class> const m_base_classes;
        array_view<const property> const m_properties;
    };

    template <typename TYPE> using class_descriptor = decltype(get_type_descriptor(std::declval<TYPE*&>()));

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
    constexpr class_type make_static_class(
      const char * i_name, 
      const array_view<const property> & i_properties,
      std::enable_if_t<all_bases<CLASS>::type::size == 0> * = nullptr) noexcept
    {
        return class_type(i_name, sizeof(CLASS), alignof(CLASS), special_functions::make<CLASS>(),
            array_view<const base_class>(),
            i_properties);
    }

    template <typename CLASS>
    constexpr class_type make_static_class(
      const char * i_name, 
      const array_view<const property> & i_properties,
      std::enable_if_t<all_bases<CLASS>::type::size != 0> * = nullptr) noexcept
    {
        return class_type(i_name, sizeof(CLASS), alignof(CLASS), special_functions::make<CLASS>(),
          base_array<CLASS, tl_remove_duplicates_t<typename all_bases<CLASS>::type> >::s_bases,
          i_properties);
    }


    namespace detail
    {
        template <typename CLASS, typename = std::void_t<>>
            struct PropTraits
        {
            constexpr static const array_view<const property> get()
            {
                return array_view<const property>();
            }
        };

        template <typename CLASS>
            struct PropTraits<CLASS, std::void_t<decltype(class_descriptor<CLASS>::properties)>>
        {
            constexpr static const array_view<const property> get()
            {
                return class_descriptor<CLASS>::properties;
            }
        };

        template <typename CLASS>
            constexpr class_type create_class()
        {
            return make_static_class<CLASS>(class_descriptor<CLASS>::name, PropTraits<CLASS>::get());
        }

        template <typename CLASS> class_type const s_class{create_class<CLASS>()};
    }

    // get_naked_type
    template <typename TYPE, typename = std::enable_if_t<std::is_class_v<TYPE>>>
    constexpr class_type const & get_naked_type() noexcept
    {
        return detail::s_class<TYPE>;
    }

    template <typename...> struct base_array;

    template <typename CLASS, typename... BASES> struct base_array<CLASS, type_list<BASES...>>
    {
        inline static const base_class s_bases[sizeof...(BASES)] = {base_class::make<CLASS, BASES>()...};
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

#define REFL_BEGIN_CLASS(Name, Class)           struct Edic_Reflect_##Class get_type_descriptor(Class*&);\
                                                struct Edic_Reflect_##Class {\
                                                    constexpr static char * name = Name; \
                                                    using this_class = Class;
#define REFL_BASES(...)                         using bases = ediacaran::type_list<__VA_ARGS__>;

#define REFL_BEGIN_PROPERTIES                   constexpr static ediacaran::property properties[] = {

#define REFL_DATA_PROP(Name, DataMember)        ediacaran::make_data_property(Name, ediacaran::get_qualified_type<decltype(this_class::DataMember)>(), offsetof(this_class, DataMember)),

#define REFL_ACCESSOR_PROP(Name, Getter, Setter) make_accessor_property< ediacaran::detail::PropertyAccessor< \
                                                decltype(&this_class::Getter), decltype(&this_class::Setter), \
                                                &this_class::Getter, &this_class::Setter> >(Name),

#define REFL_END_PROPERTIES                     };

#define REFL_END_CLASS                          };
