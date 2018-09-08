
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#define REFL_DATA_PROP(Name, DataMember)                                                           \
    edi::make_property<decltype(this_class::DataMember), offsetof(this_class, DataMember)>(Name)

#define REFL_ACCESSOR_PROP(Name, Getter, Setter)                                                   \
    edi::make_property<&this_class::Getter, &this_class::Setter>(Name)

#define REFL_ACCESSOR_RO_PROP(Name, Getter) edi::make_property<&this_class::Getter, nullptr>(Name)

#define REFL_FUNCTION(Name, Method, ParameterNames)                                                \
    edi::make_function<&this_class::Method>(Name, ParameterNames)

#define EDI_DATA(DataMember) decltype(this_class::DataMember), offsetof(this_class, DataMember)

#define EDI_FUNC(Method) &this_class::Method

namespace edi
{
    template <typename UDT> struct non_intrusive_reflection;

    namespace detail
    {
#define EDIACARAN_CLS_REFL_DEFINE_EXPRESSION_TRAIT(Name, Expression)                               \
    template <typename TYPE, typename = std::void_t<>> struct Name : std::false_type               \
    {                                                                                              \
    };                                                                                             \
    template <typename TYPE> struct Name<TYPE, std::void_t<decltype(Expression)>> : std::true_type \
    {                                                                                              \
    }

        EDIACARAN_CLS_REFL_DEFINE_EXPRESSION_TRAIT(
          HasNonintrusiveReflection, non_intrusive_reflection<TYPE>::reflect());
        EDIACARAN_CLS_REFL_DEFINE_EXPRESSION_TRAIT(
          HasAdlReflection, reflect(static_cast<TYPE **>(nullptr)));

#undef EDIACARAN_CLS_REFL_DEFINE_EXPRESSION_TRAIT

        template <typename UDT> static constexpr auto reflect()
        {
            if constexpr (HasNonintrusiveReflection<UDT>::value)
                return non_intrusive_reflection<UDT>::reflect();
            else if constexpr (HasAdlReflection<UDT>::value)
                return reflect(static_cast<UDT **>(nullptr));
            else
            {
                struct UnreflectedType
                {
                };
                UDT t = UnreflectedType{};
                //static_assert(false, "This type is not reflected");
            }
        }

        template <typename TYPE> using class_descriptor = decltype(reflect<TYPE>());

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
            constexpr static auto         static_class{reflect<UDT>()};
            constexpr static const auto & instance() noexcept
            {
                if constexpr (std::is_convertible_v<decltype(&static_class), const class_type *>)
                    return static_class;
                else
                    return static_class.get_class();
            }
        };

        template <
          typename CLASS,
          size_t NAME_LENGTH,
          typename TEMPLATE_PARAMETER_LIST,
          size_t PROPERTY_COUNT,
          size_t FUNCTION_COUNT,
          typename BASE_CLASSES_LIST,
          bool IS_CONTAINER>
        struct StaticClass;

        template <typename TYPE, bool IS_CONTAINER> struct ContainerBase
        {
            constexpr container * get_ptr() noexcept { return nullptr; }
        };

        template <typename TYPE> struct ContainerBase<TYPE, true> : container
        {
            constexpr ContainerBase() : container(make_container_reflection<TYPE>()) {}

            constexpr container * get_ptr() noexcept { return this; }
        };

