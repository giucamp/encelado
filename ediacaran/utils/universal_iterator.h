
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <ediacaran/reflection/container.h>
#include <ediacaran/utils/raw_ptr.h>
#include <type_traits>

namespace edi
{
    class universal_iterator
    {
      public:
        universal_iterator(raw_ptr i_target);

        template <typename TYPE>
        explicit universal_iterator(TYPE * i_ptr) : universal_iterator(raw_ptr(i_ptr))
        {
        }


        universal_iterator(const universal_iterator &) = delete;
        universal_iterator & operator=(const universal_iterator &) = delete;

        universal_iterator & operator++();

        ~universal_iterator();

        raw_ptr operator*() const noexcept
        {
            EDIACARAN_ASSERT(!is_over());
            return raw_ptr(m_curr_segment.m_elements, m_curr_segment.element_type);
        }

        bool is_over() const noexcept { return m_curr_segment.m_element_count == 0; }

      private:
        static void * allocate_iterator(size_t i_size);
        static void   deallocate_iterator(void * i_block, size_t i_size) noexcept;
        void          destroy_iterator() noexcept;

        void bind_container(const container * i_container, void * i_object);

      private:
        container::segment m_curr_segment;

        void *                  m_iterator{};
        static constexpr size_t s_inplace_storage_size = 64;
        std::aligned_storage_t<s_inplace_storage_size, container::iterator_alignment>
          m_iterator_inline_storage;

        const container * m_container{};
    };

} // namespace edi
