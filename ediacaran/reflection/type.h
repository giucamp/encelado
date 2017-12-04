
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2016-2017.


#pragma once
#include "ediacaran/core/ediacaran_common.h"
#include "ediacaran/reflection/special_functions.h"

namespace ediacaran
{
    class symbol_
    {
      public:
        constexpr symbol_(const char * const i_name) : m_name(i_name) {}
        constexpr const char * name() const noexcept { return m_name; }

      private:
        const char * const m_name;
    };

    class type_t : public symbol_
    {
      public:
        constexpr type_t(const char * const i_name, size_t i_size,
          size_t i_alignment,
          const ediacaran::special_functions & i_special_functions)
            : symbol_(i_name), m_size(i_size), m_alignment(i_alignment),
              m_special_functions(i_special_functions)
        {
        }

        constexpr size_t size() const noexcept { return m_size; }

        constexpr size_t alignment() const noexcept { return m_alignment; }

        constexpr const special_functions & special_functions() const noexcept
        {
            return m_special_functions;
        }

      private:
        size_t const m_size;
        size_t const m_alignment;
        ediacaran::special_functions m_special_functions;
    };
}
