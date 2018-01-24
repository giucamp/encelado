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

namespace ediacaran
{
    namespace detail
    {
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

        template <typename PROPERTY_ACCESSOR>
        constexpr property make_accessor_property(const char * i_name)
        {
            return property(
              property::accessor_tag{},
              i_name,
              get_qualified_type<typename PROPERTY_ACCESSOR::property_type>(),
              &PROPERTY_ACCESSOR::func);
        }

    } //namespace detail

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