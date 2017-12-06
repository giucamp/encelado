
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2016-2017.


#include "ediacaran/core/char_writer.h"
#include "ediacaran/reflection/qualified_type_ptr.h"
#include "ediacaran/reflection/type.h"
#include <iostream>
#include <string>

constexpr ediacaran::qualified_type_ptr t1 = ediacaran::get_type<int>();

namespace ediacaran
{
    void core_tests();
}

int ha() { return 7786; }

int h = ha();

constexpr auto t = ediacaran::get_naked_type<int>();


/*int main()
{
    using namespace ediacaran;

    //core_tests();

    char sss[1024];
    char_writer b(sss);
    b << 1234;

    int g = 6;
    
    t.special_functions().to_chars()(b, &g);

    std::cout << t.name();
    int h = 0;
    //auto t1 = get_naked_type<std::string>();
}*/
