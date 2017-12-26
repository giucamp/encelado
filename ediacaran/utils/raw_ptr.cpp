#include <ediacaran/utils/raw_ptr.h>

namespace ediacaran
{
    raw_ptr raw_ptr::full_indirection() const noexcept
    {
        auto indirection_levels = m_type.indirection_levels();
        auto constness_word = m_type.constness_word();
        auto volatileness_word = m_type.volatileness_word();
        auto object = m_object;

        while (indirection_levels > 0 && object != nullptr)
        {
            indirection_levels--;
            constness_word >>= 1;
            volatileness_word >>= 1;
            object = *static_cast<void **>(object);
        }

        return raw_ptr(
          object, qualified_type_ptr(m_type.final_type(), indirection_levels, constness_word, volatileness_word));
    }

} // namespace ediacaran
