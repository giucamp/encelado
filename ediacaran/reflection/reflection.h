#pragma once

#include "ediacaran/reflection/class_type.h"
#include "ediacaran/reflection/class_template_specialization.h"

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
    ediacaran::detail::make_accessor_property<ediacaran::detail::PropertyAccessor<decltype(&this_class::Getter),       \
      decltype(&this_class::Setter), &this_class::Getter, &this_class::Setter>>(Name),

#define REFL_ACCESSOR_RO_PROP(Name, Getter)                                                                            \
    ediacaran::detail::make_accessor_property<                                                                         \
      ediacaran::detail::PropertyAccessor<decltype(&this_class::Getter), nullptr_t, &this_class::Getter, nullptr>>(    \
      Name),

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
