
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ediacaran/utils/dyn_value.h>
#include <memory>
#include <utility>

namespace edi
{
    dyn_value::dyn_value(const qualified_type_ptr & i_qualified_type) { assign(i_qualified_type); }

    void dyn_value::assign(const qualified_type_ptr & i_qualified_type)
    {
        if (!i_qualified_type.is_empty())
        {
            auto const final_type = i_qualified_type.final_type();
            if (!final_type->is_copy_constructible())
            {
                except<unsupported_error>("The type ", final_type->name(), " is not constructible");
            }
            manual_construct(
              i_qualified_type, [=](void * i_dest) { final_type->construct(i_dest); });
        }
    }

    dyn_value::dyn_value(const raw_ptr & i_source)
    {
        auto & type = i_source.qualified_type();
        if (!type.is_empty())
        {
            auto const final_type = type.final_type();
            if (!final_type->is_copy_constructible())
            {
                except<unsupported_error>(
                  "The type ", final_type->name(), " is not copy-constructible");
            }

            EDIACARAN_INTERNAL_ASSERT(final_type->is_destructible());

            manual_construct(
              type, [&](void * i_dest) { final_type->copy_construct(i_dest, i_source.object()); });
        }
    }

    void * dyn_value::uninitialized_allocate(const qualified_type_ptr & i_qualified_type)
    {
        EDIACARAN_ASSERT(!i_qualified_type.is_empty());

        if (i_qualified_type.is_pointer())
        {
            except<unsupported_error>(
              "dyn_value does not support pointer types (", i_qualified_type, ")");
        }

        auto const final_type = i_qualified_type.final_type();
        if (!final_type->is_destructible())
        {
            except<unsupported_error>("The type ", final_type->name(), " is not destructible");
        }

        auto const prev_primary_type = m_qualified_type.final_type();
        if (
          prev_primary_type == nullptr || final_type->size() != prev_primary_type->size() ||
          final_type->alignment() != prev_primary_type->alignment())
        {
            auto const buffer = operator new (
              final_type->size(), std::align_val_t{final_type->alignment()});
            if (!empty())
                destroy();
            m_object = buffer;
        }
        else
        {
            prev_primary_type->destroy(m_object);
        }
        m_qualified_type = i_qualified_type;
        return m_object;
    }

    void dyn_value::uninitialized_deallocate() noexcept
    {
        EDIACARAN_INTERNAL_ASSERT(m_object != nullptr);
        auto const final_type = m_qualified_type.final_type();
#if __cpp_sized_deallocation
        operator delete (m_object, final_type->size(), std::align_val_t{final_type->alignment()});
#else
        operator delete (m_object, std::align_val_t{final_type->alignment()});
#endif
        m_object         = nullptr;
        m_qualified_type = qualified_type_ptr{};
    }

    bool dyn_value::operator==(const dyn_value & i_source) const
    {
        if (m_qualified_type != i_source.m_qualified_type)
            return false;

        if (m_qualified_type.is_empty())
            return true;

        auto const final_type = m_qualified_type.final_type();

        if (!final_type->is_comparable())
        {
            except<unsupported_error>("The type ", final_type->name(), " is not comparable");
        }

        return m_qualified_type.final_type()->compare_equal(m_object, i_source.m_object);
    }

    dyn_value parse_value(const qualified_type_ptr & i_qualified_type, char_reader & i_source)
    {
        dyn_value result;
        result.manual_construct(i_qualified_type, [&](void * i_dest) {
            auto const result = i_qualified_type.final_type()->parse(i_dest, i_source);
            result.on_error_except();
        });
        return result;
    }

    dyn_value parse_value(const qualified_type_ptr & i_qualified_type, const string_view & i_source)
    {
        char_reader source(i_source);
        dyn_value   result = parse_value(i_qualified_type, source);
        if (source.remaining_chars() != 0)
            throw parse_error::tailing_chars;
        return result;
    }

} // namespace edi
