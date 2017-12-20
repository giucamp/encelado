

#include "../common.h"
#include "ediacaran/reflection/class_type.h"
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
    };

    REFL_BEGIN_CLASS("TestClass", TestClass)
        REFL_BASES(TestBase_1_1, TestBase_1_2)
        REFL_BEGIN_PROPERTIES
            REFL_DATA_PROP("m_integer", m_integer)
            REFL_DATA_PROP("m_float", m_float)
        REFL_END_PROPERTIES
    REFL_END_CLASS;

    void class_tests_print_props(const void * i_sub_object, ediacaran::class_type const & i_class)
    {
        using namespace ediacaran;

        char chars[1024];
        for (auto & prop : i_class.properties())
        {
            char_writer string_out(chars);
            string_out << i_class.name() << " -> " << prop.name() << ": " << prop.qualified_type() << " = ";
            auto const primary_type = prop.qualified_type().primary_type();
            auto const buffer = operator new (primary_type->size(), std::align_val_t{primary_type->alignment()});

            ediacaran::char_writer err;
            prop.get(i_sub_object, buffer, err);

            if (prop.qualified_type() == get_qualified_type<const char *>())
            {
                auto const string = *static_cast<const char **>(buffer);
                if (string != nullptr)
                    string_out << string;
                else
                    string_out << "NULL";
            }
            else
            {
                primary_type->to_chars(buffer, string_out);
            }

            primary_type->destroy(buffer);
            operator delete (buffer, primary_type->size(), std::align_val_t{primary_type->alignment()});

            std::cout << chars << std::endl;
        }
    }

    void class_tests_print(void const * i_complete_object, ediacaran::class_type const & i_class)
    {
        class_tests_print_props(i_complete_object, i_class);
        for (auto & base : i_class.base_classes())
        {
            class_tests_print_props(base.up_cast(i_complete_object), base.get_class());
        }
    }

    template <typename CLASS> void class_tests_print(CLASS const & i_object)
    {
        class_tests_print(&i_object, ediacaran::get_naked_type<CLASS>());
    }

    // MMM_BasesTraits<CLASS>::bases - class_descriptor<CLASS>::bases if exists, type_list<> otherwise
    template <typename CLASS, typename = std::void_t<>> struct MMM_BasesTraits
    {
        constexpr static bool boo = false;
    };
    template <typename CLASS> struct MMM_BasesTraits<CLASS, std::void_t<>>
    {
        constexpr static bool boo = true;
    };

    void class_tests()
    {
        static_assert(MMM_BasesTraits<TestClass>::boo);

        TestClass test_object;
        class_tests_print(test_object);

        //auto s = ::ediacaran::BasesTraits<TestClass>::bases::size;
        //auto s3 = ::ediacaran::detail::BasesTraits<TestClass>::gg;

        auto s1 = ediacaran::class_descriptor<TestClass>::bases::size;

        const auto & t = ediacaran::get_naked_type<TestClass>();
    }
}
