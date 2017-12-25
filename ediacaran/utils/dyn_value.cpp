
#include <ediacaran/utils/dyn_value.h>
#include <memory>
#include <utility>

namespace ediacaran
{
    dyn_value::dyn_value(const raw_ptr & i_source)
    {
        auto & type = i_source.type();
        if (!type.is_empty())
        {
            auto const primary_type = type.primary_type();
            if (!primary_type->is_copy_constructible())
            {
                char err_message[512];
                to_chars(err_message, "The type ", primary_type->name(), " is not copy-constructible");
                throw unsupported_error(err_message);
            }

            EDIACARAN_INTERNAL_ASSERT(primary_type->is_destructible());

            manual_construct(type, [&](void * i_dest) { primary_type->copy_construct(i_dest, i_source.object()); });
        }
    }

    void * dyn_value::uninitialized_allocate(const qualified_type_ptr & i_type)
    {
        EDIACARAN_ASSERT(!i_type.is_empty());

        auto const primary_type = i_type.primary_type();
        if (!primary_type->is_destructible())
        {
            char err_message[512];
            to_chars(err_message, "The type ", primary_type->name(), " is not destructible");
            throw unsupported_error(err_message);
        }

        auto const prev_primary_type = m_type.primary_type();
        if (prev_primary_type == nullptr || primary_type->size() != prev_primary_type->size() ||
            primary_type->alignment() != prev_primary_type->alignment())
        {
            auto const buffer = operator new (primary_type->size(), std::align_val_t{primary_type->alignment()});
            if (!empty())
                destroy();
            m_object = buffer;
        }
        else
        {
            prev_primary_type->destroy(m_object);
        }
        m_type = i_type;
        return m_object;
    }

    void dyn_value::uninitialized_deallocate() noexcept
    {
        EDIACARAN_INTERNAL_ASSERT(m_object != nullptr);
        auto const primary_type = m_type.primary_type();
        operator delete (m_object, primary_type->size(), std::align_val_t{primary_type->alignment()});
        m_object = nullptr;
        m_type = qualified_type_ptr{};
    }

    bool dyn_value::operator==(const dyn_value & i_source) const
    {
        if (m_type != i_source.m_type)
            return false;

        if (m_type.is_empty())
            return true;

        auto const primary_type = m_type.primary_type();

        if (!primary_type->is_comparable())
        {
            char err_message[512];
            to_chars(err_message, "The type ", primary_type->name(), " is not comparable");
            throw unsupported_error(err_message);
        }

        return m_type.primary_type()->compare_equal(m_object, i_source.m_object);
    }

} // namespace ediacaran