

#include "../common.h"
#include "ediacaran/reflection/class_type.h"
#include <iostream>
#include <string>
#include <new>

namespace ediacaran_test
{
    // layer 3

    struct TestBase_3_1
    {
        virtual void f() {}
    };

    struct {
        constexpr static char * name = "TestBase_3_1";
        using this_class = TestBase_3_1;
        using bases = type_list<>;
    } get_type_descriptor(TestBase_3_1*& i_dummy); 

    struct TestBase_3_2
    {
        virtual void g() {}
        float m_float_3_2_1{};
    };

    struct {
        constexpr static char * name = "TestBase_3_2";
        using this_class = TestBase_3_2;
        using bases = type_list<>;
        constexpr static property properties[] = {
            ediacaran::make_property<decltype(&this_class::m_float_3_2_1), &this_class::m_float_3_2_1>("m_float_3_2_1") };
    } get_type_descriptor(TestBase_3_2*& i_dummy); 

    // layer 2

    struct TestBase_2_1
    {
        char m_char_2_1_1{};
    };

    struct {
        constexpr static char * name = "TestBase_2_1";
        using this_class = TestBase_2_1;
        using bases = type_list<>;
        constexpr static property properties[] = {
            ediacaran::make_property<decltype(&this_class::m_char_2_1_1), &this_class::m_char_2_1_1>("m_char_2_1_1") };
    } get_type_descriptor(TestBase_2_1*& i_dummy); 

    struct TestBase_2_2 : TestBase_3_1
    {
        int m_int2_2_1{};
        int m_int2_2_2{};
    };

    struct {
        constexpr static char * name = "TestBase_2_2";
        using this_class = TestBase_2_2;
        using bases = type_list<TestBase_3_1>;
        constexpr static property properties[] = {
            ediacaran::make_property<decltype(&this_class::m_int2_2_1), &this_class::m_int2_2_1>("m_char_2_1_1"),
            ediacaran::make_property<decltype(&this_class::m_int2_2_2), &this_class::m_int2_2_1>("m_int2_2_2")};
    } get_type_descriptor(TestBase_2_2*& i_dummy); 

    struct TestBase_2_3 : TestBase_3_2
    {
        float m_float_2_3_1{};
    };

    struct {
        constexpr static char * name = "TestBase_2_3";
        using this_class = TestBase_2_3;
        using bases = type_list<TestBase_3_2>;
        constexpr static property properties[] = {
            ediacaran::make_property<decltype(&this_class::m_float_2_3_1), &this_class::m_float_2_3_1>("m_float_2_3_1")};
    } get_type_descriptor(TestBase_2_3*& i_dummy); 

    struct TestBase_2_4
    {
        double m_double_2_4_1{};
        double m_double_2_4_2{};
    };

    struct {
        constexpr static char * name = "TestBase_2_4";
        using this_class = TestBase_2_4;
        using bases = type_list<>;
        constexpr static property properties[] = {
            ediacaran::make_property<decltype(&this_class::m_double_2_4_1), &this_class::m_double_2_4_1>("m_double_2_4_1"),
            ediacaran::make_property<decltype(&this_class::m_double_2_4_2), &this_class::m_double_2_4_2>("m_double_2_4_2")};
    } get_type_descriptor(TestBase_2_4*& i_dummy); 

    struct TestBase_2_Base
    {
        int8_t m_int8_2_1{};
        int8_t m_int8_2_2{};
        int8_t m_int8_2_3{};
    };

    struct {
        constexpr static char * name = "TestBase_2_Base";
        using this_class = TestBase_2_Base;
        using bases = type_list<>;
        constexpr static property properties[] = {
            ediacaran::make_property<decltype(&this_class::m_int8_2_1), &this_class::m_int8_2_1>("m_int8_2_1"),
            ediacaran::make_property<decltype(&this_class::m_int8_2_2), &this_class::m_int8_2_2>("m_int8_2_2"),
            ediacaran::make_property<decltype(&this_class::m_int8_2_3), &this_class::m_int8_2_3>("m_int8_2_3")};
    } get_type_descriptor(TestBase_2_Base*& i_dummy); 

