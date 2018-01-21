

#include "../common.h"
#include "ediacaran/core/string_builder.h"
#include "ediacaran/reflection/reflection.h"
#include "ediacaran/utils/raw_ptr.h"
#include <iostream>

namespace ediacaran_test
{
    template <typename B> struct ClassTemplate1
    {
    };

    template <typename A> constexpr auto reflect(ClassTemplate1<A> ** i_ptr)
    {
        char const class_name[] = "ClassTemplate1";
        using namespace ediacaran;
        using this_class = std::remove_reference_t<decltype(**i_ptr)>;

        constexpr auto template_arguments = make_template_arguments("A", get_qualified_type<A>());

        return make_static_cast<this_class, char_array_size(template_arguments)>(
          class_name, template_arguments);
    }

    template <typename A, typename B> struct ClassTemplate2
    {
    };

    template <typename A, typename B> constexpr auto reflect(ClassTemplate2<A, B> ** i_ptr)
    {
        using namespace ediacaran;
        char const class_name[] = "ClassTemplate2";
        using this_class        = std::remove_reference_t<decltype(**i_ptr)>;

        constexpr auto template_arguments =
          make_template_arguments("A, B", get_qualified_type<A>(), get_qualified_type<B>());

        return make_static_cast<this_class, char_array_size(template_arguments)>(
          class_name, template_arguments);
    }

    template <typename A, int, typename C>
    struct ClassTemplate3 : ClassTemplate1<A>, virtual ClassTemplate2<int, C>
    {
    };

    template <typename A, int B, typename C>
    constexpr auto reflect(ClassTemplate3<A, B, C> ** i_ptr)
    {
        using namespace ediacaran;
        char const class_name[] = "ClassTemplate3";
        using bases             = type_list<ClassTemplate1<A>, ClassTemplate2<int, C>>;
        using this_class        = std::remove_reference_t<decltype(**i_ptr)>;

        constexpr auto const template_arguments =
          make_template_arguments("A, B, C", get_qualified_type<A>(), B, get_qualified_type<C>());

        return make_static_cast<this_class, char_array_size(template_arguments)>(
          class_name, template_arguments, bases());
    }

    template <typename A, typename B, typename C, typename D>
    struct ClassTemplate4 : ClassTemplate3<A, 12, char>, virtual ClassTemplate2<int, int>
    {
    };

    template <typename A, typename B, typename C, typename D>
    constexpr auto reflect(ClassTemplate4<A, B, C, D> ** i_ptr)
    {
        using namespace ediacaran;
        char const class_name[] = "ClassTemplate4";
        using bases             = type_list<ClassTemplate3<A, 12, char>, ClassTemplate2<int, int>>;
        using this_class        = std::remove_reference_t<decltype(**i_ptr)>;

        return make_static_cast<this_class>(class_name, bases());
    }

    template <typename A, typename B, typename C, typename D, typename E>
    struct ClassTemplate5 : ClassTemplate4<double, double, double, double>
    {
    };

    template <typename... PARAMS> constexpr auto reflect(ClassTemplate5<PARAMS...> ** i_ptr)
    {
        using namespace ediacaran;
        char const class_name[] = "ClassTemplate5";
        using bases             = type_list<ClassTemplate4<double, double, double, double>>;
        using this_class        = std::remove_reference_t<decltype(**i_ptr)>;
        return make_static_cast<this_class>(class_name, bases());
    }

    std::string
      class_template_specialization_descr(const ediacaran::class_template_specialization & i_spec)
    {
        using namespace ediacaran;
        string_builder out;

        size_t const templ_par_count = i_spec.template_parameters().size();
        auto         names           = i_spec.template_parameter_names().begin();
        for (size_t index = 0; index < templ_par_count; index++, names++)
        {
            auto const &       par = i_spec.template_parameters()[index];
            void const * const arg = i_spec.template_arguments()[index];
            out << *names << ": " << par.qualified_type() << " = ";
            out << raw_ptr(const_cast<void *>(arg), par.qualified_type());

            if (index + 1 < templ_par_count)
                out << ", ";
        }
        return out.to_string();
    }

    void class_templates_tests()
    {
        using namespace ediacaran;

        constexpr auto & temp_1 = get_type<ClassTemplate1<int32_t>>();
        constexpr auto & temp_2 = get_type<ClassTemplate2<int32_t, double>>();
        constexpr auto & temp_3 = get_type<ClassTemplate3<float, 5, char>>();
        constexpr auto & temp_4 = get_type<ClassTemplate4<float, char, double, int64_t>>();
        constexpr auto & temp_5 = get_type<ClassTemplate5<
          float const * const *,
          symbol,
          ClassTemplate1<ClassTemplate1<ClassTemplate3<float, 25, int32_t>>>,
          double * volatile ***,
          int * const *>>();
        static_assert(temp_1.template_arguments().size() == 1);
        static_assert(temp_2.template_arguments().size() == 2);
        static_assert(temp_3.template_arguments().size() == 3);
        static_assert(temp_4.template_arguments().size() == 4);
        static_assert(temp_5.template_arguments().size() == 5);
        static_assert(temp_1.name() == "ClassTemplate1<int32>");
        static_assert(temp_2.name() == "ClassTemplate2<int32, double>");
        static_assert(temp_3.name() == "ClassTemplate3<float, 5, char>");
        static_assert(temp_4.name() == "ClassTemplate4<float, char, double, int64>");
        static_assert(
          temp_5.name() == "ClassTemplate5<float const * const *, ediacaran::symbol, "
                           "ClassTemplate1<ClassTemplate1<ClassTemplate3<float, 25, int32>>>, "
                           "double * volatile * * *, int32 * const *>");

        auto descr_1 = class_template_specialization_descr(temp_1);
        auto descr_2 = class_template_specialization_descr(temp_2);
        auto descr_3 = class_template_specialization_descr(temp_3);
        auto descr_4 = class_template_specialization_descr(temp_4);
        auto descr_5 = class_template_specialization_descr(temp_5);

        ENCELADO_TEST_ASSERT(descr_1 == "A: ediacaran::qualified_type_ptr const = int32");
        ENCELADO_TEST_ASSERT(
          descr_2 == "A: ediacaran::qualified_type_ptr const = int32, "
                     "B: ediacaran::qualified_type_ptr const = double");
        ENCELADO_TEST_ASSERT(
          descr_3 == "A: ediacaran::qualified_type_ptr const = float, "
                     "B: int32 const = 5, "
                     "C: ediacaran::qualified_type_ptr const = char");
        ENCELADO_TEST_ASSERT(
          descr_4 == "a0: ediacaran::qualified_type_ptr const = float, "
                     "a1: ediacaran::qualified_type_ptr const = char, "
                     "a2: ediacaran::qualified_type_ptr const = double, "
                     "a3: ediacaran::qualified_type_ptr const = int64");
        ENCELADO_TEST_ASSERT(
          descr_5 == "a0: ediacaran::qualified_type_ptr const = float const * const *, "
                     "a1: ediacaran::qualified_type_ptr const = ediacaran::symbol, "
                     "a2: ediacaran::qualified_type_ptr const = "
                     "ClassTemplate1<ClassTemplate1<ClassTemplate3<float, 25, int32>>>, "
                     "a3: ediacaran::qualified_type_ptr const = double * volatile * * *, "
                     "a4: ediacaran::qualified_type_ptr const = int32 * const *");
    }
}
