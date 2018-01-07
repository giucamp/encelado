

#include "../common.h"
#include "ediacaran/core/string_builder.h"
#include "ediacaran/reflection/reflection.h"
#include "ediacaran/utils/dyn_value.h"
#include "ediacaran/utils/inspect.h"
#include <iostream>
#include <new>
#include <string>
#include <array>

namespace ediacaran_test
{
    // layer 3

    struct TestBase_3_1
    {
        virtual void f() {}
    };

    constexpr auto reflect(TestBase_3_1 * * i_ptr)
    {
        auto const class_name = "TestBase_3_1";

        using namespace ediacaran;
        using this_class = std::remove_reference_t<decltype(**i_ptr)>;

        return make_static_cast<this_class>(class_name);
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

    constexpr auto reflect(TestBase_3_2** i_ptr)
    {
        auto const class_name = "TestBase_3_2";
        using bases = ediacaran::type_list<>;
        using this_class = std::remove_reference_t<decltype(**i_ptr)>;

        auto const properties = ediacaran::make_array(
            REFL_DATA_PROP("m_float_3_2_1", m_float_3_2_1),
            REFL_ACCESSOR_PROP("prop", get_prop, set_prop),
            REFL_ACCESSOR_RO_PROP("readonly_prop", get_readonly_prop)
        );
        return ediacaran::make_static_cast<this_class>(class_name, bases{}, properties);
    }

    // layer 2

    struct TestBase_2_1
    {
        char m_char_2_1_1{};
    };

    constexpr auto reflect(TestBase_2_1** i_ptr)
    {
        auto const class_name = "TestBase_2_1";
        using bases = ediacaran::type_list<>;
        using this_class = std::remove_reference_t<decltype(**i_ptr)>;

        auto const properties = ediacaran::make_array(
            REFL_DATA_PROP("m_char_2_1_1", m_char_2_1_1)
        );
        return ediacaran::make_static_cast<this_class>(class_name, bases{}, properties);
    }

    struct TestBase_2_2 : TestBase_3_1
    {
        int m_int2_2_1{};
        int m_int2_2_2{};
    };

    constexpr auto reflect(TestBase_2_2** i_ptr)
    {
        auto const class_name = "TestBase_2_2";
        using bases = ediacaran::type_list<TestBase_3_1>;
        using this_class = std::remove_reference_t<decltype(**i_ptr)>;

        auto const properties = ediacaran::make_array(
            REFL_DATA_PROP("m_int2_2_1", m_int2_2_1),
            REFL_DATA_PROP("m_int2_2_2", m_int2_2_2)
        );
        return ediacaran::make_static_cast<this_class>(class_name, bases{}, properties);
    }

    struct TestBase_2_3 : TestBase_3_2
    {
        float m_float_2_3_1{};
    };

    constexpr auto reflect(TestBase_2_3** i_ptr)
    {
        auto const class_name = "TestBase_2_3";
        using bases = ediacaran::type_list<TestBase_3_2>;
        using this_class = std::remove_reference_t<decltype(**i_ptr)>;

        auto const properties = ediacaran::make_array(
            REFL_DATA_PROP("m_float_2_3_1", m_float_2_3_1)
        );
        return ediacaran::make_static_cast<this_class>(class_name, bases{}, properties);
    }

    struct TestBase_2_4
    {
        double m_double_2_4_1{};
        double m_double_2_4_2{};
    };

    constexpr auto reflect(TestBase_2_4** i_ptr)
    {
        auto const class_name = "TestBase_2_4";
        using bases = ediacaran::type_list<>;
        using this_class = std::remove_reference_t<decltype(**i_ptr)>;

        auto const properties = ediacaran::make_array(
            REFL_DATA_PROP("m_double_2_4_1", m_double_2_4_1),
            REFL_DATA_PROP("m_double_2_4_2", m_double_2_4_2)
        );
        return ediacaran::make_static_cast<this_class>(class_name, bases{}, properties);
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

    constexpr auto reflect(TestBase_2_Base** i_ptr)
    {
        auto const class_name = "TestBase_2_Base";
        using bases = ediacaran::type_list<>;
        using this_class = std::remove_reference_t<decltype(**i_ptr)>;

        auto const properties = ediacaran::make_array(
            REFL_DATA_PROP("m_int8_2_1", m_int8_2_1),
            REFL_DATA_PROP("m_int8_2_2", m_int8_2_2),
            REFL_DATA_PROP("m_int8_2_3", m_int8_2_3)
        );
        auto const actions = ediacaran::make_array(
            REFL_ACTION("set_all", set_all, "i_value")
            //REFL_ACTION("clear_all", clear_all, "")
        );
        return ediacaran::make_static_cast<this_class>(class_name, bases{}, properties, actions);
    }

    // layer 1

    struct TestBase_1_1 : TestBase_2_1, TestBase_2_2, virtual TestBase_2_Base
    {
        const char * m_string_1 = "abc";
    };

    constexpr auto reflect(TestBase_1_1** i_ptr)
    {
        auto const class_name = "TestBase_1_1";
        using bases = ediacaran::type_list<TestBase_2_1, TestBase_2_2, TestBase_2_Base>;
        using this_class = std::remove_reference_t<decltype(**i_ptr)>;

        auto const properties = ediacaran::make_array(
            REFL_DATA_PROP("m_string_1", m_string_1)
        );
        return ediacaran::make_static_cast<this_class>(class_name, bases{}, properties);
    }

    struct TestBase_1_2 : TestBase_2_3, TestBase_2_4, virtual TestBase_2_Base
    {
        const char * m_string_2_1 = nullptr;
        const char * m_string_2_2 = "efg";
        const char * m_string_2_3 = nullptr;
    };

    constexpr auto reflect(TestBase_1_2** i_ptr)
    {
        auto const class_name = "TestBase_1_2";
        using bases = ediacaran::type_list<TestBase_2_3, TestBase_2_4, TestBase_2_Base>;
        using this_class = std::remove_reference_t<decltype(**i_ptr)>;

        auto const properties = ediacaran::make_array(
            REFL_DATA_PROP("m_string_2_1", m_string_2_1),
            REFL_DATA_PROP("m_string_2_2", m_string_2_2),
            REFL_DATA_PROP("m_string_2_3", m_string_2_3)
        );
        return ediacaran::make_static_cast<this_class>(class_name, bases{}, properties);
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

    constexpr auto reflect(TestClass** i_ptr)
    {
        auto const class_name = "TestClass";
        using bases = ediacaran::type_list<TestBase_1_1, TestBase_1_2>;
        using this_class = std::remove_reference_t<decltype(**i_ptr)>;

        auto const properties = ediacaran::make_array(
            REFL_DATA_PROP("integer", m_integer),
            REFL_DATA_PROP("float", m_float)
        );
        auto const actions = ediacaran::make_array(
            REFL_ACTION("add", add, "i_int_par, i_flt_par, i_invert"),
            REFL_ACTION("set", set, "i_int_par, i_flt_par")
        );
        return ediacaran::make_static_cast<this_class>(class_name, bases{}, properties, actions);
    }

    void class_tests_print_props(ediacaran::raw_ptr i_source)
    {
        using namespace ediacaran;

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

    template <typename A, typename B, typename C> struct Fir3
    {
    };

    template <typename A, typename B> struct Fir2
    {
    };

    template <typename B> struct Fir1
    {
    };

    void class_tests()
    {
        static_assert(ediacaran::detail::TemplateParameters<Fir1<int>>::size == 1);
        static_assert(ediacaran::detail::TemplateParameters<Fir2<int, double>>::size == 2);
        static_assert(ediacaran::detail::TemplateParameters<Fir3<int, double, char>>::size == 3);
        using namespace ediacaran;
        get_type<TestClass>();
        try
        {
            property   props[2] = {property(property::offset_tag{}, "prop", get_qualified_type<int>(), 2),
                                 property(property::offset_tag{}, "prop", get_qualified_type<int>(), 2)};
            class_type CC(
              "abc", 1, 2, special_functions{}, array_view<const base_class>{}, props, array_view<const action>{});

            ENCELADO_TEST_ASSERT(false); // should have thrown
        }
        catch (std::exception i_exc)
        {
            std::cout << "expected error: " << i_exc.what() << std::endl;
        }

        TestClass test_object;
        for (auto const & prop : inspect_properties(raw_ptr(&test_object)))
        {
            std::cout << prop.owning_class().name().data() << " -> " << prop.property().name().data() << std::endl;
        }

        class_tests_print_props(raw_ptr(&test_object));

        const auto & t = get_class<TestClass>();
        for (auto const & act : t.actions())
        {
            std::cout << act.name().data() << std::endl;

            for (auto const & par : act.parameters())
            {
                std::cout << "\t" << par.name().data() << std::endl;
            }
        }
        std::cout << "------------------" << std::endl;

        raw_ptr obj(&test_object);
        {
            auto int_v = get_property_value(obj, "integer").to_string();
            auto flt_v = get_property_value(obj, "float").to_string();
            ENCELADO_TEST_ASSERT(int_v == "4");
            ENCELADO_TEST_ASSERT(flt_v == "5");
            auto action_res = invoke_action(obj, "add(2, 3, false)").to_string();
            auto int_v_1    = get_property_value(obj, "integer").to_string();
            auto flt_v_2    = get_property_value(obj, "float").to_string();
            ENCELADO_TEST_ASSERT(action_res == "6");
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
            invoke_action(obj, "set_all(11)");
            auto n1 = get_property_value(obj, "m_int8_2_1").to_string();
            auto n2 = get_property_value(obj, "m_int8_2_2").to_string();
            auto n3 = get_property_value(obj, "m_int8_2_3").to_string();
            //ENCELADO_TEST_ASSERT(action_res == "1");
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
    }
}
