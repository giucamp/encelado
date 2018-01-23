#pragma once

#include "ediacaran/core/constexpr_string.h"
#include "ediacaran/core/remove_noexcept.h"
#include "ediacaran/reflection/class_template_specialization.h"
#include "ediacaran/reflection/class_type.h"
#include "ediacaran/reflection/enum_type.h"
#include "ediacaran/reflection/qualified_type_ptr.h"
#include <ediacaran/core/array.h>

#define REFL_DATA_PROP(Name, DataMember)                                                           \
    ediacaran::detail::make_data_property(                                                         \
      Name,                                                                                        \
      ediacaran::get_qualified_type<decltype(this_class::DataMember)>(),                           \
      offsetof(this_class, DataMember))

#define REFL_ACCESSOR_PROP(Name, Getter, Setter)                                                   \
    ediacaran::detail::make_accessor_property<ediacaran::detail::PropertyAccessor<                 \
      ediacaran::remove_noexcept_t<decltype(&this_class::Getter)>,                                 \
      ediacaran::remove_noexcept_t<decltype(&this_class::Setter)>,                                 \
      &this_class::Getter,                                                                         \
      &this_class::Setter>>(Name)

#define REFL_ACCESSOR_RO_PROP(Name, Getter)                                                        \
    ediacaran::detail::make_accessor_property<ediacaran::detail::PropertyAccessor<                 \
      ediacaran::remove_noexcept_t<decltype(&this_class::Getter)>,                                 \
      std::nullptr_t,                                                                              \
      &this_class::Getter,                                                                         \
      nullptr>>(Name)

#define REFL_ACTION(Name, Method, ParameterNames)                                                  \
    ediacaran::detail::make_action<decltype(&this_class::Method), &this_class::Method>(            \
      Name, ParameterNames)

