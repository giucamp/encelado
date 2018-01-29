#pragma once
#include <ediacaran/reflection/container.h>
#include <ediacaran/utils/raw_ptr.h>

namespace ediacaran
{
    class iterator
    {
      public:
        iterator(raw_ptr i_container)
        {
            auto const container = i_container.full_indirection();
            //container.qualified_type().final_type()->
        }

        iterator & operator++() { return *this; }

        iterator operator++(int)
        {
            iterator copy(*this);
                     operator++();
            return copy;
        }

        bool is_over() noexcept { return false; }

      private:
        void * m_segment_start{};
        void * m_segment_end{};
    };

} // namespace ediacaran
