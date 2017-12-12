

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

    struct TestBase_3_2
    {
        virtual void g() {}
    };

    // layer 2

    struct TestBase_2_1
    {
        char m_arr_1[32]{};
    };

    struct TestBase_2_2 : TestBase_3_1
    {
        int m_arr_2[32]{};
    };

    struct TestBase_2_3 : TestBase_3_2
    {
        float m_arr_3[32]{};
    };

    struct TestBase_2_4
    {
        double m_arr_4[32]{};
    };

    // layer 1

    struct TestBase_1_1 : TestBase_2_1, TestBase_2_2
    {
        std::string m_string_1 = "abc";
    };

    struct TestBase_1_2 : TestBase_2_3, TestBase_2_4
    {
        std::string m_string_2 = "efg";
    };

    // layer 0

    struct TestClass : TestBase_1_1, TestBase_1_2
    {
        int m_integer = 42;
        float m_float = 42.f;
    };
}

namespace ediacaran
{
    template <> struct class_descriptor<ediacaran_test::TestClass>
    {
        constexpr static char * name = "TestClass";
        
        using bases = type_list<ediacaran_test::TestBase_1_1, ediacaran_test::TestBase_1_2>;

        constexpr static property propoerties[] = {
            make_property<int ediacaran_test::TestClass::*, &ediacaran_test::TestClass::m_integer>("integer"),
            make_property<float ediacaran_test::TestClass::*, &ediacaran_test::TestClass::m_float>("float") };
    };

    //

    template <> struct class_descriptor<ediacaran_test::TestBase_1_1>
    {
        constexpr static char * name = "TestBase_1_1";
        using bases = type_list<ediacaran_test::TestBase_2_1, ediacaran_test::TestBase_2_2>;
    };

    template <> struct class_descriptor<ediacaran_test::TestBase_1_2>
    {
        constexpr static char * name = "TestBase_1_2";
        using bases = type_list<ediacaran_test::TestBase_2_3, ediacaran_test::TestBase_2_4>;
    };

    //

    template <> struct class_descriptor<ediacaran_test::TestBase_2_1>
    {
        constexpr static char * name = "TestBase_2_1";
        using bases = type_list<>;
    };

    template <> struct class_descriptor<ediacaran_test::TestBase_2_2>
    {
        constexpr static char * name = "TestBase_2_2";
        using bases = type_list<ediacaran_test::TestBase_3_1>;
    };

    template <> struct class_descriptor<ediacaran_test::TestBase_2_3>
    {
        constexpr static char * name = "TestBase_2_3";
        using bases = type_list<ediacaran_test::TestBase_3_2>;
    };

    template <> struct class_descriptor<ediacaran_test::TestBase_2_4>
    {
        constexpr static char * name = "TestBase_2_3";
        using bases = type_list<>;
    };

    //

    template <> struct class_descriptor<ediacaran_test::TestBase_3_1>
    {
        constexpr static char * name = "TestBase_3_1";
        using bases = type_list<>;
    };

    template <> struct class_descriptor<ediacaran_test::TestBase_3_2>
    {
        constexpr static char * name = "TestBase_3_2";
        using bases = type_list<>;
    };
}

namespace ediacaran_test
{

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