        template <
          typename CLASS,
          size_t SPECIALIZATION_NAME_LENGTH,
          size_t PROPERTY_COUNT,
          size_t FUNCTION_COUNT,
          typename... TEMPLATE_PARAMETERS,
          typename... BASE_CLASSES,
          bool IS_CONTAINER>
        struct StaticClass<
          CLASS,
          SPECIALIZATION_NAME_LENGTH,
          template_arguments<TEMPLATE_PARAMETERS...>,
          PROPERTY_COUNT,
          FUNCTION_COUNT,
          type_list<BASE_CLASSES...>,
          IS_CONTAINER> : ContainerBase<CLASS, IS_CONTAINER>
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
              array<function, FUNCTION_COUNT> const &            i_functions)
                : m_specialization_name(i_specialization_name), m_properties(i_properties),
                  m_functions(i_functions), m_template_arguments(i_template_arguments),
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
                    m_functions,
                    m_template_parameters_array,
                    m_template_arguments_array,
                    ContainerBase<CLASS, IS_CONTAINER>::get_ptr())
            {
            }

            constexpr const class_template_specialization & get_class() const noexcept
            {
                return m_class;
            }

          private:
            array<char, SPECIALIZATION_NAME_LENGTH>          m_specialization_name;
            array<property, PROPERTY_COUNT> const            m_properties;
            array<function, FUNCTION_COUNT> const            m_functions;
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
          size_t FUNCTION_COUNT,
          typename... BASE_CLASSES,
          bool IS_CONTAINER>
        struct StaticClass<
          CLASS,
          CLASS_NAME_LENGTH,
          template_arguments<>,
          PROPERTY_COUNT,
          FUNCTION_COUNT,
          type_list<BASE_CLASSES...>,
          IS_CONTAINER> : ContainerBase<CLASS, IS_CONTAINER>
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
              array<function, FUNCTION_COUNT> const & i_functions)
                : m_name(i_name), m_properties(i_properties), m_functions(i_functions),
                  m_class(
                    m_name.data(),
                    sizeof(CLASS),
                    alignof(CLASS),
                    special_functions::make<CLASS>(),
                    BasesArray<CLASS, all_bases>::s_bases,
                    m_properties,
                    m_functions,
                    ContainerBase<CLASS, IS_CONTAINER>::get_ptr())
            {
            }

            constexpr const class_type & get_class() const noexcept { return m_class; }

          private:
            array<char, CLASS_NAME_LENGTH>        m_name;
            array<property, PROPERTY_COUNT> const m_properties;
            array<function, FUNCTION_COUNT> const m_functions;
            class_type const                      m_class;
        };

        // properties

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

            static void
              getter_impl(property const &, const void * i_source_object, void * o_value_dest)
            {
                EDIACARAN_ASSERT(i_source_object != nullptr);
                EDIACARAN_ASSERT(o_value_dest != nullptr);
                auto const object = static_cast<const CLASS *>(i_source_object);
                new (o_value_dest) property_type((object->*GETTER)());
            }

            static void
              setter_impl(property const &, void * i_dest_object, const void * i_value_source)
            {
                EDIACARAN_ASSERT(i_dest_object != nullptr);
                EDIACARAN_ASSERT(i_value_source != nullptr);
                auto const dest_object = static_cast<CLASS *>(i_dest_object);
                (dest_object->*SETTER)(*static_cast<const property_type *>(i_value_source));
            }

            constexpr static property::getter getter() { return &getter_impl; }
            constexpr static property::setter setter() { return &setter_impl; }
        };

        template <
          typename CLASS,
          typename GETTER_RETURN_TYPE,
          GETTER_RETURN_TYPE (CLASS::*GETTER)() const>
        struct PropertyAccessor<
          GETTER_RETURN_TYPE (CLASS::*)() const,
          std::nullptr_t,
          GETTER,
          nullptr>
        {
            using owner_class   = CLASS;
            using property_type = std::decay_t<GETTER_RETURN_TYPE>;

            static void
              getter_impl(property const &, const void * i_source_object, void * o_value_dest)
            {
                EDIACARAN_ASSERT(i_source_object != nullptr);
                EDIACARAN_ASSERT(o_value_dest != nullptr);
                auto const object = static_cast<const CLASS *>(i_source_object);
                new (o_value_dest) property_type((object->*GETTER)());
            }

            constexpr static property::getter getter() { return &getter_impl; }
            constexpr static property::setter setter() { return nullptr; }
        };

    } //namespace detail

    template <
      typename CLASS,
      typename BASE_CLASSES_LIST = type_list<>,
      size_t CLASS_NAME_SIZE,
      size_t PROPERTY_COUNT = 0,
      size_t FUNCTION_COUNT = 0>
    constexpr auto make_class(
      const char (&i_name)[CLASS_NAME_SIZE],
      array<property, PROPERTY_COUNT> const & i_properties = array<property, 0>{},
      array<function, FUNCTION_COUNT> const & i_functions  = array<function, 0>{})
    {
        static_assert(is_type_list_v<BASE_CLASSES_LIST>);

        constexpr bool is_container = detail::HasStdContainerInterface<CLASS>::value;

        if constexpr (detail::TemplateAutoDeducer<CLASS>::arguments_count == 0)
        {
            array<char, CLASS_NAME_SIZE> name{};
            to_chars(name.data(), CLASS_NAME_SIZE, i_name);
            return detail::StaticClass<
              CLASS,
              CLASS_NAME_SIZE,
              template_arguments<>,
              PROPERTY_COUNT,
              FUNCTION_COUNT,
              BASE_CLASSES_LIST,
              is_container>(name, make_template_arguments(), i_properties, i_functions);
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
              FUNCTION_COUNT,
              BASE_CLASSES_LIST,
              is_container>(name, auto_template_arguments, i_properties, i_functions);
        }
    }

    template <
      typename CLASS,
      size_t ARGUMENTS_STRING_SIZE,
      typename BASE_CLASSES_LIST = type_list<>,
      size_t TEMPLATE_NAME_SIZE,
      typename... TEMPLATE_PARAMETERS,
      size_t PROPERTY_COUNT = 0,
      size_t FUNCTION_COUNT = 0>
    constexpr auto make_class(
      const char (&i_template_name)[TEMPLATE_NAME_SIZE],
      const template_arguments<TEMPLATE_PARAMETERS...> & i_template_arguments,
      array<property, PROPERTY_COUNT> const &            i_properties = array<property, 0>{},
      array<function, FUNCTION_COUNT> const &            i_functions  = array<function, 0>{})
    {
        static_assert(is_type_list_v<BASE_CLASSES_LIST>);

        constexpr bool is_container = detail::HasStdContainerInterface<CLASS>::value;

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
          FUNCTION_COUNT,
          BASE_CLASSES_LIST,
          is_container>(specialization_name, i_template_arguments, i_properties, i_functions);
    }

    template <typename PROP_TYPE, size_t OFFSET>
    constexpr property make_property(const char * i_name)
    {
        return property(i_name, get_qualified_type<PROP_TYPE>(), OFFSET);
    }

    template <auto GETTER, auto SETTER = nullptr>
    constexpr std::
      enable_if_t<std::is_member_function_pointer_v<std::decay_t<decltype(GETTER)>>, property>
      make_property(const char * i_name)
    {
        using GETTER_TYPE = std::decay_t<decltype(GETTER)>;
        using SETTER_TYPE = std::decay_t<decltype(SETTER)>;

        using accessor = detail::PropertyAccessor<
          edi::remove_noexcept_t<GETTER_TYPE>,
          edi::remove_noexcept_t<SETTER_TYPE>,
          GETTER,
          SETTER>;

        using property_type = std::conditional_t<
          SETTER != nullptr,
          typename accessor::property_type,
          std::add_const_t<typename accessor::property_type>>;

        return property(
          i_name,
          get_qualified_type<property_type>(),
          i_name,
          accessor::getter(),
          accessor::setter());
    }

} // namespace edi