namespace ediacaran
{
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
            return type{type_kind::is_fundamental,
                        i_name,
                        sizeof(TYPE),
                        alignof(TYPE),
                        special_functions::template make<TYPE>()};
        }

        template <typename, typename> struct TypeInstance;

        constexpr type g_type_bool{MakeFundamentalType<bool>("bool")};
        constexpr type g_type_char{MakeFundamentalType<char>("char")};
        constexpr type g_type_float{MakeFundamentalType<float>("float")};
        constexpr type g_type_pointer{MakeFundamentalType<void *>("pointer")};
        constexpr type g_type_double{MakeFundamentalType<double>("double")};
        constexpr type g_type_long_double{MakeFundamentalType<long double>("long_double")};
        constexpr type g_type_void{type_kind::is_fundamental, "void", 1, 1, special_functions{}};

        template <> struct TypeInstance<bool, bool>
        {
            constexpr static const auto & instance() noexcept { return g_type_bool; }
        };

        template <> struct TypeInstance<char, char>
        {
            constexpr static const auto & instance() noexcept { return g_type_char; }
        };

        template <> struct TypeInstance<float, float>
        {
            constexpr static const auto & instance() noexcept { return g_type_float; }
        };

        template <> struct TypeInstance<double, double>
        {
            constexpr static const auto & instance() noexcept { return g_type_double; }
        };

        template <> struct TypeInstance<long double, long double>
        {
            constexpr static const auto & instance() noexcept { return g_type_long_double; }
        };

        template <> struct TypeInstance<void *, void *>
        {
            constexpr static const auto & instance() noexcept { return g_type_pointer; }
        };

        template <> struct TypeInstance<void, void>
        {
            constexpr static const auto & instance() noexcept { return g_type_void; }
        };

        template <typename INT_TYPE>
        struct TypeInstance<
          INT_TYPE,
          std::
            enable_if_t<std::is_integral_v<INT_TYPE> && !std::is_same_v<INT_TYPE, bool>, INT_TYPE>>
        {
            constexpr static type         s_instance{MakeFundamentalType<INT_TYPE>(
              constexpr_string<WriteIntTypeName<INT_TYPE>>::string.data())};
            constexpr static const auto & instance() noexcept { return s_instance; }
        };

        // --------------------------------------------------

        template <typename TYPE>
        using class_descriptor = decltype(reflect(std::declval<TYPE **>()));

        // BasesArray - array of base_class, constructed from an input type_list
        template <typename...> struct BasesArray;
        template <typename CLASS, typename... BASES> struct BasesArray<CLASS, type_list<BASES...>>
        {
            constexpr static base_class s_bases[sizeof...(BASES)] = {
              base_class::make<CLASS, BASES>()...};
        };
        template <typename CLASS> struct BasesArray<CLASS, type_list<>>
        {
            constexpr static array_view<const base_class> s_bases{};
        };

        // makes a list of all the direct and indirect bases of CLASS using class_descriptor<...>::bases as input
        template <typename...> struct BasesTypeList;
        template <typename CLASS> // this expands <CLASS> to <CLASS, type_list<BASES...>>
        struct BasesTypeList<CLASS>
        {
            using bases = typename class_descriptor<CLASS>::bases;
            using type  = typename BasesTypeList<CLASS, bases>::type;
        };
        template <typename CLASS, typename... BASES>
        struct BasesTypeList<CLASS, type_list<BASES...>>
        {
            using type =
              tl_push_back_t<type_list<BASES...>, typename BasesTypeList<BASES>::type...>;
        };

        // TemplateAutoDeducer
        template <typename CLASS> struct TemplateAutoDeducer
        {
            constexpr static size_t               arguments_count = 0;
            constexpr static template_arguments<> get_template_arguments() noexcept
            {
                return make_template_arguments();
            }
        };
        template <template <class...> class CLASS, typename... TYPES>
        struct TemplateAutoDeducer<CLASS<TYPES...>>
        {
            struct Writer
            {
                constexpr void operator()(char_writer & i_writer)
                {
                    for (size_t index = 0; index < sizeof...(TYPES); index++)
                    {
                        i_writer << "a" << index;
                        if (index + 1 < sizeof...(TYPES))
                            i_writer << ", ";
                    }
                }
            };

            constexpr static size_t arguments_count = sizeof...(TYPES);
            constexpr static auto   get_template_arguments() noexcept
            {
                return make_template_arguments(
                  constexpr_string<Writer>::string.data(), get_qualified_type<TYPES>()...);
            }
        };

        template <typename UDT>
        struct TypeInstance<UDT, std::enable_if_t<std::is_class_v<UDT> || std::is_enum_v<UDT>, UDT>>
        {
            constexpr static auto         static_class{reflect(static_cast<UDT **>(nullptr))};
            constexpr static const auto & instance() noexcept { return static_class.get_class(); }
        };

        template <
          typename CLASS,
          size_t NAME_LENGTH,
          typename TEMPLATE_PARAMETER_LIST,
          size_t PROPERTY_COUNT,
          size_t ACTION_COUNT,
          typename... BASE_CLASSES>
        struct StaticClass;

        template <
          typename CLASS,
          size_t SPECIALIZATION_NAME_LENGTH,
          size_t PROPERTY_COUNT,
          size_t ACTION_COUNT,
          typename... TEMPLATE_PARAMETERS,
          typename... BASE_CLASSES>
        struct StaticClass<
          CLASS,
          SPECIALIZATION_NAME_LENGTH,
          template_arguments<TEMPLATE_PARAMETERS...>,
          PROPERTY_COUNT,
          ACTION_COUNT,
          BASE_CLASSES...>
        {
            template <size_t... INDEX>
            constexpr auto make_template_parameters_array(
              const template_arguments<TEMPLATE_PARAMETERS...> & i_template_arguments,
              std::index_sequence<INDEX...>)
            {
                return array<const parameter, sizeof...(TEMPLATE_PARAMETERS)>{
                  {parameter{get_qualified_type<std::remove_reference_t<decltype(
                    i_template_arguments.template get<INDEX>())>>()}...}};
            }

            template <size_t... INDEX>
            constexpr auto make_template_arguments_array(
              const template_arguments<TEMPLATE_PARAMETERS...> & i_template_arguments,
              std::index_sequence<INDEX...>)
            {
                return array<void const * const, sizeof...(TEMPLATE_PARAMETERS)>{
                  {&i_template_arguments.template get<INDEX>()...}};
            }

          public:
            using bases = type_list<BASE_CLASSES...>;

            // type list of all the direct and indirect base classes
            using all_bases = tl_remove_duplicates_t<
              tl_push_back_t<bases, typename BasesTypeList<BASE_CLASSES>::type...>>;

            constexpr StaticClass(
              const array<char, SPECIALIZATION_NAME_LENGTH> &    i_specialization_name,
              const template_arguments<TEMPLATE_PARAMETERS...> & i_template_arguments,
              array<property, PROPERTY_COUNT> const &            i_properties,
              array<action, ACTION_COUNT> const &                i_actions)
                : m_specialization_name(i_specialization_name), m_properties(i_properties),
                  m_actions(i_actions), m_template_arguments(i_template_arguments),
                  m_template_parameters_array(make_template_parameters_array(
                    m_template_arguments,
                    std::make_index_sequence<sizeof...(TEMPLATE_PARAMETERS)>{})),
                  m_template_arguments_array(make_template_arguments_array(
                    m_template_arguments,
                    std::make_index_sequence<sizeof...(TEMPLATE_PARAMETERS)>{})),
                  m_class(
                    m_specialization_name.data(),
                    m_template_arguments.parameter_names(),
                    sizeof(CLASS),
                    alignof(CLASS),
                    special_functions::make<CLASS>(),
                    BasesArray<CLASS, all_bases>::s_bases,
                    m_properties,
                    m_actions,
                    m_template_parameters_array,
                    m_template_arguments_array)
            {
            }

            constexpr const class_template_specialization & get_class() const noexcept
            {
                return m_class;
            }

          private:
            array<char, SPECIALIZATION_NAME_LENGTH>          m_specialization_name;
            array<property, PROPERTY_COUNT> const            m_properties;
            array<action, ACTION_COUNT> const                m_actions;
            template_arguments<TEMPLATE_PARAMETERS...> const m_template_arguments;
            array<const parameter, sizeof...(TEMPLATE_PARAMETERS)> const
              m_template_parameters_array;
            array<const void * const, sizeof...(TEMPLATE_PARAMETERS)> const
                                                m_template_arguments_array;
            class_template_specialization const m_class;
        };

        template <
          typename CLASS,
          size_t CLASS_NAME_LENGTH,
          size_t PROPERTY_COUNT,
          size_t ACTION_COUNT,
          typename... BASE_CLASSES>
        struct StaticClass<
          CLASS,
          CLASS_NAME_LENGTH,
          template_arguments<>,
          PROPERTY_COUNT,
          ACTION_COUNT,
          BASE_CLASSES...>
        {
          public:
            using bases = type_list<BASE_CLASSES...>;

            // type list of all the direct and indirect base classes
            using all_bases = tl_remove_duplicates_t<
              tl_push_back_t<bases, typename BasesTypeList<BASE_CLASSES>::type...>>;

            constexpr StaticClass(
              const array<char, CLASS_NAME_LENGTH> & i_name,
              template_arguments<>,
              array<property, PROPERTY_COUNT> const & i_properties,
              array<action, ACTION_COUNT> const &     i_actions)
                : m_name(i_name), m_properties(i_properties), m_actions(i_actions),
                  m_class(
                    m_name.data(),
                    sizeof(CLASS),
                    alignof(CLASS),
                    special_functions::make<CLASS>(),
                    BasesArray<CLASS, all_bases>::s_bases,
                    m_properties,
                    m_actions)
            {
            }

            constexpr const class_type & get_class() const noexcept { return m_class; }

          private:
            array<char, CLASS_NAME_LENGTH>        m_name;
            array<property, PROPERTY_COUNT> const m_properties;
            array<action, ACTION_COUNT> const     m_actions;
            class_type const                      m_class;
        };

        //
        template <typename ENUM_TYPE, size_t NAME_LENGTH, size_t MEMBER_COUNT> class StaticEnum
        {
          public:
            using underlying_type = std::underlying_type_t<ENUM_TYPE>;

            constexpr StaticEnum(
              const array<char, NAME_LENGTH> &                        i_name,
              array<enum_member<underlying_type>, MEMBER_COUNT> const i_members)
                : m_name(i_name), m_members(i_members), m_enum(
                                          m_name.data(),
                                          sizeof(ENUM_TYPE),
                                          alignof(ENUM_TYPE),
                                          special_functions::template make<ENUM_TYPE>(),
                                          m_members)
            {
            }

            constexpr const enum_type<underlying_type> & get_class() const noexcept
            {
                return m_enum;
            }

          private:
            array<char, NAME_LENGTH>                                m_name;
            array<enum_member<underlying_type>, MEMBER_COUNT> const m_members;
            enum_type<underlying_type> const                        m_enum;
        };

    } // namespace detail

    template <
      typename CLASS,
      size_t CLASS_NAME_SIZE,
      typename... BASE_CLASSES,
      size_t PROPERTY_COUNT = 0,
      size_t ACTION_COUNT   = 0>
    constexpr auto make_static_cast(
      const char (&i_name)[CLASS_NAME_SIZE],
      type_list<BASE_CLASSES...> /*i_base_classes*/        = type_list<>{},
      array<property, PROPERTY_COUNT> const & i_properties = array<property, 0>{},
      array<action, ACTION_COUNT> const &     i_actions    = array<action, 0>{})
    {
        if constexpr (detail::TemplateAutoDeducer<CLASS>::arguments_count == 0)
        {
            array<char, CLASS_NAME_SIZE> name{};
            to_chars(name.data(), CLASS_NAME_SIZE, i_name);
            return detail::StaticClass<
              CLASS,
              CLASS_NAME_SIZE,
              template_arguments<>,
              PROPERTY_COUNT,
              ACTION_COUNT,
              BASE_CLASSES...>(name, make_template_arguments(), i_properties, i_actions);
        }
        else
        {
            constexpr auto auto_template_arguments =
              detail::TemplateAutoDeducer<CLASS>::get_template_arguments();
            constexpr auto template_arguments_str_size = char_array_size(auto_template_arguments);
            array<char, CLASS_NAME_SIZE + template_arguments_str_size> name{};
            to_chars(
              name.data(),
              CLASS_NAME_SIZE + template_arguments_str_size,
              i_name,
              auto_template_arguments);
            return detail::StaticClass<
              CLASS,
              CLASS_NAME_SIZE + template_arguments_str_size,
              std::decay_t<decltype(auto_template_arguments)>,
              PROPERTY_COUNT,
              ACTION_COUNT,
              BASE_CLASSES...>(name, auto_template_arguments, i_properties, i_actions);
        }
    }

    template <
      typename CLASS,
      size_t ARGUMENTS_STRING_SIZE,
      size_t TEMPLATE_NAME_SIZE,
      typename... TEMPLATE_PARAMETERS,
      typename... BASE_CLASSES,
      size_t PROPERTY_COUNT = 0,
      size_t ACTION_COUNT   = 0>
    constexpr auto make_static_cast(
      const char (&i_template_name)[TEMPLATE_NAME_SIZE],
      const template_arguments<TEMPLATE_PARAMETERS...> & i_template_arguments,
      type_list<BASE_CLASSES...> /*i_base_classes*/        = type_list<>{},
      array<property, PROPERTY_COUNT> const & i_properties = array<property, 0>{},
      array<action, ACTION_COUNT> const &     i_actions    = array<action, 0>{})
    {
        array<char, ARGUMENTS_STRING_SIZE + TEMPLATE_NAME_SIZE> specialization_name{};
        to_chars(
          specialization_name.data(),
          specialization_name.size(),
          i_template_name,
          i_template_arguments);
        return detail::StaticClass<
          CLASS,
          ARGUMENTS_STRING_SIZE + TEMPLATE_NAME_SIZE,
          template_arguments<TEMPLATE_PARAMETERS...>,
          PROPERTY_COUNT,
          ACTION_COUNT,
          BASE_CLASSES...>(specialization_name, i_template_arguments, i_properties, i_actions);
    }

    template <typename ENUM_TYPE, size_t NAME_SIZE, size_t MEMBER_COUNT = 0>
    constexpr auto make_enum(
      const char(&i_name)[NAME_SIZE],
      array<enum_member<std::underlying_type_t<ENUM_TYPE>>, MEMBER_COUNT> const & i_members =
        array<enum_member<std::underlying_type_t<ENUM_TYPE>>, 0>{})
    {
        array<char, NAME_SIZE> name{};
        to_chars(name.data(), NAME_SIZE, i_name);
        return detail::StaticEnum<ENUM_TYPE, NAME_SIZE, MEMBER_COUNT>(name, i_members);
    }

    template <typename ENUM_TYPE>
        constexpr auto make_enum_member(const char * i_name, ENUM_TYPE i_value)
    {
        return enum_member<std::underlying_type_t<ENUM_TYPE>>(i_name, static_cast<std::underlying_type_t<ENUM_TYPE>>(i_value));
    }

    template <typename TYPE> constexpr const auto & get_type() noexcept
    {
        return detail::TypeInstance<TYPE, TYPE>::instance();
    }

    constexpr const type & get_ptr_type() noexcept
    {
        return detail::TypeInstance<void *, void *>::instance();
    }

    template <typename TYPE> constexpr const class_type & get_class_type() noexcept
    {
        return detail::TypeInstance<TYPE, TYPE>::instance();
    }

    template <typename TYPE> constexpr const enum_type<std::underlying_type_t<TYPE>> & get_enum_type() noexcept
    {
        return detail::TypeInstance<TYPE, TYPE>::instance();
    }

    /** Retrieves (by value) a qualified_type_ptr associated to the template argument.
    The result is never empty (is_empty() always return false). The template
    argument may be void or any void pointer (with any cv-qualification). */
    template <typename TYPE> constexpr qualified_type_ptr get_qualified_type()
    {
        static_assert(
          detail::StaticQualification<TYPE>::s_indirection_levels <=
            qualified_type_ptr::s_max_indirection_levels,
          "Maximum indirection level exceeded");

        return qualified_type_ptr(
          &get_type<typename detail::StaticQualification<TYPE>::UnderlyingType>(),
          detail::StaticQualification<TYPE>::s_indirection_levels,
          detail::StaticQualification<TYPE>::s_constness_word,
          detail::StaticQualification<TYPE>::s_volatileness_word);
    }

    constexpr auto reflect(string_view ** i_ptr)
    {
        char const class_name[] = "ediacaran::string_view";
        using bases             = type_list<>;

        using namespace ediacaran;
        using this_class = std::remove_reference_t<decltype(**i_ptr)>;

        auto const properties = make_array(REFL_ACCESSOR_RO_PROP("size", size));

        return make_static_cast<this_class>(class_name, bases{}, properties);
    }

    constexpr auto reflect(symbol ** i_ptr)
    {
        char const class_name[] = "ediacaran::symbol";
        using bases             = type_list<>;

        using namespace ediacaran;
        using this_class = std::remove_reference_t<decltype(**i_ptr)>;

        auto const properties = make_array(REFL_ACCESSOR_RO_PROP("name", name));

        return make_static_cast<this_class>(class_name, bases{}, properties);
    }

    constexpr auto reflect(type ** i_ptr)
    {
        char const class_name[] = "ediacaran::type";

        using namespace ediacaran;
        using this_class = std::remove_reference_t<decltype(**i_ptr)>;
        using bases      = type_list<>;

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
          REFL_ACCESSOR_RO_PROP("is_parsable", is_parsable));

        return make_static_cast<this_class>(class_name, bases{}, properties);
    }

    constexpr auto reflect(qualified_type_ptr ** i_ptr)
    {
        char const class_name[] = "ediacaran::qualified_type_ptr";
        using this_class        = std::remove_reference_t<decltype(**i_ptr)>;
        return make_static_cast<this_class>(class_name);
    }
}
