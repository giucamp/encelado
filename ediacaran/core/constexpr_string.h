#pragma once
#include "ediacaran/core/char_writer.h"

namespace ediacaran
{
    template <typename WRITER_FUNC> struct constexpr_string
    {
        constexpr static size_t get_length() noexcept
        {
            char_writer writer;
            WRITER_FUNC()(writer);
            return writer.input_size();
        }

        constexpr static size_t length = get_length();

        constexpr static std::array<char, length + 1> get_string() noexcept
        {
            std::array<char, length + 1> name = {};
            char_writer writer(name.data(), name.size());
            WRITER_FUNC()(writer);
            return name;
        }

        constexpr static std::array<char, length + 1> string = get_string();
    };
}