    // layer 1

    struct TestBase_1_1 : TestBase_2_1, TestBase_2_2, virtual TestBase_2_Base
    {
        const char * m_string_1 = "abc";
    };

    struct {
        constexpr static char * name = "TestBase_1_1";
        using this_class = TestBase_1_1;
        using bases = type_list<TestBase_2_1, TestBase_2_2, TestBase_2_Base>;
        constexpr static property properties[] = {
            ediacaran::make_property<decltype(&this_class::m_string_1), &this_class::m_string_1>("m_string_1")};
    } get_type_descriptor(TestBase_1_1*& i_dummy); 

    struct TestBase_1_2 : TestBase_2_3, TestBase_2_4, virtual TestBase_2_Base
    {
        const char * m_string_2_1 = nullptr;
        const char * m_string_2_2 = "efg";
        const char * m_string_2_3 = nullptr;
    };

    struct {
        constexpr static char * name = "TestBase_1_2";
        using this_class = TestBase_1_2;
        using bases = type_list<TestBase_2_3, TestBase_2_4, TestBase_2_Base>;
        constexpr static property properties[] = {
            ediacaran::make_property<decltype(&this_class::m_string_2_1), &this_class::m_string_2_1>("m_string_2_1"),
            ediacaran::make_property<decltype(&this_class::m_string_2_2), &this_class::m_string_2_2>("m_string_2_2"),
            ediacaran::make_property<decltype(&this_class::m_string_2_3), &this_class::m_string_2_3>("m_string_2_3")};
    } get_type_descriptor(TestBase_1_2*& i_dummy); 


    // layer 0

    struct TestClass : TestBase_1_1, TestBase_1_2
    {
        int m_integer = 42;
        float m_float = 42.f;
    };

    struct {
        constexpr static char * name = "TestClass";
        using this_class = TestClass;
        using bases = type_list<TestBase_1_1, TestBase_1_2>;
        
        constexpr static property properties[] = {
            ediacaran::make_property<decltype(&this_class::m_integer), &this_class::m_integer>("integer"),
            ediacaran::make_property<decltype(&this_class::m_float), &this_class::m_float>("float") };
    } get_type_descriptor(TestClass*&);

    void class_tests_print_props(const void * i_sub_object, class_type const & i_class)
    {
        char chars[1024];
        for(auto & prop : i_class.properties())
        {
            char_writer string_out(chars);
            string_out << i_class.name() << " -> " << prop.name() << ": ";
            auto const primary_type = prop.qualified_type().primary_type();
            auto const buffer = operator new (primary_type->size(), std::align_val_t{primary_type->alignment()});
            
            prop.get(i_sub_object, buffer, ediacaran::char_writer{});

            if(prop.qualified_type() == get_qualified_type<const char *>())
            {
                auto const string = *static_cast<const char * *>(buffer);
                if(string != nullptr)
                    string_out << string;
                else
                    string_out << "NULL";
            }
            else
            {
                primary_type->special_functions().to_chars()(buffer,string_out);
            }

            primary_type->special_functions().scalar_destructor()(buffer, static_cast<char*>(buffer) + primary_type->size());
            operator delete (buffer, primary_type->size(), std::align_val_t{primary_type->alignment()});

            std::cout << chars << std::endl;
        }
    }

    void class_tests_print(void const * i_complete_object, class_type const & i_class)
    {
        class_tests_print_props(i_complete_object, i_class);
        for(auto & base : i_class.base_classes())
        {
            class_tests_print_props(base.up_cast(i_complete_object), base.get_class());
        }
    }

    template <typename CLASS>
        void class_tests_print(CLASS const & i_object)
    {
        class_tests_print(&i_object, get_naked_type<CLASS>());
    }

    void class_tests()
    {
        TestClass test_object;
        class_tests_print(test_object);

        constexpr auto g = all_bases<TestClass>::type::size;

        const auto & t = get_naked_type<TestClass>();
    }
}
