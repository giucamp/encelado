

#include "../common.h"
#include "ediacaran/core/string_builder.h"
#include "ediacaran/reflection/class_type.h"
#include "ediacaran/utils/dyn_value.h"
#include "ediacaran/utils/inspect.h"
#include <iostream>
#include <new>
#include <string>

namespace ediacaran_test
{
    // layer 3

    struct TestBase_3_1
    {
        virtual void f() {}
    };

    REFL_BEGIN_CLASS("TestBase_3_1", TestBase_3_1)
        REFL_BASES()
    REFL_END_CLASS;

    struct TestBase_3_2
    {
        virtual void g() {}
        float m_float_3_2_1{};

        int m_prop = 5;

        int get_prop() const { return m_prop; }

        void set_prop(int i_value) { m_prop = i_value; }

        double get_readonly_prop() const { return 1.23; }

        void set_writeonly_prop(char) { }
    };

    REFL_BEGIN_CLASS("TestBase_3_2", TestBase_3_2)
        REFL_BASES()
        REFL_BEGIN_PROPERTIES
            REFL_DATA_PROP("m_float_3_2_1", m_float_3_2_1)
            REFL_ACCESSOR_PROP("prop", get_prop, set_prop)
            REFL_ACCESSOR_RO_PROP("readonly_prop", get_readonly_prop)
            REFL_ACCESSOR_WO_PROP("writeonly_prop", set_writeonly_prop)
        REFL_END_PROPERTIES
    REFL_END_CLASS;

    // layer 2

    struct TestBase_2_1
    {
        char m_char_2_1_1{};
    };

    REFL_BEGIN_CLASS("TestBase_2_1", TestBase_2_1)
        REFL_BASES()
        REFL_BEGIN_PROPERTIES
            REFL_DATA_PROP("m_char_2_1_1", m_char_2_1_1)
        REFL_END_PROPERTIES
    REFL_END_CLASS;

    struct TestBase_2_2 : TestBase_3_1
    {
        int m_int2_2_1{};
        int m_int2_2_2{};
    };

    REFL_BEGIN_CLASS("TestBase_2_2", TestBase_2_2)
        REFL_BASES(TestBase_3_1)
        REFL_BEGIN_PROPERTIES
            REFL_DATA_PROP("m_int2_2_1", m_int2_2_1)
            REFL_DATA_PROP("m_int2_2_2", m_int2_2_2)
        REFL_END_PROPERTIES
    REFL_END_CLASS;

    struct TestBase_2_3 : TestBase_3_2
    {
        float m_float_2_3_1{};
    };


    REFL_BEGIN_CLASS("TestBase_2_3", TestBase_2_3)
        REFL_BASES(TestBase_3_2)
        REFL_BEGIN_PROPERTIES
           REFL_DATA_PROP("m_float_2_3_1", m_float_2_3_1)
        REFL_END_PROPERTIES
    REFL_END_CLASS;

    struct TestBase_2_4
    {
        double m_double_2_4_1{};
        double m_double_2_4_2{};
    };

    REFL_BEGIN_CLASS("TestBase_2_4", TestBase_2_4)
        REFL_BASES()
        REFL_BEGIN_PROPERTIES
            REFL_DATA_PROP("m_double_2_4_1", m_double_2_4_1)
            REFL_DATA_PROP("m_double_2_4_2", m_double_2_4_2)
        REFL_END_PROPERTIES
    REFL_END_CLASS;

    struct TestBase_2_Base
    {
        int8_t m_int8_2_1{};
        int8_t m_int8_2_2{};
        int8_t m_int8_2_3{};
    };

    REFL_BEGIN_CLASS("TestBase_2_Base", TestBase_2_Base)
        REFL_BASES()
        REFL_BEGIN_PROPERTIES
            REFL_DATA_PROP("m_int8_2_1", m_int8_2_1)
            REFL_DATA_PROP("m_int8_2_2", m_int8_2_2)
            REFL_DATA_PROP("m_int8_2_3", m_int8_2_3)
        REFL_END_PROPERTIES
    REFL_END_CLASS;

