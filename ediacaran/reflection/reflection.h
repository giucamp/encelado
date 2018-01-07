#pragma once

#include "ediacaran/core/remove_noexcept.h"
#include "ediacaran/reflection/class_template_specialization.h"
#include "ediacaran/reflection/class_type.h"
#include "ediacaran/reflection/qualified_type_ptr.h"
#include "ediacaran/reflection/type.h"
#include <array>

#define REFL_DATA_PROP(Name, DataMember)                                                                               \
    ediacaran::detail::make_data_property(                                                                             \
      Name, ediacaran::get_qualified_type<decltype(this_class::DataMember)>(), offsetof(this_class, DataMember))

#define REFL_ACCESSOR_PROP(Name, Getter, Setter)                                                                       \
    ediacaran::detail::make_accessor_property<ediacaran::detail::PropertyAccessor<                                     \
      ediacaran::remove_noexcept_t<decltype(&this_class::Getter)>,                                                     \
      ediacaran::remove_noexcept_t<decltype(&this_class::Setter)>, &this_class::Getter, &this_class::Setter>>(Name)

#define REFL_ACCESSOR_RO_PROP(Name, Getter)                                                                            \
    ediacaran::detail::make_accessor_property<ediacaran::detail::PropertyAccessor<                                     \
      ediacaran::remove_noexcept_t<decltype(&this_class::Getter)>, nullptr_t, &this_class::Getter, nullptr>>(Name)

#define REFL_ACTION(Name, Method, ParameterNames)                                                                      \
    ediacaran::detail::make_action<decltype(&this_class::Method), &this_class::Method>(Name, ParameterNames)

namespace ediacaran
{
    #ifdef _MSC_VER
        // workaround for std::array<T, 0> containing an instance of T
        constexpr std::array<property, 0> empty_properties{{property{property::offset_tag{}, "", qualified_type_ptr{}, 0}}};
        constexpr std::array<parameter, 0> empty_parameters{parameter{"", qualified_type_ptr{}}};
        constexpr std::array<action, 0> empty_actions{action{"", qualified_type_ptr{}, empty_parameters, nullptr} };
    #else
        constexpr std::array<property, 0> empty_properties;
        constexpr std::array<parameter, 0> empty_parameters;
        constexpr std::array<action, 0> empty_actions;
    #endif

    namespace detail
    {
        template <typename INT_TYPE> struct WriteIntTypeName
        {
            constexpr void operator()(char_writer & o_dest) noexcept
            {
                static_assert(std::numeric_limits<INT_TYPE>::radix == 2);

                if constexpr (!std::is_signed_v<INT_TYPE>)
                    o_dest << "uint" << std::numeric_limits<INT_TYPE>::digits;
                else
                    o_dest << "int" << (std::numeric_limits<INT_TYPE>::digits + 1);
            }
        };

        template <typename TYPE> constexpr type MakeFundamentalType(const char * i_name) noexcept
        {
            return type{type_kind::is_fundamental, i_name, sizeof(TYPE), alignof(TYPE),
                        special_functions::template make<TYPE>()};
        }

        template <typename, typename> struct TypeInstance;

        template <> struct TypeInstance<bool, bool>
        {
            constexpr static type instance{MakeFundamentalType<bool>("bool")};
        };

        template <> struct TypeInstance<char, char>
        {
            constexpr static type instance{MakeFundamentalType<char>("char")};
        };

        template <> struct TypeInstance<float, float>
        {
            constexpr static type instance{MakeFundamentalType<float>("float")};
        };

        template <> struct TypeInstance<double, double>
        {
            constexpr static type instance{MakeFundamentalType<double>("double")};
        };

        template <> struct TypeInstance<long double, long double>
        {
            constexpr static type instance{MakeFundamentalType<long double>("long_double")};
        };

        template <> struct TypeInstance<void *, void *>
        {
            constexpr static type instance{MakeFundamentalType<void *>("pointer")};
        };

        template <> struct TypeInstance<void, void>
        {
            constexpr static type instance{type_kind::is_fundamental, "void", 1, 1, special_functions{}};
        };

        template <typename INT_TYPE>
        struct TypeInstance<
          INT_TYPE, std::enable_if_t<std::is_integral_v<INT_TYPE> && !std::is_same_v<INT_TYPE, bool>, INT_TYPE>>
        {
            constexpr static type instance{
              MakeFundamentalType<INT_TYPE>(constexpr_string<detail::WriteIntTypeName<INT_TYPE>>::string.data())};
        };

        // --------------------------------------------------

        template <typename TYPE> using class_descriptor = decltype(reflect(std::declval<TYPE **>()));

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
            using type  = typename all_bases<CLASS, bases>::type;
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
            constexpr static size_t size                       = 1 + sizeof...(OTHER_TYPES);
            using argument_type                                = qualified_type_ptr;
            constexpr static qualified_type_ptr argument_value = get_qualified_type<FIRST_TYPE>();
        };

        struct Edic_Reflect_Defaults
        {
            using bases = type_list<>;
            constexpr static array_view<const property> properties{};
            constexpr static array_view<const action>   actions{};
        };

