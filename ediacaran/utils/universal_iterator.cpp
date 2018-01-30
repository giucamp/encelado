
#include <ediacaran/utils/universal_iterator.h>
#include <new>

namespace ediacaran
{
    void * universal_iterator::allocate_iterator(size_t i_size)
    {
        if constexpr (container::iterator_alignment < __STDCPP_DEFAULT_NEW_ALIGNMENT__)
        {
            return ::operator new(i_size);
        }
        else
        {
            return ::operator new(i_size, std::align_val_t(container::iterator_alignment));
        }
    }

    void universal_iterator::deallocate_iterator(void * i_block, size_t i_size) noexcept
    {
        if constexpr (container::iterator_alignment < __STDCPP_DEFAULT_NEW_ALIGNMENT__)
        {
            ::operator delete(i_block, i_size);
        }
        else
        {
            ::operator delete(i_block, i_size, std::align_val_t(container::iterator_alignment));
        }
    }

    universal_iterator::universal_iterator(raw_ptr i_target)
    {
        auto const target     = i_target.full_indirection();
        auto const final_type = target.qualified_type().final_type();
        if (final_type->is_class())
        {
            auto const class_obj = static_cast<const class_type *>(final_type);
            auto const container = class_obj->container();
            if (container != nullptr)
            {
                auto const iterator_size = container->iterator_size();
                if (iterator_size <= s_inplace_storage_size)
                {
                    auto const iterator = &m_iterator_inline_storage;
                    m_curr_segment      = container->construct_iterator()(
                      iterator, const_cast<void *>(target.object()));
                    m_iterator  = iterator;
                    m_container = container;
                }
                else
                {
                    auto const iterator = allocate_iterator(iterator_size);
                    try
                    {
                        m_curr_segment = container->construct_iterator()(
                          iterator, const_cast<void *>(target.object()));
                        m_iterator  = iterator;
                        m_container = container;
                    }
                    catch (...)
                    {
                        deallocate_iterator(iterator, iterator_size);
                        throw;
                    }
                }
            }
        }
    }

    universal_iterator & universal_iterator::operator++()
    {
        EDIACARAN_ASSERT(m_curr_segment.m_element_count > 0);
        --m_curr_segment.m_element_count;
        m_curr_segment.m_elements = address_add(
          m_curr_segment.m_elements, m_curr_segment.element_type.primary_type()->size());
        if (m_curr_segment.m_element_count == 0)
        {
            m_curr_segment = m_container->next_segment()(m_iterator);
        }
        return *this;
    }

    void universal_iterator::destroy_iterator() noexcept
    {
        if (m_iterator != nullptr)
        {
            m_container->destroy_iterator_function()(m_iterator);
            if (m_container->iterator_size() > s_inplace_storage_size)
            {
                deallocate_iterator(m_iterator, m_container->iterator_size());
            }
            m_iterator  = nullptr;
            m_container = nullptr;
        }
    }

    universal_iterator::~universal_iterator() { destroy_iterator(); }

} // namespace ediacaran
