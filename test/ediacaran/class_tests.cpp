

#include "../common.h"
#include "ediacaran/reflection/class_type.h"
#include <iostream>
#include <string>

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
    };

    struct {
        constexpr static char * name = "TestBase_3_2";
        using this_class = TestBase_3_2;
        using bases = type_list<>;
    } get_type_descriptor(TestBase_3_2*& i_dummy); 

    // layer 2

    struct TestBase_2_1
    {
        char m_arr_1[32]{};
    };

    struct {
        constexpr static char * name = "TestBase_2_1";
        using this_class = TestBase_2_1;
        using bases = type_list<>;
    } get_type_descriptor(TestBase_2_1*& i_dummy); 

    struct TestBase_2_2 : TestBase_3_1
    {
        int m_arr_2[32]{};
    };

    struct {
        constexpr static char * name = "TestBase_2_2";
        using this_class = TestBase_2_2;
        using bases = type_list<TestBase_3_1>;
    } get_type_descriptor(TestBase_2_2*& i_dummy); 

    struct TestBase_2_3 : TestBase_3_2
    {
        float m_arr_3[32]{};
    };

    struct {
        constexpr static char * name = "TestBase_2_3";
        using this_class = TestBase_2_3;
        using bases = type_list<TestBase_3_2>;
    } get_type_descriptor(TestBase_2_3*& i_dummy); 

    struct TestBase_2_4
    {
        double m_arr_4[32]{};
    };

    struct {
        constexpr static char * name = "TestBase_2_4";
        using this_class = TestBase_2_4;
        using bases = type_list<>;
    } get_type_descriptor(TestBase_2_4*& i_dummy); 

    struct TestBase_2_Base
    {
        int8_t m_arr_5[3]{};
    };

    struct {
        constexpr static char * name = "TestBase_2_Base";
        using this_class = TestBase_2_Base;
        using bases = type_list<>;
    } get_type_descriptor(TestBase_2_Base*& i_dummy); 

    // layer 1

    struct TestBase_1_1 : TestBase_2_1, TestBase_2_2, virtual TestBase_2_Base
    {
        std::string m_string_1 = "abc";
    };

    struct {
        constexpr static char * name = "TestBase_1_1";
        using this_class = TestBase_1_1;
        using bases = type_list<TestBase_2_1, TestBase_2_2, TestBase_2_Base>;
    } get_type_descriptor(TestBase_1_1*& i_dummy); 

    struct TestBase_1_2 : TestBase_2_3, TestBase_2_4, virtual TestBase_2_Base
    {
        std::string m_string_2 = "efg";
    };

    struct {
        constexpr static char * name = "TestBase_1_2";
        using this_class = TestBase_1_2;
        using bases = type_list<TestBase_2_3, TestBase_2_4, TestBase_2_Base>;
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



    void class_tests()
    {
        constexpr auto g = all_bases<TestClass>::type::size;

        const auto & t = get_naked_type<TestClass>();

        std::cout << all_bases<TestBase_3_1>::type::size;
        all_bases<TestBase_3_1>::type::dbg_print(std::cout);

        tl_push_back_t<type_list<int, double>, float, type_list<int, double>, float, type_list<int, double>>::dbg_print(
          std::cout);
        //detail::BaseList<TestBase_1_1>::type::dbg_print(std::cout);

        // using L =type_list<int,float>;
        // L::dbg_print(std::cout); std::cout << std::endl;
        // tl_push_back_t<L, double, std::string>::dbg_print(std::cout); std::cout << std::endl;

        all_bases<TestClass>::type::dbg_print(std::cout);
    }
}
