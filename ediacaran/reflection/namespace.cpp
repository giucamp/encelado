
#include "ediacaran/reflection/namespace.h"
#include "ediacaran/reflection/reflection.h"
#include <string>

namespace ediacaran
{
    global_namespace_::global_namespace_() : namespace_("")
    {
        register_type("void", &get_type<void>());

        register_type("int8", &get_type<int32_t>());
        register_type("int16", &get_type<int32_t>());
        register_type("int32", &get_type<int32_t>());
        register_type("int64", &get_type<int32_t>());

        register_type("uint8", &get_type<uint32_t>());
        register_type("uint16", &get_type<uint32_t>());
        register_type("uint32", &get_type<uint32_t>());
        register_type("uint64", &get_type<uint32_t>());

        register_type("float", &get_type<float>());
        register_type("double", &get_type<double>());
        register_type("long double", &get_type<long double>());
    }

    void global_namespace_::register_type(const char * i_full_name, const type * i_type)
    {
        std::string full_type_name{i_full_name};
        m_types.insert(std::make_pair(full_type_name, i_type));
    }

    void
      global_namespace_::unregister_type(const char * i_full_name, const type * /*i_type*/) noexcept
    {
        std::string full_type_name{i_full_name};

        auto const removed = m_types.erase(full_type_name);
        EDIACARAN_ASSERT(removed == 1);
    }

    const type * global_namespace_::find_type(const string_view & i_full_name) const
    {
        std::string full_type_name{i_full_name.data(), i_full_name.size()};
        auto const  it = m_types.find(full_type_name);
        if (it == m_types.end())
            return nullptr;
        else
            return it->second;
    }

    expected<void, parse_error> parse(const type ** o_type_ptr, char_reader & i_source) noexcept
    {
        try
        {
            auto const first_char = i_source.next_chars();

            bool some_identifier_found = false;
            auto curr_char             = first_char;
            for (;;)
            {
                // accept "::"
                if (*curr_char == ':')
                {
                    curr_char++;
                    if (*curr_char++ != ':')
                    {
                        return parse_error::missing_expected_chars;
                    }
                }

                // accept identifier
                if (is_alpha(*curr_char) || *curr_char == '_')
                {
                    curr_char++;
                    some_identifier_found = true;
                    while (is_alphanum(*curr_char) || *curr_char == '_')
                    {
                        curr_char++;
                    }
                }
                else
                    break;
            }

            if (!some_identifier_found)
            {
                return parse_error::missing_expected_chars;
            }

            string_view const full_name{first_char, static_cast<size_t>(curr_char - first_char)};
            auto const        type_ptr = global_namespace_::get().find_type(full_name);
            if (type_ptr == nullptr)
            {
                return parse_error::not_found;
            }
            i_source.skip(full_name.size());
            *o_type_ptr = type_ptr;
            return {};
        }
        catch (...)
        {
            return parse_error::unknown_error;
        }
    }
}