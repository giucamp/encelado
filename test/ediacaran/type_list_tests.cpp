

#include "../common.h"
#include "ediacaran/core/type_list.h"
#include <iostream>
#include <string>

namespace ediacaran_test
{
    void type_list_tests()
    {
        using namespace ediacaran;

        // push_back, contains

        using L  = type_list<int, float, double>;
        using L1 = tl_push_back_t<L, type_list<std::string, int>, char>;
        static_assert(tl_contains_v<int, L1>);
        static_assert(tl_contains_v<float, L1>);
        static_assert(tl_contains_v<double, L1>);
        static_assert(tl_contains_v<char, L1>);
        static_assert(!tl_contains_v<void, L1>);
        static_assert(std::is_same_v<L1, type_list<int, float, double, std::string, int, char>>);

        using L2 = tl_push_back_t<L, int>;
        static_assert(std::is_same_v<L2, type_list<int, float, double, int>>);
        static_assert(L2::size == 4);

        using L3 = tl_push_back_t<
          L,
          type_list<char, int *, int **>,
          type_list<void, char ***>,
          type_list<void **, char *****>>;
        static_assert(std::is_same_v<
                      L3,
                      type_list<
                        int,
                        float,
                        double,
                        char,
                        int *,
                        int **,
                        void,
                        char ***,
                        void **,
                        char *****>>);
        static_assert(L3::size == 10);

        static_assert(tl_contains_v<int, int>);
        static_assert(tl_contains_v<int, float, int, double>);
        static_assert(!tl_contains_v<char, float, int, double>);

        static_assert(tl_contains<int, int>::value);
        static_assert(tl_contains<int, float, int, double>::value);
        static_assert(!tl_contains<char, float, int, double>::value);

        // tl_remove_duplicates_t

        using R1 = tl_remove_duplicates_t<L1>;
        static_assert(std::is_same_v<R1, type_list<int, float, double, std::string, char>>);

        using R2 = tl_remove_duplicates_t<type_list<>>;
        static_assert(std::is_same_v<R2, type_list<>>);

        using R3 = tl_remove_duplicates_t<type_list<char, char, char>>;
        static_assert(std::is_same_v<R3, type_list<char>>);

        using R4 = tl_remove_duplicates_t<type_list<char, int, char, char, int, double>>;
        static_assert(std::is_same_v<R4, type_list<char, int, double>>);


        // difference

        using D = tl_difference<L1, type_list<int, const void>>::type;
        static_assert(std::is_same_v<D, type_list<float, double, std::string, char>>);

        using D1 = tl_difference<L1, type_list<>>::type;
        static_assert(std::is_same_v<D1, type_list<int, float, double, std::string, int, char>>);

        using D2 = tl_difference<type_list<>, type_list<>>::type;
        static_assert(std::is_same_v<D2, type_list<>> && D2::size == 0);

        using D3 = tl_difference<L1, type_list<int, const void, char, float>>::type;
        static_assert(std::is_same_v<D3, type_list<double, std::string>>);

        using D4 =
          tl_difference<L1, type_list<int, const void, double, std::string, char, float>>::type;
        static_assert(std::is_same_v<D4, type_list<>>);

        using D5 = tl_difference<L1, L1>::type;
        static_assert(std::is_same_v<D5, type_list<>>);

        // is_type_list_v
        static_assert(is_type_list_v<D4> && is_type_list_v<D3>);
        static_assert(!is_type_list_v<int> && !is_type_list_v<D3 *>);
    }
}