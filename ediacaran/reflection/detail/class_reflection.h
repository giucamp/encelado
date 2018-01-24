#pragma once

#define REFL_DATA_PROP(Name, DataMember)                                                           \
    ediacaran::make_property<decltype(this_class::DataMember), offsetof(this_class, DataMember)>(  \
      Name)

#define REFL_ACCESSOR_PROP(Name, Getter, Setter)                                                   \
    ediacaran::make_property<                                                                      \
      decltype(&this_class::Getter),                                                               \
      &this_class::Getter,                                                                         \
      decltype(&this_class::Setter),                                                               \
      &this_class::Setter>(Name)

#define REFL_ACCESSOR_RO_PROP(Name, Getter)                                                        \
    ediacaran::                                                                                    \
      make_property<decltype(&this_class::Getter), &this_class::Getter, std::nullptr_t, nullptr>(  \
        Name)

#define REFL_ACTION(Name, Method, ParameterNames)                                                  \
    ediacaran::detail::make_action<decltype(&this_class::Method), &this_class::Method>(            \
      Name, ParameterNames)

#define EDIACARAN_DATA(Class, DataMember)           decltype(Class::DataMember), offsetof(Class, DataMember)

#define EDIACARAN_CONST_ACCESSOR(Class, Getter)     decltype(&Class::Getter), &Class::Getter

#define EDIACARAN_ACCESSOR(Class, Getter, Setter)   decltype(&Class::Getter), &Class::Getter, decltype(&Class::Setter), &Class::Setter

namespace ediacaran
{
    namespace detail
    {
        // class

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
          typename BASE_CLASSES_LIST>
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
          type_list<BASE_CLASSES...>>
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
          type_list<BASE_CLASSES...>>
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
        struct PropertyAccessor<
          GETTER_RETURN_TYPE (CLASS::*)() const,
          std::nullptr_t,
          GETTER,
          nullptr>
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

    } //namespace detail

    template <
      typename CLASS,
      typename BASE_CLASSES_LIST = type_list<>,
      size_t CLASS_NAME_SIZE,
      size_t PROPERTY_COUNT = 0,
      size_t ACTION_COUNT   = 0>
    constexpr auto make_class(
      const char (&i_name)[CLASS_NAME_SIZE],
      array<property, PROPERTY_COUNT> const & i_properties = array<property, 0>{},
      array<action, ACTION_COUNT> const &     i_actions    = array<action, 0>{})
    {
        static_assert(is_type_list_v<BASE_CLASSES_LIST>);

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
              BASE_CLASSES_LIST>(name, make_template_arguments(), i_properties, i_actions);
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
              BASE_CLASSES_LIST>(name, auto_template_arguments, i_properties, i_actions);
        }
    }

    template <
      typename CLASS,
      size_t ARGUMENTS_STRING_SIZE,
      typename BASE_CLASSES_LIST = type_list<>,
      size_t TEMPLATE_NAME_SIZE,
      typename... TEMPLATE_PARAMETERS,
      size_t PROPERTY_COUNT = 0,
      size_t ACTION_COUNT   = 0>
    constexpr auto make_class(
      const char (&i_template_name)[TEMPLATE_NAME_SIZE],
      const template_arguments<TEMPLATE_PARAMETERS...> & i_template_arguments,
      array<property, PROPERTY_COUNT> const & i_properties = array<property, 0>{},
      array<action, ACTION_COUNT> const &     i_actions    = array<action, 0>{})
    {
        static_assert(is_type_list_v<BASE_CLASSES_LIST>);

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
          BASE_CLASSES_LIST>(specialization_name, i_template_arguments, i_properties, i_actions);
    }

    template <typename PROP_TYPE, size_t OFFSET>
    constexpr property make_property(const char * i_name)
    {
        return property(property::offset_tag{}, i_name, get_qualified_type<PROP_TYPE>(), OFFSET);
    }

    template <typename GETTER_TYPE, GETTER_TYPE GETTER, typename SETTER_TYPE, SETTER_TYPE SETTER>
    constexpr property make_property(const char * i_name)
    {
        using accessor = detail::PropertyAccessor<
          ediacaran::remove_noexcept_t<GETTER_TYPE>,
          ediacaran::remove_noexcept_t<SETTER_TYPE>,
          GETTER,
          SETTER>;

        return property(
          property::accessor_tag{},
          i_name,
          get_qualified_type<typename accessor::property_type>(),
          &accessor::func);
    }

} // namespace ediacaran