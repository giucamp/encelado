
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../common.h"
#include "ediacaran/core/string_builder.h"
#include "ediacaran/std_refl/string.h"
#include "ediacaran/std_refl/vector.h"
#include "ediacaran/utils/dyn_value.h"
#include "ediacaran/utils/inspect.h"
#include <iostream>
#include <string>
#include <vector>

namespace ediacaran_test
{
    // Animal

    class Animal
    {
        std::string m_name;

      public:
        std::string get_name_2() const { return m_name; }

        std::string const & get_name_3() const { return m_name; }

        std::string get_name_4() const { return m_name; }
        void        set_name_4(std::string i_value) { m_name = i_value; }

        std::string const & get_name_5() const noexcept { return m_name; }
        void                set_name_5(std::string const & i_value) { m_name = i_value; }

        virtual const char * get_color() const noexcept = 0;

        virtual std::string make_sound() const = 0;

        virtual void move_by(int x, int y) noexcept = 0;

        friend constexpr auto reflect(Animal ** i_ptr);
    };

    constexpr auto reflect(Animal ** i_ptr)
    {
        using namespace edi;
        char const class_name[] = "Animal";
        using this_class        = std::remove_reference_t<decltype(**i_ptr)>;

        auto const properties = make_array(
          make_property<EDI_DATA(m_name)>("name"),
          make_property<EDI_FUNC(get_name_2)>("name_2"),
          make_property<EDI_FUNC(get_name_3)>("name_3"),
          make_property<EDI_FUNC(get_name_4), EDI_FUNC(set_name_4)>("name_4"),
          make_property<EDI_FUNC(get_name_5), EDI_FUNC(set_name_5)>("name_5"),
          make_property<EDI_FUNC(get_color)>("color"));

        auto const functions = edi::make_array(
          make_function<EDI_FUNC(make_sound)>("make_sound"),
          make_function<EDI_FUNC(move_by)>("move_by", "x, y"));
        return make_class<this_class>(class_name, properties, functions);
    }

    // Flying

    class Flying : public virtual Animal
    {
      public:
        std::vector<int> m_wings;

        virtual ~Flying() = default;

        virtual size_t wing_count() const { return m_wings.size(); }
    };

    constexpr auto reflect(Flying ** i_ptr)
    {
        using namespace edi;
        char const class_name[] = "Flying";
        using this_class        = std::remove_reference_t<decltype(**i_ptr)>;

        auto const properties = make_array(
          make_property<EDI_DATA(m_wings)>("m_wings"),
          make_property<EDI_FUNC(wing_count)>("m_wings"));
        return make_class<this_class>(class_name, properties);
    }

    // Vertebrate

    class Vertebrate : public virtual Animal
    {
      public:
        int m_vertebre_count = 18;
    };

    constexpr auto reflect(Vertebrate ** i_ptr)
    {
        using namespace edi;
        char const class_name[] = "Vertebrate";
        using bases             = type_list<Animal>;
        using this_class        = std::remove_reference_t<decltype(**i_ptr)>;

        auto const properties =
          make_array(make_property<EDI_DATA(m_vertebre_count)>("vertebre_count"));

        return make_class<this_class, bases>(class_name, properties);
    }

    // Mammal

    class Mammal : public Vertebrate
    {
    };

    // Invertebrate

    class Invertebrate : public virtual Animal
    {
    };

    // Insect

    class Insect : public Invertebrate, public Flying
    {
    };

    // Bat

    class Bat : public Mammal, public Flying
    {
    };

    // Bird

    class Bird : public Vertebrate, public Flying
    {
    };

    void animalia_tests()
    {
        using namespace edi;

        constexpr auto coll = make_container_reflection<std::vector<int>>();

        constexpr auto anlimal_type = get_qualified_type<Animal>();
        auto           u            = get_type<Animal>();
        static_assert(
          find_named(get_type<Animal>().functions(), "make_sound")->qualification() ==
          cv_qualification::const_q);
    }
}
