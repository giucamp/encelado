

namespace ediacaran_test
{
    // layer 3

    struct TestBase_3_1;
}

#include "../common.h"
#include "ediacaran/reflection/class_type.h"
#include <iostream>
#include <string>

namespace ediacaran_test
{
    // layer 3

    struct TestBase_3_1
    {
        using bases = type_list<>;

        virtual void f() {}
    };

    struct TestBase_3_2
    {
        using bases = type_list<>;

        virtual void g() {}
    };

    // layer 2

    struct TestBase_2_1
    {
        using bases = type_list<>;

        char m_arr_1[32]{};
    };

    struct TestBase_2_2 : TestBase_3_1
    {
        using bases = type_list<TestBase_3_1>;

        int m_arr_2[32]{};
    };

    struct TestBase_2_3 : TestBase_3_2
    {
        using bases = type_list<TestBase_3_2>;

        float m_arr_3[32]{};
    };

    struct TestBase_2_4
    {
        using bases = type_list<>;

        double m_arr_4[32]{};
    };

    // layer 1

    struct TestBase_1_1 : TestBase_2_1, TestBase_2_2
    {
        using bases = type_list<TestBase_2_1, TestBase_2_2>;

        std::string m_string_1 = "abc";
    };

    struct TestBase_1_2 : TestBase_2_3, TestBase_2_4
    {
        using bases = type_list<TestBase_2_3, TestBase_2_4>;

        std::string m_string_2 = "efg";
    };

    // layer 0

    struct TestClass : TestBase_1_1, TestBase_1_2
    {
        using bases = type_list<TestBase_1_1, TestBase_1_2>;

        int m_integer = 42;
        float m_float = 42.f;
    };
}

namespace ediacaran
{
    template <typename CLASS> struct class_descriptor
    {
        using bases = typename CLASS::bases;
    };

    class_type create_class(tag<ediacaran_test::TestClass>)
    {
        return ediacaran::make_static_class<ediacaran_test::TestClass>("TestClass");
    }

    class_type create_class(tag<ediacaran_test::TestBase_3_1>)
    {
        return ediacaran::make_static_class<ediacaran_test::TestBase_3_1>("TestBase_3_1");
    }

    class_type create_class(tag<ediacaran_test::TestBase_3_2>)
    {
        return ediacaran::make_static_class<ediacaran_test::TestBase_3_2>("TestBase_3_2");
    }


    class_type create_class(tag<ediacaran_test::TestBase_2_1>)
    {
        return ediacaran::make_static_class<ediacaran_test::TestBase_2_1>("TestBase_2_1");
    }


    class_type create_class(tag<ediacaran_test::TestBase_2_2>)
    {
        return ediacaran::make_static_class<ediacaran_test::TestBase_2_2>("TestBase_2_2");
    }

    class_type create_class(tag<ediacaran_test::TestBase_2_3>)
    {
        return ediacaran::make_static_class<ediacaran_test::TestBase_2_3>("TestBase_2_3");
    }

    class_type create_class(tag<ediacaran_test::TestBase_2_4>)
    {
        return ediacaran::make_static_class<ediacaran_test::TestBase_2_4>("TestBase_2_4");
    }

    class_type create_class(tag<ediacaran_test::TestBase_1_1>)
    {
        return ediacaran::make_static_class<ediacaran_test::TestBase_1_1>("TestBase_1_1");
    }

    class_type create_class(tag<ediacaran_test::TestBase_1_2>)
    {
        return ediacaran::make_static_class<ediacaran_test::TestBase_1_2>("TestBase_1_2");
    }
}

namespace ediacaran_test
{

    void class_tests()
    {
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
