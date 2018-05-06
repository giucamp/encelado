
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "test_types.h"

namespace cambrian_test
{
    namespace serialization
    {
        void edit_serialization_test_data(TestClass & i_result, int32_t i_depth)
        {
            i_result.m_int    = i_depth;
            i_result.m_double = static_cast<double>(i_depth);

            if (i_depth > 0)
            {
                i_result.m_objects_1.resize(3);
                for (auto & obj : i_result.m_objects_1)
                    edit_serialization_test_data(obj, i_depth - 1);

                i_result.m_objects_2.resize(2);
                for (auto & obj : i_result.m_objects_2)
                    edit_serialization_test_data(obj, i_depth - 1);
            }
        }

        dyn_value make_serialization_test_data()
        {
            TestClass result;
            edit_serialization_test_data(result, 3);
            return raw_ptr(&result);
        }

    } // namespace serialization

} // namespace cambrian_test
