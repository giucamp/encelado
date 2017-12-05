#pragma once
#include "ediacaran/reflection/type.h"
#include <string.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace ediacaran
{
    class namespace_ : public symbol_
    {
      public:
        using symbol_::symbol_;

        namespace_(const namespace_ &) = delete;
        namespace_ & operator=(const namespace_ &) = delete;


      private:
        std::vector<const type_t *> m_types;
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

        void register_type(const char * i_full_name, const type_t * i_type);

        void unregister_type(
          const char * i_full_name, const type_t * i_type) noexcept;

        const type_t * find_type(const string_view & i_full_name) const;

      private:
        global_namespace_();

      private:
        std::unordered_map<std::string, const type_t *> m_types;
    };

    bool try_parse(const type_t ** o_type_ptr, char_reader & i_source,
      char_writer & i_error) noexcept;
}