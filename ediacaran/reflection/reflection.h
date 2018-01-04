#pragma once

#include "ediacaran/core/remove_noexcept.h"
#include "ediacaran/reflection/class_template_specialization.h"
#include "ediacaran/reflection/class_type.h"
#include "ediacaran/reflection/qualified_type_ptr.h"
#include "ediacaran/reflection/type.h"

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
      Name, ediacaran::get_qualified_type<decltype(this_class::DataMember)>(), offsetof(this_class, DataMember)),

#define REFL_ACCESSOR_PROP(Name, Getter, Setter)                                                                       \
    ediacaran::detail::make_accessor_property<                                                                         \
      ediacaran::detail::PropertyAccessor<ediacaran::remove_noexcept_t<decltype(&this_class::Getter)>,                 \
        ediacaran::remove_noexcept_t<decltype(&this_class::Setter)>, &this_class::Getter, &this_class::Setter>>(Name),

#define REFL_ACCESSOR_RO_PROP(Name, Getter)                                                                            \
    ediacaran::detail::make_accessor_property<ediacaran::detail::PropertyAccessor<                                     \
      ediacaran::remove_noexcept_t<decltype(&this_class::Getter)>, nullptr_t, &this_class::Getter, nullptr>>(Name),

#define REFL_END_PROPERTIES                                                                                            \
    }                                                                                                                  \
    ;

#define REFL_BEGIN_ACTIONS constexpr static ediacaran::action actions[] = {

#define REFL_ACTION(Name, Method, ParameterNames)                                                                      \
    ediacaran::detail::make_action<decltype(&this_class::Method), &this_class::Method, ParameterNames>(Name),

#define REFL_END_ACTIONS                                                                                               \
    }                                                                                                                  \
    ;

#define REFL_END_CLASS                                                                                                 \
    }                                                                                                                  \
    ;

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
            return type{
              type_kind::is_fundamental, i_name, sizeof(TYPE), alignof(TYPE), special_functions::template make<TYPE>()};
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
        struct TypeInstance<INT_TYPE,
          std::enable_if_t<std::is_integral_v<INT_TYPE> && !std::is_same_v<INT_TYPE, bool>, INT_TYPE>>
        {
            constexpr static type instance{
              MakeFundamentalType<INT_TYPE>(constexpr_string<detail::WriteIntTypeName<INT_TYPE>>::string.data())};
        };

        // --------------------------------------------------

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
            using argument_type = qualified_type_ptr;
            constexpr static qualified_type_ptr argument_value = get_qualified_type<FIRST_TYPE>();
        };

        struct Edic_Reflect_Defaults
        {
            using bases = type_list<>;
            constexpr static array_view<const property> properties{};
            constexpr static array_view<const action> actions{};
        };

        template <typename CLASS> struct TypeInstance<CLASS, std::enable_if_t<std::is_class_v<CLASS>, CLASS>>
        {
            constexpr static class_type instance{class_descriptor<CLASS>::name, sizeof(CLASS), alignof(CLASS),
              special_functions::make<CLASS>(),
              base_array<CLASS, tl_remove_duplicates_t<typename all_bases<CLASS>::type>>::s_bases,
              class_descriptor<CLASS>::properties, class_descriptor<CLASS>::actions};
        };

    } // namespace detail

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

        return qualified_type_ptr(&get_type<typename detail::StaticQualification<TYPE>::UnderlyingType>(),
          detail::StaticQualification<TYPE>::s_indirection_levels, detail::StaticQualification<TYPE>::s_constness_word,
          detail::StaticQualification<TYPE>::s_volatileness_word);
    }

    REFL_BEGIN_CLASS("ediacaran::string_view", string_view)
    REFL_BEGIN_PROPERTIES
    REFL_ACCESSOR_RO_PROP("size", size)
    REFL_END_PROPERTIES
    REFL_END_CLASS;

    REFL_BEGIN_CLASS("ediacaran::symbol", symbol)
    REFL_BEGIN_PROPERTIES
    REFL_ACCESSOR_RO_PROP("name", name)
    REFL_END_PROPERTIES
    REFL_END_CLASS;

    REFL_BEGIN_CLASS("ediacaran::type", type)
    REFL_BASES(symbol)
    REFL_BEGIN_PROPERTIES
    REFL_ACCESSOR_RO_PROP("size", size)
    REFL_ACCESSOR_RO_PROP("alignment", alignment)
    REFL_ACCESSOR_RO_PROP("is_constructible", is_constructible)
    REFL_ACCESSOR_RO_PROP("is_destructible", is_destructible)
    REFL_ACCESSOR_RO_PROP("is_copy_constructible", is_copy_constructible)
    REFL_ACCESSOR_RO_PROP("is_move_constructible", is_move_constructible)
    REFL_ACCESSOR_RO_PROP("is_copy_assignable", is_copy_assignable)
    REFL_ACCESSOR_RO_PROP("is_move_assignable", is_move_assignable)
    REFL_ACCESSOR_RO_PROP("is_comparable", is_comparable)
    REFL_ACCESSOR_RO_PROP("is_stringizable", is_stringizable)
    REFL_ACCESSOR_RO_PROP("is_parsable", is_parsable)
    REFL_END_PROPERTIES
    REFL_END_CLASS;
}