

#include "../common.h"
#include "ediacaran/reflection/reflection.h"

namespace ediacaran_test
{
    template <typename B> struct Fir1
    {
    };

    template <typename A, typename B> struct Fir2
    {
    };

    template <typename A, int, typename C> struct Fir3
    {
    };

    template <typename A, typename B, typename C, typename D> struct Fir4
    {
    };

    template <typename T_1> constexpr auto reflect(Fir1<T_1> ** i_ptr)
    {
        char const class_name[] = "Fir1";
        using namespace ediacaran;
        using bases      = type_list<>;
        using this_class = std::remove_reference_t<decltype(**i_ptr)>;

        constexpr auto template_arguments =
          make_template_arguments("T_1", get_qualified_type<T_1>());

        return make_static_cast<this_class, char_array_size(template_arguments)>(
          class_name, template_arguments);
    }

    template <typename T_1, typename T_2> constexpr auto reflect(Fir2<T_1, T_2> ** i_ptr)
    {
        using namespace ediacaran;
        char const class_name[] = "Fir2";
        using bases             = type_list<>;
        using this_class        = std::remove_reference_t<decltype(**i_ptr)>;

        constexpr auto template_arguments =
          make_template_arguments("T_1, T_2", get_qualified_type<T_1>(), get_qualified_type<T_2>());

        return make_static_cast<this_class, char_array_size(template_arguments)>(
          class_name, template_arguments);
    }

    template <typename T_1, int T_2, typename T_3>
    constexpr auto reflect(Fir3<T_1, T_2, T_3> ** i_ptr)
    {
        using namespace ediacaran;
        char const class_name[] = "Fir3";
        using bases             = type_list<>;
        using this_class        = std::remove_reference_t<decltype(**i_ptr)>;

        constexpr auto const template_arguments = make_template_arguments(
          "T_1, T_2, T_3",
          get_qualified_type<T_1>(),
          T_2,
          get_qualified_type<T_3>());

        return make_static_cast<this_class, char_array_size(template_arguments)>(
          class_name, template_arguments);
    }

    template <typename A, typename B, typename C, typename D>
    constexpr auto reflect(Fir4<A, B, C, D> ** i_ptr)
    {
        using namespace ediacaran;
        char const class_name[] = "Fir4";
        using bases = type_list<>;
        using this_class = std::remove_reference_t<decltype(**i_ptr)>;

        return make_static_cast<this_class>(class_name);
    }

    void class_templates_tests()
    {
        using namespace ediacaran;

        constexpr auto & temp_1 = get_type<Fir1<int>>();
        constexpr auto & temp_2 = get_type<Fir2<int, double>>();
        constexpr auto & temp_3 = get_type<Fir3<float, 5, char>>();
        static_assert(temp_1.template_arguments().size() == 1);
        static_assert(temp_2.template_arguments().size() == 2);
        static_assert(temp_3.template_arguments().size() == 3);
        static_assert(temp_1.name() == "Fir1<int32>");
        static_assert(temp_2.name() == "Fir2<int32, double>");
        static_assert(temp_3.name() == "Fir3<float, 5, char>");

        static_assert(detail::TemplateArguments<Fir1<int>>::size == 1);
        static_assert(detail::TemplateArguments<Fir2<int, double>>::size == 2);
        static_assert(detail::TemplateArguments<Fir3<int, 4, char>>::size == 0);
    }
}
