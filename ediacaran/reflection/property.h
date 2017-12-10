#pragma once

#include "ediacaran/core/char_writer.h"
#include "ediacaran/reflection/qualified_type_ptr.h"
#include "ediacaran/reflection/type.h"

namespace ediacaran
{
    class property : public symbol_t
    {
      public:
        using getter = bool (*)(
          const void * i_source_object, const void * i_source_value, char_writer & o_error) = nullptr;
        using setter = bool (*)(void * i_dest_object, const void * i_source_value, char_writer & o_error) = nullptr;

        bool set(void * i_dest_object, const void * i_source_value, char_writer & o_error) noexcept {}

        bool get(const void * i_source_object, void * o_dest_value, char_writer & o_error) noexcept {}

      private:
        const qualified_type_ptr m_type;
        bool (*get)(const void * i_source_object, const void * i_source_value, char_writer & o_error) = nullptr;
        bool (*set)(void * i_dest_object, const void * i_source_value, char_writer & o_error) = nullptr;
    };

} // namespace ediacaran
