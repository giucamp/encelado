
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <ediacaran/reflection/container.h>
#include <iterator>

namespace edi
{
    namespace detail
    {
        template <typename CONTAINER, typename = std::void_t<>>
        struct HasStdContainerInterface : std::false_type
        {
        };

        template <typename CONTAINER>
        struct HasStdContainerInterface<
          CONTAINER,
          std::void_t<decltype(std::begin(std::declval<CONTAINER>()))>> : std::true_type
        {
        };

        template <typename TYPE> std::add_lvalue_reference_t<TYPE> declval_value();

        template <typename CONTAINER, bool IS_CONTIGUOUS> struct StdContainer;

        template <typename CONTAINER> struct StdContainer<CONTAINER, false>
        {
            using native_iterator = decltype(std::begin(declval_value<CONTAINER>()));

            using element_type =
              std::remove_reference_t<decltype(*std::declval<native_iterator>())>;

            static_assert(
              !std::is_reference_v<native_iterator>, "begin must return an iterator by value");
            static_assert(
              std::is_reference_v<decltype(*std::declval<native_iterator>())>,
              "a deferenced iterator must yeld a reference");

            struct Iterator
            {
                native_iterator m_curr, m_end;
            };

            static constexpr size_t iterator_storage_size =
              alignof(Iterator) <= container::iterator_alignment
                ? sizeof(Iterator)
                : sizeof(Iterator) + alignof(Iterator) - container::iterator_alignment;

            static Iterator * get_iterator_ptr(void * i_storage) noexcept
            {
                Iterator * result{};
                if constexpr (alignof(Iterator) <= container::iterator_alignment)
                    result = static_cast<Iterator *>(i_storage);
                else
                    result = address_upper_align(i_storage, container::iterator_alignment);
                EDIACARAN_ASSERT(result + 1 <= address_add(i_storage, iterator_storage_size));
                return result;
            }

            static container::segment construct_iterator(void * i_iterator_dest, void * i_container)
            {
                EDIACARAN_ASSERT(i_container != nullptr);
                EDIACARAN_ASSERT(i_iterator_dest != nullptr);

                // construct the iterator
                auto & container = *static_cast<CONTAINER *>(i_container);
                auto & iterator  = *new (get_iterator_ptr(i_iterator_dest))
                                    Iterator{std::begin(container), std::end(container)};

                if (iterator.m_curr != iterator.m_end)
                {
                    void * const elements =
                      const_cast<std::remove_cv_t<element_type> *>(&*iterator.m_curr);
                    return container::segment{get_qualified_type<element_type>(), elements, 1};
                }
                else
                {
                    return container::segment{};
                }
            }

            static void destroy_iterator(void * i_iterator_dest) noexcept
            {
                std::destroy_at(get_iterator_ptr(i_iterator_dest));
            }

            static container::segment next_segment(void * i_iterator)
            {
                auto & iterator = *get_iterator_ptr(i_iterator);
                ++iterator.m_curr;
                if (iterator.m_curr != iterator.m_end)
                {
                    void * const elements =
                      const_cast<std::remove_cv_t<element_type> *>(&*iterator.m_curr);
                    return container::segment{get_qualified_type<element_type>(), elements, 1};
                }
                else
                {
                    return container::segment{};
                }
            }
        };

        template <typename CONTAINER> struct StdContainer<CONTAINER, true>
        {
            using native_iterator = decltype(std::begin(declval_value<CONTAINER>()));

            using element_type =
              std::remove_reference_t<decltype(*std::declval<native_iterator>())>;

            static_assert(
              !std::is_reference_v<native_iterator>, "begin must return an iterator by value");
            static_assert(
              std::is_reference_v<decltype(*std::declval<native_iterator>())>,
              "a deferenced iterator must yeld a reference");

            static constexpr size_t iterator_storage_size = 1;

            static container::segment construct_iterator(void * i_iterator_dest, void * i_container)
            {
                EDIACARAN_ASSERT(i_container != nullptr);
                EDIACARAN_ASSERT(i_iterator_dest != nullptr);

                auto & container = *static_cast<CONTAINER *>(i_container);
                return container::segment{
                  get_qualified_type<element_type>(),
                  const_cast<typename CONTAINER::value_type *>(container.data()),
                  container.size()};
            }

            static void destroy_iterator(void * /*i_iterator_dest*/) noexcept {}

            static container::segment next_segment(void * /*i_iterator*/)
            {
                return container::segment{};
            }
        };

    } //namespace detail

    template <
      typename CONTAINER,
      typename std::enable_if_t<detail::HasStdContainerInterface<CONTAINER>::value> * = nullptr>
    constexpr container make_container_reflection() noexcept
    {
        using Cont = detail::StdContainer<CONTAINER, is_contiguous_container_v<CONTAINER>>;
        return container{is_contiguous_container_v<CONTAINER> ? container::capability::contiguous
                                                              : container::capability::none,
                         get_qualified_type<typename Cont::element_type>(),
                         Cont::iterator_storage_size,
                         &Cont::construct_iterator,
                         &Cont::next_segment,
                         &Cont::destroy_iterator};
    }

} // namespace edi