        template <typename CLASS> struct TypeInstance<CLASS, std::enable_if_t<std::is_class_v<CLASS>, CLASS>>
        {
            constexpr static auto static_class{reflect(static_cast<CLASS * *>(nullptr))};
            constexpr static const class_type & instance = static_class.get_class();
        };

    } // namespace detail

    template <typename CLASS, size_t PROPERTY_COUNT, size_t ACTION_COUNT, typename... BASE_CLASSES>
        struct StaticClass
    {
    public:

        using bases = type_list<BASE_CLASSES...>;
        
        constexpr StaticClass(const char * i_name, std::array<property, PROPERTY_COUNT> const & i_properties, std::array<action, ACTION_COUNT> const & i_actions)
            : m_properties(i_properties), m_actions(i_actions), m_class(i_name,
                sizeof(CLASS),
                alignof(CLASS),
                special_functions::make<CLASS>(),
                detail::base_array<CLASS, tl_remove_duplicates_t<typename detail::all_bases<CLASS>::type>>::s_bases,
                m_properties, m_actions
            )
        {
        }

        constexpr const class_type & get_class() const noexcept
        {
            return m_class;
        }

    private:
        std::array<property, PROPERTY_COUNT> const m_properties;
        std::array<action, ACTION_COUNT> const m_actions;
        class_type const m_class;
    };


    template <typename CLASS, size_t PROPERTY_COUNT, size_t ACTION_COUNT, typename... BASE_CLASSES>
        constexpr auto make_static_cast(const char * i_name,
            type_list<BASE_CLASSES...> /*i_base_classes*/,
            std::array<property, PROPERTY_COUNT> const & i_properties,
            std::array<action, ACTION_COUNT> const & i_actions)
    {
        return StaticClass<CLASS, PROPERTY_COUNT, ACTION_COUNT, BASE_CLASSES...>(i_name, i_properties, i_actions);
    }

    template <typename CLASS, size_t PROPERTY_COUNT, typename... BASE_CLASSES>
        constexpr auto make_static_cast(const char * i_name,
            type_list<BASE_CLASSES...> /*i_base_classes*/,
            std::array<property, PROPERTY_COUNT> const & i_properties)
    {
        return StaticClass<CLASS, PROPERTY_COUNT, 0, BASE_CLASSES...>(i_name, i_properties, empty_actions);
    }

    template <typename CLASS, typename... BASE_CLASSES>
        constexpr auto make_static_cast(const char * i_name,
            type_list<BASE_CLASSES...> /*i_base_classes*/)
    {
        return StaticClass<CLASS, 0, 0, BASE_CLASSES...>(i_name, empty_properties, empty_actions);
    }

    template <typename CLASS>
        constexpr auto make_static_cast(const char * i_name)
    {
        return StaticClass<CLASS, 0, 0>(i_name, empty_properties, empty_actions);
    }

    template <typename TYPE> constexpr const type & get_type() noexcept
    {
        return detail::TypeInstance<TYPE, TYPE>::instance;
    }

    template <typename TYPE> constexpr const class_type & get_class() noexcept
    {
        return detail::TypeInstance<TYPE, TYPE>::instance;
    }

    /** Retrieves (by value) a qualified_type_ptr associated to the template argument.
    The result is never empty (is_empty() always return false). The template
    argument may be void or any void pointer (with any cv-qualification). */
    template <typename TYPE> constexpr qualified_type_ptr get_qualified_type()
    {
        static_assert(
          detail::StaticQualification<TYPE>::s_indirection_levels <= qualified_type_ptr::s_max_indirection_levels,
          "Maximum indirection level exceeded");

        return qualified_type_ptr(
          &get_type<typename detail::StaticQualification<TYPE>::UnderlyingType>(),
          detail::StaticQualification<TYPE>::s_indirection_levels, detail::StaticQualification<TYPE>::s_constness_word,
          detail::StaticQualification<TYPE>::s_volatileness_word);
    }

    constexpr auto reflect(string_view** i_ptr)
    {
        auto const class_name = "ediacaran::string_view";
        using bases = type_list<>;

        using namespace ediacaran;
        using this_class = std::remove_reference_t<decltype(**i_ptr)>;

        auto const properties = make_array(
            REFL_ACCESSOR_RO_PROP("size", size)
        );

        return make_static_cast<this_class>(class_name, bases{}, properties);
    }

    constexpr auto reflect(symbol** i_ptr)
    {
        auto const class_name = "ediacaran::symbol";
        using bases = type_list<>;

        using namespace ediacaran;
        using this_class = std::remove_reference_t<decltype(**i_ptr)>;

        auto const properties = make_array(
            REFL_ACCESSOR_RO_PROP("name", name)
        );

        return make_static_cast<this_class>(class_name, bases{}, properties);
    }

    constexpr auto reflect(type** i_ptr)
    {
        auto const class_name = "ediacaran::type";

        using namespace ediacaran;
        using this_class = std::remove_reference_t<decltype(**i_ptr)>;
        using bases = type_list<symbol>;

        auto const properties = make_array(
            REFL_ACCESSOR_RO_PROP("size", size),
            REFL_ACCESSOR_RO_PROP("alignment", alignment),
            REFL_ACCESSOR_RO_PROP("is_constructible", is_constructible),
            REFL_ACCESSOR_RO_PROP("is_destructible", is_destructible),
            REFL_ACCESSOR_RO_PROP("is_copy_constructible", is_copy_constructible),
            REFL_ACCESSOR_RO_PROP("is_move_constructible", is_move_constructible),
            REFL_ACCESSOR_RO_PROP("is_copy_assignable", is_copy_assignable),
            REFL_ACCESSOR_RO_PROP("is_move_assignable", is_move_assignable),
            REFL_ACCESSOR_RO_PROP("is_comparable", is_comparable),
            REFL_ACCESSOR_RO_PROP("is_stringizable", is_stringizable),
            REFL_ACCESSOR_RO_PROP("is_parsable", is_parsable)
        );

        return make_static_cast<this_class>(class_name, bases{}, properties);
    }
}