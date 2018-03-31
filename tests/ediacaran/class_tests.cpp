
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../common.h"
#include "ediacaran/core/string_builder.h"
#include "ediacaran/reflection/reflection.h"
#include "ediacaran/utils/dyn_value.h"
#include "ediacaran/utils/inspect.h"
#include <algorithm>
#include <ediacaran/core/array.h>
#include <iostream>
#include <new>
#include <string>

namespace ediacaran_test
{
    enum class Enum_1
    {
        Member_1 = 1,
        Member_2 = 2,
        Member_3 = 4
    };

    constexpr auto reflect(Enum_1 ** i_ptr)
    {
        using this_enum = std::remove_reference_t<decltype(**i_ptr)>;
        using namespace edi;
        return make_enum<this_enum>(
          "Enum_1",
          make_array(
            make_enum_member("Member_1", this_enum::Member_1),
            make_enum_member("Member_2", this_enum::Member_2),
            make_enum_member("Member_3", this_enum::Member_3)));
    }

    // layer 3

    struct TestBase_3_1
    {
        virtual void f() {}
    };

    constexpr auto reflect(TestBase_3_1 ** i_ptr)
    {
        char const class_name[] = "TestBase_3_1";

        using namespace edi;
        using this_class = std::remove_reference_t<decltype(**i_ptr)>;

        return make_class<this_class>(class_name);
    }

    struct TestBase_3_2
    {
        virtual void g() {}
        float        m_float_3_2_1{};

        int m_prop = 5;

        int get_prop() const { return m_prop; }

        void set_prop(int i_value) { m_prop = i_value; }

        double get_readonly_prop() const { return 1.23; }
    };

    constexpr auto reflect(TestBase_3_2 ** i_ptr)
    {
        using namespace edi;
        char const class_name[] = "TestBase_3_2";
        using bases             = type_list<>;
        using this_class        = std::remove_reference_t<decltype(**i_ptr)>;

        auto const properties = make_array(
          REFL_DATA_PROP("m_float_3_2_1", m_float_3_2_1),
          REFL_ACCESSOR_PROP("prop", get_prop, set_prop),
          REFL_ACCESSOR_RO_PROP("readonly_prop", get_readonly_prop));
        return make_class<this_class, bases>(class_name, properties);
    }

    // layer 2

    struct TestBase_2_1
    {
        char m_char_2_1_1{};
    };

    constexpr auto reflect(TestBase_2_1 ** i_ptr)
    {
        char const class_name[] = "TestBase_2_1";
        using bases             = edi::type_list<>;
        using this_class        = std::remove_reference_t<decltype(**i_ptr)>;

        auto const properties = edi::make_array(REFL_DATA_PROP("m_char_2_1_1", m_char_2_1_1));
        return edi::make_class<this_class, bases>(class_name, properties);
    }

    struct TestBase_2_2 : TestBase_3_1
    {
        int m_int2_2_1{};
        int m_int2_2_2{};
    };

    constexpr auto reflect(TestBase_2_2 ** i_ptr)
    {
        char const class_name[] = "TestBase_2_2";
        using bases             = edi::type_list<TestBase_3_1>;
        using this_class        = std::remove_reference_t<decltype(**i_ptr)>;

        auto const properties = edi::make_array(
          REFL_DATA_PROP("m_int2_2_1", m_int2_2_1), REFL_DATA_PROP("m_int2_2_2", m_int2_2_2));
        return edi::make_class<this_class, bases>(class_name, properties);
    }

    struct TestBase_2_3 : TestBase_3_2
    {
        float m_float_2_3_1{};
    };

    constexpr auto reflect(TestBase_2_3 ** i_ptr)
    {
        char const class_name[] = "TestBase_2_3";
        using bases             = edi::type_list<TestBase_3_2>;
        using this_class        = std::remove_reference_t<decltype(**i_ptr)>;

        auto const properties = edi::make_array(REFL_DATA_PROP("m_float_2_3_1", m_float_2_3_1));
        return edi::make_class<this_class, bases>(class_name, properties);
    }

    struct TestBase_2_4
    {
        double m_double_2_4_1{};
        double m_double_2_4_2{};

        void           f_1() {}
        double const * f_2() { return nullptr; }
        void           f_3() noexcept {}
        double const * f_4() noexcept { return nullptr; }

        void           f_5() const {}
        double const * f_6() const { return nullptr; }
        void           f_7() const noexcept {}
        double const * f_8() const noexcept { return nullptr; }

