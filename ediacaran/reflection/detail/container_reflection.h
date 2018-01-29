#pragma once

#include <ediacaran/reflection/container.h>
#include <iterator>

namespace ediacaran
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

        template <typename FORWARD_ITERATOR>
            constexpr FORWARD_ITERATOR bounded_next(const FORWARD_ITERATOR & i_iterator,
                typename std::iterator_traits<FORWARD_ITERATOR>::size_type i_offset,
                const FORWARD_ITERATOR & i_end, std::random_access_iterator_tag)
        {
            return i_iterator + i_offset;
        }

        template <typename FORWARD_ITERATOR>
            constexpr FORWARD_ITERATOR bounded_next(const FORWARD_ITERATOR & i_iterator,
                typename std::iterator_traits<FORWARD_ITERATOR>::size_type i_offset,
                const FORWARD_ITERATOR & i_end, std::input_iterator_tag)
        {
            for(; i_offset > 0 && i_iterator != i_end; i_offset--)
            {
                i_iterator++;
            }
            return i_iterator;
        }

        template <typename CONTAINER> struct StdContainer
        {
            using native_iterator = decltype(std::begin(std::declval<CONTAINER>()));
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

            static void construct_iterator(
              void * i_container, void * i_iterator_dest, container::index i_start_index)
            {
                EDIACARAN_ASSERT(i_container != nullptr);
                EDIACARAN_ASSERT(i_iterator_dest != nullptr);

                // convert start_index to the native difference_type
                using difference_type =
                  typename std::iterator_traits<native_iterator>::difference_type;
                auto const start_index = static_cast<difference_type>(i_start_index);
                if (static_cast<container::index>(start_index) != i_start_index)
                {
                    except<std::runtime_error>(
                      "Narrowing conversion of ",
                      i_start_index,
                      " to the difference_type of the container");
                }

                // construct the iterator
                auto & container = *static_cast<CONTAINER *>(i_container);
                new (get_iterator_ptr(i_iterator_dest))
                  Iterator{std::next(std::begin(container), start_index), std::end(container)};
            }

            static void destroy_iterator(void * i_iterator_dest) noexcept
            {
                std::destroy_at(get_iterator_ptr(i_iterator_dest));
            }

            static void iterator_move_and_get(
              void *                  i_iterator,
              container::signed_index i_index_offset,
              qualified_type_ptr *    o_element_type,
              void **                 o_elements,
              container::index *      o_count)
            {
                auto & iterator = *get_iterator_ptr(i_iterator);

                // convert start_index to the native difference_type
                using difference_type =
                  typename std::iterator_traits<native_iterator>::difference_type;
                auto const index_offset = static_cast<difference_type>(i_index_offset);
                if (static_cast<container::signed_index>(index_offset) != i_index_offset)
                {
                    except<std::runtime_error>(
                      "Narrowing conversion of ",
                      i_index_offset,
                      " to the difference_type of the container");
                }

                iterator.m_curr = std::next(iterator.m_curr, index_offset);
                *o_element_type = get_qualified_type<element_type>();
            }
        };

    } //namespace detail

    template <
      typename CONTAINER,
      typename std::enable_if_t<detail::HasStdContainerInterface<CONTAINER>::value> * = nullptr>
    constexpr container make_container_reflection() noexcept
    {
        using Cont = detail::StdContainer<CONTAINER>;
        return container{container::capability::none,
                         get_qualified_type<typename Cont::element_type>(),
                         Cont::iterator_storage_size,
                         &Cont::construct_iterator,
                         &Cont::destroy_iterator,
                         &Cont::iterator_move_and_get};
    }

} // namespace ediacaran
