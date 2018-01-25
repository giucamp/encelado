#pragma once

#include "ediacaran/core/array_view.h"
#include "ediacaran/core/ediacaran_common.h"
#include "ediacaran/core/type_list.h"
#include "ediacaran/reflection/function.h"
#include "ediacaran/reflection/property.h"
#include "ediacaran/reflection/type.h"
#include <cstddef>

namespace ediacaran
{
    class class_type;

    struct base_class
    {
      public:
        template <typename DERIVED, typename BASE> constexpr static base_class make() noexcept;

        void * up_cast(void * i_derived) const noexcept { return (*m_up_caster)(i_derived); }

        const void * up_cast(const void * i_derived) const noexcept
        {
            return (*m_up_caster)(const_cast<void *>(i_derived));
        }

        constexpr class_type const & get_class() const noexcept { return m_class; }

      private:
        constexpr base_class(class_type const & i_class, void * (*i_up_caster)(void *)noexcept)
            : m_class(i_class), m_up_caster(i_up_caster)
        {
        }

        template <typename DERIVED, typename BASE>
        static void * impl_up_cast(void * i_derived) noexcept
        {
            auto const derived = static_cast<DERIVED *>(i_derived);
            EDIACARAN_ASSERT(derived != nullptr);
            return static_cast<BASE *>(derived);
        }

      private:
        class_type const & m_class;
        void * (*const m_up_caster)(void *)noexcept;
    };

    class class_type : public type
    {
      public:
        constexpr class_type(
          const char * const                   i_name,
          size_t                               i_size,
          size_t                               i_alignment,
          const special_functions &            i_special_functions,
          const array_view<const base_class> & i_base_classes,
          const array_view<const property> &   i_properties,
          const array_view<const function> &   i_functions)
            : type(type_kind::is_class, i_name, i_size, i_alignment, i_special_functions),
              m_base_classes(i_base_classes), m_properties(i_properties), m_functions(i_functions)
        {
            check_duplicates();
        }

        constexpr array_view<const base_class> const & bases() const noexcept
        {
            return m_base_classes;
        }

        constexpr array_view<const property> const & properties() const noexcept
        {
            return m_properties;
        }

        constexpr array_view<const function> const & functions() const noexcept
        {
            return m_functions;
        }

      private:
        constexpr void check_duplicates() const
        {
            for (size_t i = 0; i < m_properties.size(); i++)
            {
                for (size_t j = i + 1; j < m_properties.size(); j++)
                {
                    if (m_properties[i].name() == m_properties[j].name())
                    {
                        except<std::runtime_error>(
                          "duplicate property ", m_properties[i].name(), " in class ", name());
                    }
                }

                for (size_t base_class_index = 0; base_class_index < m_base_classes.size();
                     base_class_index++)
                {
                    auto & base_props = m_base_classes[base_class_index].get_class().m_properties;
                    for (size_t j = 0; j < base_props.size(); j++)
                    {
                        if (m_properties[i].name() == base_props[j].name())
                        {
                            except<std::runtime_error>(
                              "shadowing property ",
                              m_properties[i].name(),
                              " in class ",
                              name(),
                              ", already in ",
                              m_base_classes[base_class_index].get_class().name());
                        }
                    }
                }
            }
        }

      private:
        array_view<const base_class> const m_base_classes;
        array_view<const property> const   m_properties;
        array_view<const function> const   m_functions;
    };

    // forward
    template <typename TYPE> constexpr const class_type & get_class_type() noexcept;

    template <typename DERIVED, typename BASE> constexpr base_class base_class::make() noexcept
    {
        return base_class(get_class_type<BASE>(), &impl_up_cast<DERIVED, BASE>);
    }

} // namespace ediacaran
