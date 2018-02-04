#pragma once
#include "ediacaran/reflection/type.h"
#include <string.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace ediacaran
{
    class namespace_
    {
      public:
        namespace_(const char * i_name) : m_name(i_name) {}

        constexpr string_view name() const noexcept
        {
            // workaround for gcc considering the comparison of two char* non-constexpr
            return string_view(m_name, string_view::traits_type::length(m_name));
        }

        namespace_(const namespace_ &) = delete;
        namespace_ & operator=(const namespace_ &) = delete;


      private:
        const char * const              m_name;
        std::vector<const type *>       m_types;
        std::vector<const namespace_ *> m_namespaces;
        friend class global_namespace_;
    };

    class global_namespace_ : public namespace_
    {
      public:
        static const global_namespace_ & get() noexcept { return edit(); }

        static global_namespace_ & edit() noexcept
        {
            static global_namespace_ s_instance;
            return s_instance;
        }

        void register_type(const char * i_full_name, const type * i_type);

        void unregister_type(const char * i_full_name, const type * i_type) noexcept;

        const type * find_type(const string_view & i_full_name) const;

      private:
        global_namespace_();

      private:
        std::unordered_map<std::string, const type *> m_types;
    };

    expected<void, parse_error> parse(const type ** o_type_ptr, char_reader & i_source) noexcept;
}