        void           f_9() const volatile {}
        double const * f_10() const volatile { return nullptr; }
        void           f_11() const volatile noexcept {}
        double const * f_12() const volatile noexcept { return nullptr; }

        void           f_13() volatile {}
        double const * f_14() volatile { return nullptr; }
        void           f_15() volatile noexcept {}
        double const * f_16() volatile noexcept { return nullptr; }
    };

    constexpr auto reflect(TestBase_2_4 ** i_ptr)
    {
        using namespace edi;
        char const class_name[] = "TestBase_2_4";
        using this_class        = std::remove_reference_t<decltype(**i_ptr)>;

        auto const properties = make_array(
          REFL_DATA_PROP("m_double_2_4_1", m_double_2_4_1),
          REFL_DATA_PROP("m_double_2_4_2", m_double_2_4_2));

        auto const functions = make_array(
          make_function<EDI_FUNC(f_1)>("f_1"),
          make_function<EDI_FUNC(f_2)>("f_2"),
          make_function<EDI_FUNC(f_3)>("f_3"),
          make_function<EDI_FUNC(f_4)>("f_4"),
          make_function<EDI_FUNC(f_5)>("f_5"),
          make_function<EDI_FUNC(f_6)>("f_6"),
          make_function<EDI_FUNC(f_7)>("f_7"),
          make_function<EDI_FUNC(f_8)>("f_8"),
          make_function<EDI_FUNC(f_9)>("f_9"),
          make_function<EDI_FUNC(f_10)>("f_10"),
          make_function<EDI_FUNC(f_11)>("f_11"),
          make_function<EDI_FUNC(f_12)>("f_12"),
          make_function<EDI_FUNC(f_13)>("f_13"),
          make_function<EDI_FUNC(f_14)>("f_14"),
          make_function<EDI_FUNC(f_15)>("f_15"),
          make_function<EDI_FUNC(f_16)>("f_16"));

        return make_class<this_class>(class_name, properties, functions);
    }

    struct TestBase_2_Base
    {
        int8_t m_int8_2_1{1};
        int8_t m_int8_2_2{2};
        int8_t m_int8_2_3{3};

        void set_all(int8_t i_value)
        {
            m_int8_2_1 = i_value;
            m_int8_2_2 = i_value + 1;
            m_int8_2_3 = i_value + 2;
        }

        int8_t clear_all()
        {
            auto res   = m_int8_2_1 + m_int8_2_2 + m_int8_2_3;
            m_int8_2_1 = 0;
            m_int8_2_2 = 0;
            m_int8_2_3 = 0;
            return res;
        }
    };

    constexpr auto reflect(TestBase_2_Base ** i_ptr)
    {
        char const class_name[] = "TestBase_2_Base";
        using bases             = edi::type_list<>;
        using this_class        = std::remove_reference_t<decltype(**i_ptr)>;

        auto const properties = edi::make_array(
          REFL_DATA_PROP("m_int8_2_1", m_int8_2_1),
          REFL_DATA_PROP("m_int8_2_2", m_int8_2_2),
          REFL_DATA_PROP("m_int8_2_3", m_int8_2_3));
        auto const functions = edi::make_array(
          REFL_FUNCTION("set_all", set_all, "i_value"), REFL_FUNCTION("clear_all", clear_all, ""));
        return edi::make_class<this_class, bases>(class_name, properties, functions);
    }

    // layer 1

    struct TestBase_1_1 : TestBase_2_1, TestBase_2_2, virtual TestBase_2_Base
    {
        const char * m_string_1 = "abc";
    };

    constexpr auto reflect(TestBase_1_1 ** i_ptr)
    {
        char const class_name[] = "TestBase_1_1";
        using bases             = edi::type_list<TestBase_2_1, TestBase_2_2, TestBase_2_Base>;
        using this_class        = std::remove_reference_t<decltype(**i_ptr)>;

        auto const properties = edi::make_array(REFL_DATA_PROP("m_string_1", m_string_1));
        return edi::make_class<this_class, bases>(class_name, properties);
    }

    struct TestBase_1_2 : TestBase_2_3, TestBase_2_4, virtual TestBase_2_Base
    {
        const char * m_string_2_1 = nullptr;
        const char * m_string_2_2 = "efg";
        const char * m_string_2_3 = nullptr;
    };

