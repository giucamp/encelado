
#include <ediacaran/utils/dyn_value.h>
#include <memory>
#include <utility>

namespace ediacaran
{
    void * dyn_value::uninitialized_allocate(const qualified_type_ptr & i_type)
    {
        if(!empty())
            destroy();

        auto const primary_type = i_type.primary_type();

        if(!primary_type->is_destructible())
        {
            char err_message[512];
            to_chars(err_message, "The type ", primary_type->name(), " is not destructible");
            throw unsupported_error(err_message);
        }

        auto const buffer = operator new (primary_type->size(), std::align_val_t{primary_type->alignment()});
        m_type = i_type;
        return m_object = buffer;
    }

    dyn_value::dyn_value(const raw_ptr & i_source)
    {
        auto & type = i_source.type();
        if(!type.is_empty())
        {
            auto const primary_type = type.primary_type();
            if(!primary_type->is_copy_constructible())
            {
                char err_message[512];
                to_chars(err_message, "The type ", primary_type->name(), " is not copy-constructible");
                throw unsupported_error(err_message);
            }

            EDIACARAN_INTERNAL_ASSERT(primary_type->is_destructible());

            uninitialized_allocate(type);

            try
            {
                primary_type->copy_construct(m_object, i_source.object());
            }
            catch(...)
            {
                deallocate();
                throw;
            }
        }
    }

    bool dyn_value::operator == (const dyn_value & i_source) const
    {
        if(m_type != i_source.m_type)
            return false;

        if(m_type.is_empty())
            return true;

        auto const primary_type = m_type.primary_type();

        if(!primary_type->is_comparable())
        {
            char err_message[512];
            to_chars(err_message, "The type ", primary_type->name(), " is not comparable");
            throw unsupported_error(err_message);
        }

        return m_type.primary_type()->compare_equal(m_object, i_source.m_object);
    }

} // namespace ediacaran