    // layer 1

    struct TestBase_1_1 : TestBase_2_1, TestBase_2_2, virtual TestBase_2_Base
    {
        const char * m_string_1 = "abc";
    };

    REFL_BEGIN_CLASS("TestBase_1_1", TestBase_1_1)
        REFL_BASES(TestBase_2_1, TestBase_2_2, TestBase_2_Base)
        REFL_BEGIN_PROPERTIES
            REFL_DATA_PROP("m_string_1", m_string_1)
        REFL_END_PROPERTIES
    REFL_END_CLASS;

    struct TestBase_1_2 : TestBase_2_3, TestBase_2_4, virtual TestBase_2_Base
    {
        const char * m_string_2_1 = nullptr;
        const char * m_string_2_2 = "efg";
        const char * m_string_2_3 = nullptr;
    };


    REFL_BEGIN_CLASS("TestBase_1_1", TestBase_1_2)
        REFL_BASES(TestBase_2_3, TestBase_2_4, TestBase_2_Base)
        REFL_BEGIN_PROPERTIES
            REFL_DATA_PROP("m_string_2_1", m_string_2_1)
            REFL_DATA_PROP("m_string_2_2", m_string_2_2)
            REFL_DATA_PROP("m_string_2_3", m_string_2_3)
        REFL_END_PROPERTIES
    REFL_END_CLASS;


    // layer 0

    struct TestClass : TestBase_1_1, TestBase_1_2
    {
        int m_integer = 42;
        float m_float = 42.f;

        int action_1(int a, const float b, double c)
        {
            m_integer += a;
            m_float += b;
            return m_integer;
        }

        int action_2(int a, const float b)
        {
            m_integer += a *2;
            m_float += b * 2;
            return m_integer;
        }
    };

    REFL_BEGIN_CLASS("TestClass", TestClass)
        REFL_BASES(TestBase_1_1, TestBase_1_2)
        REFL_BEGIN_PROPERTIES
            REFL_DATA_PROP("m_integer", m_integer)
            REFL_DATA_PROP("m_float", m_float)
        REFL_END_PROPERTIES
        REFL_BEGIN_ACTIONS
            REFL_ACTION("action_1", action_1)
            REFL_ACTION("action_2", action_2)
        REFL_END_ACTIONS
    REFL_END_CLASS;

    void class_tests_print_props(ediacaran::raw_ptr i_source)
    {
        using namespace ediacaran;

        for (auto const & prop : inspect_properties(i_source))
        {
            std::string str = prop.owning_class().name();
            str += " -> ";
            str += prop.name();
            str += ": ";
            str += to_string(prop.qualified_type());
            str += " = ";
            str += prop.get_string_value();
            std::cout << str << std::endl;
        }
    }

    void class_tests()
    {
        using namespace ediacaran;
        get_type<TestClass>();
        try
        {
            property props[2] = {
              property(property::offset_tag{}, property_flags::inplace, "prop", get_qualified_type<int>(), 2),
              property(property::offset_tag{}, property_flags::inplace, "prop", get_qualified_type<int>(), 2)};
            class_type CC("abc", 1, 2, special_functions{}, array_view<const base_class>{}, props, array_view<const action>{});

            ENCELADO_TEST_ASSERT(false); // should have thrown
        }
        catch (std::exception i_exc)
        {
            std::cout << "expected error: " << i_exc.what() << std::endl;
        }

        TestClass test_object;
        for (auto const & prop : inspect_properties(&test_object))
        {
            std::cout << prop.owning_class().name().data() << " -> " << prop.property().name().data() << std::endl;
        }

        class_tests_print_props(&test_object);

        auto s1 = class_descriptor<TestClass>::bases::size;

        const auto & t = get_type<TestClass>();
    }
}
