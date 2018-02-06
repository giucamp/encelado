
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ediacaran/utils/raw_ptr.h>

namespace edi
{
    raw_ptr raw_ptr::full_indirection() const
    {
        auto indirection_levels = m_qualified_type.indirection_levels();
        auto constness_word     = m_qualified_type.constness_word();
        auto volatileness_word  = m_qualified_type.volatileness_word();
        auto object             = m_object;

        while (indirection_levels > 0)
        {
            if (object == nullptr)
                except<null_pointer_indirection>("Indirection of null pointer");

            indirection_levels--;
            constness_word >>= 1;
            volatileness_word >>= 1;
            object = *static_cast<void **>(object);
        }

        return raw_ptr(
          object,
          qualified_type_ptr(m_qualified_type.final_type(), 0, constness_word, volatileness_word));
    }

    raw_ptr raw_ptr::try_full_indirection() const noexcept
    {
        auto indirection_levels = m_qualified_type.indirection_levels();
        auto constness_word     = m_qualified_type.constness_word();
        auto volatileness_word  = m_qualified_type.volatileness_word();
        auto object             = m_object;

        while (indirection_levels > 0 && object != nullptr)
        {
            indirection_levels--;
            constness_word >>= 1;
            volatileness_word >>= 1;
            object = *static_cast<void **>(object);
        }

        return raw_ptr(
          object,
          qualified_type_ptr(
            m_qualified_type.final_type(), indirection_levels, constness_word, volatileness_word));
    }

    void raw_ptr::to_string(char_writer & o_dest) const noexcept
    {
        if (!m_qualified_type.is_empty())
        {
            size_t indirection_levels = m_qualified_type.indirection_levels();
            void * object             = m_object;
            while (indirection_levels > 0 && object != nullptr)
            {
                indirection_levels--;
                object = *static_cast<void **>(object);
                o_dest << '*';
            }
            if (object == nullptr)
            {
                o_dest << "{null}";
            }
            else
            {
                EDIACARAN_INTERNAL_ASSERT(indirection_levels == 0);
                auto const & type = *m_qualified_type.final_type();
                if (type.is_stringizable())
                {
                    type.stringize(object, o_dest);
                }
                else
                {
                    o_dest << "the type " << type.name() << " is not stringizable";
                }
            }
        }
    }

    void raw_ptr::to_string(std::string & o_dest) const
    {
        auto const min_size = sizeof(void *) * 4;
        if (o_dest.size() < min_size)
        {
            o_dest.resize(min_size);
        }

        char_writer writer(o_dest.data(), o_dest.size() + 1);
        to_string(writer);

        if (writer.remaining_size() < 0)
        {
            o_dest.resize(o_dest.size() - writer.remaining_size());
            writer = char_writer(o_dest.data(), o_dest.size() + 1);
            to_string(writer);
            EDIACARAN_INTERNAL_ASSERT(writer.remaining_size() >= 0);
        }
        EDIACARAN_INTERNAL_ASSERT(o_dest.size() >= static_cast<size_t>(writer.remaining_size()));
        o_dest.resize(o_dest.size() - static_cast<size_t>(writer.remaining_size()));
    }

} // namespace edi
