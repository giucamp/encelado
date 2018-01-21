
#include "../common.h"
#include "ediacaran/core/char_reader.h"
#include "ediacaran/core/char_writer.h"
#include "ediacaran/core/comma_separated_names.h"
#include "ediacaran/core/string_builder.h"
#include <algorithm>
#include <cstring>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace ediacaran_test
{
    template <typename TYPE, typename PRG, std::enable_if_t<std::is_integral_v<TYPE>> * = nullptr>
        inline TYPE uniform_rand(PRG & i_generator, const TYPE & i_min, const TYPE & i_max)
    {
        return std::uniform_int_distribution<TYPE>(i_min, i_max)(i_generator);
    }

    template <typename TYPE, typename PRG, std::enable_if_t<std::is_floating_point_v<TYPE>> * = nullptr>
        inline TYPE uniform_rand(PRG & i_generator, const TYPE & i_min, const TYPE & i_max)
    {
        return std::uniform_real_distribution<TYPE>(i_min, i_max)(i_generator);
    }

    template <typename TYPE, typename PRG>
        inline TYPE uniform_rand(PRG & i_generator)
    {
        return uniform_rand(i_generator, TYPE{}, std::numeric_limits<TYPE>::max());
    }

    class StringConversionTests
    {
    private:

        template <typename TEST>
            static auto make_maker()
        {
            return [](std::mt19937_64 & i_rand) -> std::unique_ptr<Test> { return std::make_unique<TEST>(i_rand); };
        }

        class Test
        {
        public:
            virtual void to_chars(ediacaran::char_writer & o_dest) const = 0;
            virtual void parse(ediacaran::char_reader & i_source) const = 0;
            virtual ~Test() = default;
        };

        template <typename TYPE>
            class TypedTest : public Test
        {
        public:
            TypedTest(std::mt19937_64 & io_rand)
                : m_value{uniform_rand<TYPE>(io_rand)}
            {

            }

            void to_chars(ediacaran::char_writer & o_dest) const override
            {
                o_dest << m_value;
            }

            void parse(ediacaran::char_reader & i_source) const override
            {
                i_source >> m_value;

                // generate a random string
                /*char random_input[256];
                std::generate(std::begin(random_input), std::end(random_input), [&rand] {
                    return static_cast<char>(rand());
                });
                random_input[255] = 0;

                // try to parse the random string
                char_reader random_stream(random_input);
                value_type  val;
                try_parse(val, random_stream);

                // try to accept the random string
                random_stream = char_reader(random_input);
                val = value_type{};
                try_accept(val, random_stream);*/
            }

        private:
            TYPE const m_value;
        };

        class CharTest : public Test
        {
        public:
            CharTest(std::mt19937_64 & io_rand)
                : m_value{ static_cast<char>('A' + uniform_rand<int>(io_rand, 0, 21)) }
                { }

            void to_chars(ediacaran::char_writer & o_dest) const override
            {
                o_dest << m_value;
            }

            void parse(ediacaran::char_reader & i_source) const override
            {
                i_source >> m_value;
            }

        private:
            char const m_value;
        };

        class StringTest : public Test
        {
        public:
            StringTest(std::mt19937_64 & io_rand)
                : m_value( uniform_rand<size_t>(io_rand, 1, 32),
                  'A' + static_cast<char>(uniform_rand<int>(io_rand, 0, 21)) )
            { }

            void to_chars(ediacaran::char_writer & o_dest) const override
            {
                o_dest << m_value;
            }

            void parse(ediacaran::char_reader & i_source) const override
            {
                i_source >> m_value;
            }

        private:
            std::string const m_value;
        };

        using TestMaker = std::unique_ptr<Test> (*)(std::mt19937_64 & i_random);

        std::vector<std::unique_ptr<Test>> m_tests;
        std::vector<TestMaker> const m_tests_makers;

    public:

        StringConversionTests()
            : m_tests_makers{
                make_maker<TypedTest<short>>(),
                make_maker<TypedTest<int>>(),
                make_maker<TypedTest<long>>(),
                make_maker<TypedTest<long long>>(),
                make_maker<TypedTest<short>>(),
                make_maker<TypedTest<unsigned int>>(),
                make_maker<TypedTest<unsigned long>>(),
                make_maker<TypedTest<unsigned long long>>(),
                make_maker<TypedTest<float>>(),
                make_maker<TypedTest<double>>(),
                make_maker<TypedTest<long double>>(),
                make_maker<CharTest>(),
                make_maker<StringTest>() }
        {
        }

        void run()
        {
            using namespace ediacaran;

            size_t const buffer_size = 1024 * 1024;
            auto const   buff = std::make_unique<char[]>(buffer_size);
            memset(buff.get(), 7, buffer_size);
            ediacaran::char_writer out(buff.get(), buffer_size);

            auto const      seed = std::random_device{}();
            std::mt19937_64 rand{ seed };

            ENCELADO_TEST_ASSERT(m_tests_makers.size() > 0);
            while (out.remaining_size() > 0)
            {
                auto const index = uniform_rand<size_t>(rand, 0, m_tests_makers.size() - 1);
                auto test = m_tests_makers[index](rand);

                test->to_chars(out);
                out << ' ';

                m_tests.push_back(std::move(test));
            }
            m_tests.pop_back();

            char_reader in(string_view(buff.get(), buffer_size));
            for (const auto & test : m_tests)
            {
                test->parse(in);
                in >> spaces;
            }
            m_tests.clear();
        }
    };

    void string_conversion_tests()
    {
        StringConversionTests tests;
        tests.run();
    }

    template <typename INT_TYPE, typename BIG_INT_TYPE>
    void typed_string_overflow_tests(bool i_negative = false)
    {
        using namespace ediacaran;

        char buff[1024], error_buffer[1024];

        BIG_INT_TYPE const max =
          i_negative ? std::numeric_limits<INT_TYPE>::min() : std::numeric_limits<INT_TYPE>::max();
        for (BIG_INT_TYPE number = max - 22; number <= max + 22; number++)
        {
            char_writer out(buff);
            out << number;

            error_buffer[0]    = 0;
            INT_TYPE    result = 0;
            char_reader in(buff);
            char_writer err(error_buffer);
            bool const  res = try_parse(result, in, err);

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
        size_t const   test_size = 5'000;
        size_t         progress  = 0;
        for (size_t j = 0; j < test_size; j += 1)
        {
            for (; progress < j; progress++)
                builder << progress << ' ' << std::to_string(progress) << ' ';

            auto        string = builder.to_string();
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
        auto              str = to_string(123, ' ', 456, " abc");
        ENCELADO_TEST_ASSERT(target == str.c_str());

        auto str_1 = to_string(std::make_tuple(1, 2, "abc", 3, 4, 'a'));
        ENCELADO_TEST_ASSERT(str_1 == "1, 2, abc, 3, 4, a");

        auto str_2 = to_string(std::make_pair(1, 2));
        ENCELADO_TEST_ASSERT(str_2 == "1, 2");

        char small_char_array[5];
        auto required = to_chars(small_char_array, 123, ' ', 456, " abc");
        ENCELADO_TEST_ASSERT(required == target.size() + 1);

        char big_char_array[32];
        required = to_chars(big_char_array, 123, ' ', 456, " abc");
        ENCELADO_TEST_ASSERT(required == target.size() + 1);
        ENCELADO_TEST_ASSERT(target == big_char_array);

        char        buff[10];
        char_writer writer(buff);
        for (int i = 0; i < 20; i++)
            writer << 'a';
        ENCELADO_TEST_ASSERT(strlen(buff) == 9);

        writer = char_writer(buff);
        for (int i = 0; i < 20; i++)
            writer << "abcd";
        ENCELADO_TEST_ASSERT(strlen(buff) == 9);

        static_assert(char_array_size('a') == 2);
        static_assert(char_array_size(33) == 3);

        constexpr auto fixed_array = to_char_array<3>(32);
        static_assert(fixed_array[0] == '3');
        static_assert(fixed_array[1] == '2');
        static_assert(fixed_array[2] == '\0');

        char fix_str_1[1];
        auto r = to_chars(fix_str_1, 1, " ", 2);
        ENCELADO_TEST_ASSERT(r == 4);
    }

    template <typename IT_1, typename IT2> constexpr size_t het_it_dist(IT_1 i_begin, IT2 i_end)
    {
        size_t count = 0;
        while (i_begin != i_end)
        {
            ++count;
            ++i_begin;
        }
        return count;
    }

    void string_comma_separated_names_tests()
    {
        using namespace ediacaran;

        {
            constexpr comma_separated_names names("");
            static_assert(het_it_dist(names.begin(), names.end()) == 0);
        }

        {
            constexpr comma_separated_names names("   ");
            static_assert(het_it_dist(names.begin(), names.end()) == 0);
        }

        {
            constexpr comma_separated_names names("a11");
            static_assert(*std::next(names.begin(), 0) == "a11");
            static_assert(het_it_dist(names.begin(), names.end()) == 1);
        }

        {
            constexpr comma_separated_names names("    uno    ,  due   ");
            static_assert(*std::next(names.begin(), 0) == "uno");
            static_assert(*std::next(names.begin(), 1) == "due");
            static_assert(het_it_dist(names.begin(), names.end()) == 2);
        }

        {
            constexpr comma_separated_names names("a, _b, c542_s_a");
            static_assert(*std::next(names.begin(), 0) == "a");
            static_assert(*std::next(names.begin(), 1) == "_b");
            static_assert(*std::next(names.begin(), 2) == "c542_s_a");
            static_assert(het_it_dist(names.begin(), names.end()) == 3);
        }

        {
            constexpr comma_separated_names names("aaaa,ba, ceee");
            static_assert(*std::next(names.begin(), 0) == "aaaa");
            static_assert(*std::next(names.begin(), 1) == "ba");
            static_assert(*std::next(names.begin(), 2) == "ceee");
            static_assert(het_it_dist(names.begin(), names.end()) == 3);
        }
    }

    void string_tests()
    {
        string_conversion_tests();
        string_basic_tests();
        string_builder_tests();
        string_overflow_tests();
        string_comma_separated_names_tests();

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
        static_assert(parse<bool>("true"));
        static_assert(!parse<bool>("false"));
    }
}