    constexpr auto reflect(TestBase_1_2 ** i_ptr)
    {
        char const class_name[] = "TestBase_1_2";
        using bases             = edi::type_list<TestBase_2_3, TestBase_2_4, TestBase_2_Base>;
        using this_class        = std::remove_reference_t<decltype(**i_ptr)>;

        auto const properties = edi::make_array(
          REFL_DATA_PROP("m_string_2_1", m_string_2_1),
          REFL_DATA_PROP("m_string_2_2", m_string_2_2),
          REFL_DATA_PROP("m_string_2_3", m_string_2_3));
        return edi::make_class<this_class, bases>(class_name, properties);
    }

    // layer 0

    struct TestClass : TestBase_1_1, TestBase_1_2
    {
        int   m_integer = 4;
        float m_float   = 5;

        int add(int i_int_par, const float i_flt_par, bool i_invert)
        {
            if (!i_invert)
            {
                m_integer += i_int_par;
                m_float += i_flt_par;
            }
            else
            {
                m_integer -= i_int_par;
                m_float -= i_flt_par;
            }
            return m_integer;
        }

        int set(int i_int_par, const float i_flt_par)
        {
            m_integer = i_int_par;
            m_float   = i_flt_par;
            return m_integer;
        }
    };

    constexpr auto reflect(TestClass ** i_ptr)
    {
        using namespace edi;
        constexpr char class_name[] = "TestClass";
        using bases                 = type_list<TestBase_1_1, TestBase_1_2>;
        using this_class            = std::remove_reference_t<decltype(**i_ptr)>;

        auto const properties = make_array(
          make_property<EDI_DATA(m_integer)>("integer"), make_property<EDI_DATA(m_float)>("float"));

        auto const functions = make_array(
          REFL_FUNCTION("add", add, "i_int_par, i_flt_par, i_invert"),
          REFL_FUNCTION("set", set, "i_int_par, i_flt_par"));

        return make_class<this_class, bases>(class_name, properties, functions);
    }

    void class_tests_print_props(edi::raw_ptr i_source)
    {
        using namespace edi;

        std::string string;
        for (auto const & prop : inspect_properties(i_source))
        {
            prop.get_value().to_string(string);

            std::string str = prop.owning_class().name();
            str += " -> ";
            str += prop.name();
            str += ": ";
            str += to_string(prop.qualified_type());
            str += " = ";
            str += string;
            std::cout << str << std::endl;
        }
    }

