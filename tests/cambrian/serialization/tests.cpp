
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "cambrian/data/serializer.h"
#include "cambrian/data/type_registry.h"
#include "test_types.h"
#include <memory>
#include <vector>

namespace cambrian_test
{
    using namespace cambrian;

    namespace serialization
    {
        void tests()
        {
            dyn_value data = make_serialization_test_data();

            std::vector<std::vector<unsigned char>> pages;

            type_registry registry;
            binary_writer writer(registry, data);

            bool finished = false;
            while (!finished)
            {
                pages.emplace_back(512, 55);
                byte_writer byte_wr(pages.back().data(), pages.back().size());
                finished = writer.step(byte_wr) == binary_writer::finished;
                ENCELADO_TEST_ASSERT(byte_wr.remaining_size() >= 0);
                auto const actual_page_size =
                  static_cast<size_t>(pages.back().size() - byte_wr.remaining_size());
                pages.back().resize(actual_page_size);
            }

            int g = 0;
        }

    } // namespace serialization

} // namespace cambrian_test
