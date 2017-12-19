
#include "../common.h"
#include "ediacaran/core/char_reader.h"
#include "ediacaran/core/char_writer.h"
#include "ediacaran/core/string_builder.h"
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace ediacaran_test
{
    void string_conversion_tests()
    {
        using namespace ediacaran;

        size_t const buffer_size = 1024 * 1024;
        auto const buff = std::make_unique<char[]>(buffer_size);
        memset(buff.get(), 7, buffer_size);
        char_writer out(buff.get(), buffer_size);

        using var = std::variant<bool, int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t, float,
          double, long double, char, std::basic_string<char>>;


        auto const seed = std::random_device{}();
        std::mt19937_64 mt{seed};
        auto rand = [&mt] { return std::uniform_int_distribution<unsigned long long>()(mt); };

        std::vector<var> objects;
        std::vector<const char *> delimiters;
        for (;;)
        {
            auto const next_char = out.next_dest();
            auto const op = rand() % 14;
            switch (op)
            {
            case 0:
            {
                auto const value = static_cast<int8_t>(rand());
                objects.push_back(value);
                out << value << ' ';
                break;
            }
            case 1:
            {
                auto const value = static_cast<int16_t>(rand());
                objects.push_back(value);
                out << value << ' ';
                break;
            }
            case 2:
            {
                auto const value = static_cast<int32_t>(rand());
                objects.push_back(value);
                out << value << ' ';
                break;
            }
            case 3:
            {
                auto const value = static_cast<int64_t>(rand());
                objects.push_back(value);
                out << value << ' ';
                break;
            }
            case 4:
            {
                auto const value = static_cast<uint8_t>(rand());
                objects.push_back(value);
                out << value << ' ';
                break;
            }
            case 5:
            {
                auto const value = static_cast<uint16_t>(rand());
                objects.push_back(value);
                out << value << ' ';
                break;
            }
            case 6:
            {
                auto const value = static_cast<uint32_t>(rand());
                objects.push_back(value);
                out << value << ' ';
                break;
            }
            case 7:
            {
                auto const value = static_cast<uint64_t>(rand());
                objects.push_back(value);
                out << value << ' ';
                break;
            }
            case 8:
            {
                auto const value = static_cast<float>(rand() & 0xFFFF) / 1.012345678f;
                objects.push_back(value);
                out << value << ' ';
                break;
            }
            case 9:
            {
                auto const value = static_cast<double>(rand() & 0xFFFF) / 1.012345678f;
                objects.push_back(value);
                out << value << ' ';
                break;
            }
            case 10:
            {
                auto const value = static_cast<long double>(rand() & 0xFFFF) / 1.012345678L;
                objects.push_back(value);
                out << value << ' ';
                break;
            }
            case 11:
            {
                auto const value = static_cast<char>('a' + rand() % 16);
                objects.push_back(value);
                out << value << ' ';
                break;
            }
            case 12:
            {
                auto const c = static_cast<char>('a' + rand() % 16);
                std::string const value(static_cast<size_t>(1 + rand() % 32), c);
                objects.push_back(value);
                out << value << ' ';
                break;
            }
            case 13:
            {
                auto const value = (rand() & 1) == 0;
                objects.push_back(value);
                out << value << ' ';
                break;
            }
            default:
                ENCELADO_TEST_ASSERT(false);
                break;
            }
            if (out.remaining_size() <= 0)
            {
                objects.pop_back();
                break;
            }
            ENCELADO_TEST_ASSERT(!std::isspace(*next_char));
            auto const start_of_buff = buff.get();
            auto const buff_len = strlen(start_of_buff);
            auto const end_of_buff = start_of_buff + buff_len;
            ENCELADO_TEST_ASSERT(end_of_buff == out.next_dest());
            delimiters.push_back(out.next_dest());
        }

        char_reader in(string_view(buff.get(), buffer_size));
        for (size_t index = 0; index < objects.size(); index++)
        {
            auto const & obj = objects[index];

            char random_input[256];
            std::generate(
              std::begin(random_input), std::end(random_input), [&rand] { return static_cast<char>(rand()); });
            random_input[255] = 0;

            std::visit(
              [&in, &obj, index, &delimiters, &random_input](const auto & i_value) {

                  using value_type = std::decay_t<decltype(i_value)>;

                  // try with the random input
                  if constexpr (!std::is_same_v<value_type, std::string> && !std::is_same_v<value_type, char>)
                  {
                      char_reader random_stream(random_input);
                      value_type val;
                      try_parse(val, random_stream);

                      random_stream = char_reader(random_input);
                      val = value_type{};
                      try_accept(val, random_stream);
                  }

                  // consume an object
                  auto const expected = std::get<value_type>(obj);
                  in >> expected >> spaces;
                  ENCELADO_TEST_ASSERT(delimiters[index] == in.next_chars());
              },
              obj);
        }
    }

    template <typename INT_TYPE, typename BIG_INT_TYPE> void typed_string_overflow_tests(bool i_negative = false)
    {
        using namespace ediacaran;

        char buff[1024], error_buffer[1024];

        BIG_INT_TYPE const max =
          i_negative ? std::numeric_limits<INT_TYPE>::min() : std::numeric_limits<INT_TYPE>::max();
        for (BIG_INT_TYPE number = max - 22; number <= max + 22; number++)
        {
            char_writer out(buff);
            out << number;

            error_buffer[0] = 0;
            INT_TYPE result = 0;
            char_reader in(buff);
            char_writer err(error_buffer);
            bool const res = try_parse(result, in, err);

            bool const expected_res = i_negative ? (number >= max) : (number <= max);
            ENCELADO_TEST_ASSERT(res == expected_res);
            if (res)
            {
                ENCELADO_TEST_ASSERT(result == number);
            }
            else
            {
                ENCELADO_TEST_ASSERT(strcmp(error_buffer, "integer overflow") == 0);
            }
        }
    }

    void string_overflow_tests()
    {
        typed_string_overflow_tests<int8_t, int16_t>();
        typed_string_overflow_tests<int16_t, int32_t>();
        typed_string_overflow_tests<int32_t, int64_t>();
        typed_string_overflow_tests<int8_t, int16_t>(true);
        typed_string_overflow_tests<int16_t, int32_t>(true);
        typed_string_overflow_tests<int32_t, int64_t>(true);
        typed_string_overflow_tests<uint8_t, uint16_t>();
        typed_string_overflow_tests<uint16_t, uint32_t>();
        typed_string_overflow_tests<uint32_t, uint64_t>();
    }

    void string_builder_tests()
    {
        using namespace ediacaran;

        string_builder builder;
        size_t const test_size = 5'000;
        size_t progress = 0;
        for (size_t j = 0; j < test_size; j += 1)
        {
            for (; progress < j; progress++)
                builder << progress << ' ' << std::to_string(progress) << ' ';

            auto string = builder.to_string();
            char_reader reader(string);
            for (size_t i = 0; i < j; i++)
            {
                reader >> std::as_const(i) >> ' ';
                reader >> std::as_const(i) >> ' ';
            }
        }
    }

    void string_basic_tests()
    {
        using namespace ediacaran;

        string_view const target("123 456 abc");
        auto str = to_string(123, ' ', 456, " abc");
        ENCELADO_TEST_ASSERT(target == str.c_str());

        char small_char_array[5];
        auto required = to_chars(small_char_array, 123, ' ', 456, " abc");
        ENCELADO_TEST_ASSERT(required == target.size() + 1);

        char big_char_array[32];
        required = to_chars(big_char_array, 123, ' ', 456, " abc");
        ENCELADO_TEST_ASSERT(required == target.size() + 1);
        ENCELADO_TEST_ASSERT(target == big_char_array);
    }

    void string_tests()
    {
        string_basic_tests();
        string_builder_tests();
        string_conversion_tests();
        string_overflow_tests();

        using namespace ediacaran;

        static_assert(has_try_accept_v<bool>);
        static_assert(has_try_parse_v<bool>);

        static_assert(has_try_accept_v<int8_t>);
        static_assert(has_try_parse_v<int8_t>);
        static_assert(has_try_accept_v<uint16_t>);
        static_assert(has_try_parse_v<uint16_t>);

        static_assert(has_try_accept_v<int16_t>);
        static_assert(has_try_parse_v<int16_t>);
        static_assert(has_try_accept_v<uint16_t>);
        static_assert(has_try_parse_v<uint16_t>);

        static_assert(has_try_accept_v<int32_t>);
        static_assert(has_try_parse_v<int32_t>);
        static_assert(has_try_accept_v<uint16_t>);
        static_assert(has_try_parse_v<uint16_t>);

        static_assert(has_try_accept_v<int64_t>);
        static_assert(has_try_parse_v<int64_t>);
        static_assert(has_try_accept_v<uint16_t>);
        static_assert(has_try_parse_v<uint16_t>);

        static_assert(has_try_accept_v<float>);
        static_assert(has_try_parse_v<float>);

        static_assert(has_try_accept_v<double>);
        static_assert(has_try_parse_v<double>);

        static_assert(has_try_accept_v<long double>);
        static_assert(has_try_parse_v<long double>);

        static_assert(!has_try_parse_v<SpacesTag>);
        static_assert(has_try_accept_v<SpacesTag>);

        static_assert(!has_try_parse_v<std::string>);
        static_assert(has_try_accept_v<std::string>);

        static_assert(!has_try_accept_v<void>);
        static_assert(!has_try_parse_v<void>);

        static_assert(parse<int>("42") == 42);
        static_assert(parse<int>("-42") == -42);
    }
}