    void class_tests()
    {
        using namespace edi;

        auto const & te = get_type<Enum_1>();

        char dest[40];
        to_chars(dest, Enum_1(1 | 2 | 8));

        try
        {
            property props[2] = {
              property(property::offset_tag{}, "prop", get_qualified_type<int>(), 2),
              property(property::offset_tag{}, "prop", get_qualified_type<int>(), 2)};
            class_type CC(
              "abc",
              1,
              2,
              special_functions{},
              array_view<const base_class>{},
              props,
              array_view<const function>{},
              nullptr);

            ENCELADO_TEST_ASSERT(false); // should have thrown
        }
        catch (std::exception i_exc)
        {
            std::cout << "expected error: " << i_exc.what() << std::endl;
        }

        TestClass test_object;
        for (auto const & prop : inspect_properties(raw_ptr(&test_object)))
        {
            std::cout << prop.owning_class().name().data() << " -> "
                      << prop.property().name().data() << std::endl;
        }

        class_tests_print_props(raw_ptr(&test_object));

        const auto & t = get_type<TestClass>();
        for (auto const & act : t.functions())
        {
            std::cout << act.name().data() << std::endl;

            for (auto const & par : act.parameter_names())
            {
                std::string name(par.data(), par.size());
                std::cout << "\t" << name << std::endl;
            }
        }
        std::cout << "------------------" << std::endl;

        raw_ptr obj(&test_object);
        {
            auto int_v = get_property_value(obj, "integer").to_string();
            auto flt_v = get_property_value(obj, "float").to_string();
            ENCELADO_TEST_ASSERT(int_v == "4");
            ENCELADO_TEST_ASSERT(flt_v == "5");
            auto function_res = invoke_function(obj, "add(2, 3, false)").to_string();
            auto int_v_1      = get_property_value(obj, "integer").to_string();
            auto flt_v_2      = get_property_value(obj, "float").to_string();
            ENCELADO_TEST_ASSERT(function_res == "6");
            ENCELADO_TEST_ASSERT(int_v_1 == "6");
            ENCELADO_TEST_ASSERT(flt_v_2 == "8");
        }
        {
            auto v1 = get_property_value(obj, "m_int8_2_1").to_string();
            auto v2 = get_property_value(obj, "m_int8_2_2").to_string();
            auto v3 = get_property_value(obj, "m_int8_2_3").to_string();
            ENCELADO_TEST_ASSERT(v1 == "1");
            ENCELADO_TEST_ASSERT(v2 == "2");
            ENCELADO_TEST_ASSERT(v3 == "3");
            invoke_function(obj, "set_all(11)");
            auto n1 = get_property_value(obj, "m_int8_2_1").to_string();
            auto n2 = get_property_value(obj, "m_int8_2_2").to_string();
            auto n3 = get_property_value(obj, "m_int8_2_3").to_string();
            //ENCELADO_TEST_ASSERT(function_res == "1");
            ENCELADO_TEST_ASSERT(n1 == "11");
            ENCELADO_TEST_ASSERT(n2 == "12");
            ENCELADO_TEST_ASSERT(n3 == "13");

            int8_t      val = 34;
            char_reader prop_n("m_int8_2_1");
            char_reader prop_v("32");
            set_property_value(obj, prop_n, prop_v);
            set_property_value(obj, "m_int8_2_2", "33");
            set_property_value(obj, "m_int8_2_3", raw_ptr(&val));

            auto m1 = get_property_value(obj, "m_int8_2_1").to_string();
            auto m2 = get_property_value(obj, "m_int8_2_2").to_string();
            auto m3 = get_property_value(obj, "m_int8_2_3").to_string();
            ENCELADO_TEST_ASSERT(m1 == "32");
            ENCELADO_TEST_ASSERT(m2 == "33");
            ENCELADO_TEST_ASSERT(m3 == "34");
        }

        std::cout << "------------------" << std::endl;

        static_assert(
          find_named(get_type<TestBase_2_4>().functions(), "f_1")->qualification() ==
          cv_qualification::no_q);
        static_assert(
          find_named(get_type<TestBase_2_4>().functions(), "f_2")->qualification() ==
          cv_qualification::no_q);
        static_assert(
          find_named(get_type<TestBase_2_4>().functions(), "f_3")->qualification() ==
          cv_qualification::no_q);
        static_assert(
          find_named(get_type<TestBase_2_4>().functions(), "f_4")->qualification() ==
          cv_qualification::no_q);

        static_assert(
          find_named(get_type<TestBase_2_4>().functions(), "f_5")->qualification() ==
          cv_qualification::const_q);
        static_assert(
          find_named(get_type<TestBase_2_4>().functions(), "f_6")->qualification() ==
          cv_qualification::const_q);
        static_assert(
          find_named(get_type<TestBase_2_4>().functions(), "f_7")->qualification() ==
          cv_qualification::const_q);
        static_assert(
          find_named(get_type<TestBase_2_4>().functions(), "f_8")->qualification() ==
          cv_qualification::const_q);

        static_assert(
          find_named(get_type<TestBase_2_4>().functions(), "f_9")->qualification() ==
          (cv_qualification::const_q | cv_qualification::volatile_q));
        static_assert(
          find_named(get_type<TestBase_2_4>().functions(), "f_10")->qualification() ==
          (cv_qualification::const_q | cv_qualification::volatile_q));
        static_assert(
          find_named(get_type<TestBase_2_4>().functions(), "f_11")->qualification() ==
          (cv_qualification::const_q | cv_qualification::volatile_q));
        static_assert(
          find_named(get_type<TestBase_2_4>().functions(), "f_12")->qualification() ==
          (cv_qualification::const_q | cv_qualification::volatile_q));

        static_assert(
          find_named(get_type<TestBase_2_4>().functions(), "f_13")->qualification() ==
          cv_qualification::volatile_q);
        static_assert(
          find_named(get_type<TestBase_2_4>().functions(), "f_14")->qualification() ==
          cv_qualification::volatile_q);
        static_assert(
          find_named(get_type<TestBase_2_4>().functions(), "f_15")->qualification() ==
          cv_qualification::volatile_q);
        static_assert(
          find_named(get_type<TestBase_2_4>().functions(), "f_16")->qualification() ==
          cv_qualification::volatile_q);
    }
} // namespace ediacaran_test
