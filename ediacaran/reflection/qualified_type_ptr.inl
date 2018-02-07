
namespace edi
{
	namespace detail
	{
		template <typename TYPE>
		class StaticQualification;

		// generic template for non pointer, non const and non volatile types
		template <typename TYPE> class StaticQualification
		{
		public:

			using UnderlyingType = TYPE; /* it's the final type, that is the type with maximum
										 indirection level. For example the following types have all 'int' as final
										 type: 'int', 'int &', 'int * * * const * const &'.
										 C++ language remainder: a reference is like a const pointer, that is 'int &' is
										 the same as 'int * const', except that the first exposes directly its last
										 level of indirection. */

			static const size_t s_indirection_levels = 0; /* the number of indirection levels,
														  which is zero for non-pointers, 1 for pointers, 2 for pointers to pointers,
														  and so on. */

			static const size_t s_constness_word = 0; /* specifies, for each indirection
													  level,  the constness of the type.
													  - 'int', 'int**', "int&": no const is present, constness_word is zero.
													  - 'int * * const': the first indirection level is const (that is, the
													  pointer cannot be assigned after initialization), so the
													  constness_word is 1 (only bit 0 set).
													  - 'const int ********': only the last indirection level (8) is const,
													  so the 8th bit of the constness_word is 1 (constness_word=256).
													  C++ language remainder: the const keyword can be prefix or postfix to
													  the type it applies to: 'const int *' and 'int const *' are the
													  same declarations. */

			static const size_t s_volatileness_word = 0;

			static const bool s_is_lvalue_reference = false;
			static const bool s_is_rvalue_reference = false;

		private:

		};

		// partial specialization for lvalue reference types (they are threated like const pointers)
		template <typename TYPE> class StaticQualification<TYPE&>
		{
		public:
			using UnderlyingType = typename StaticQualification<TYPE>::UnderlyingType;
			static const size_t s_indirection_levels = StaticQualification<TYPE>::s_indirection_levels + 1;
			static const size_t s_constness_word = 1 | (StaticQualification<TYPE>::s_constness_word << 1);
			static const size_t s_volatileness_word = StaticQualification<TYPE>::s_volatileness_word << 1;
			static const bool s_is_lvalue_reference = true;
			static const bool s_is_rvalue_reference = false;
		};

		// partial specialization for rvalue reference types
		template <typename TYPE> class StaticQualification<TYPE&&>
		{
		public:
			using UnderlyingType = typename StaticQualification<TYPE>::UnderlyingType;
			static const size_t s_indirection_levels = StaticQualification<TYPE>::s_indirection_levels + 1;
			static const size_t s_constness_word = 1 | (StaticQualification<TYPE>::s_constness_word << 1);
			static const size_t s_volatileness_word = StaticQualification<TYPE>::s_volatileness_word << 1;
			static const bool s_is_lvalue_reference = false;
			static const bool s_is_rvalue_reference = true;
		};

		// partial specialization for pointer types
		template <typename TYPE> class StaticQualification<TYPE*>
		{
		public:
			using UnderlyingType = typename  StaticQualification<TYPE>::UnderlyingType;
			static const size_t s_indirection_levels = StaticQualification<TYPE>::s_indirection_levels + 1;
			static const size_t s_constness_word = StaticQualification<TYPE>::s_constness_word << 1;
			static const size_t s_volatileness_word = StaticQualification<TYPE>::s_volatileness_word << 1;
			static const bool s_is_lvalue_reference = StaticQualification<TYPE>::s_is_lvalue_reference;
			static const bool s_is_rvalue_reference = StaticQualification<TYPE>::s_is_rvalue_reference;
		};

		// partial specialization for const types
		template <typename TYPE> class StaticQualification<const TYPE>
		{
		public:
			using UnderlyingType = typename StaticQualification<TYPE>::UnderlyingType;
			static const size_t s_indirection_levels = StaticQualification<TYPE>::s_indirection_levels;
			static const size_t s_constness_word = StaticQualification<TYPE>::s_constness_word | 1;
			static const size_t s_volatileness_word = StaticQualification<TYPE>::s_volatileness_word;
			static const bool s_is_lvalue_reference = StaticQualification<TYPE>::s_is_lvalue_reference;
			static const bool s_is_rvalue_reference = StaticQualification<TYPE>::s_is_rvalue_reference;
		};

		// partial specialization for volatile types
		template <typename TYPE> class StaticQualification<volatile TYPE>
		{
		public:
			using UnderlyingType = typename  StaticQualification<TYPE>::UnderlyingType;
			static const size_t s_indirection_levels = StaticQualification<TYPE>::s_indirection_levels;
			static const size_t s_constness_word = StaticQualification<TYPE>::s_constness_word;
			static const size_t s_volatileness_word = StaticQualification<TYPE>::s_volatileness_word | 1;
			static const bool s_is_lvalue_reference = StaticQualification<TYPE>::s_is_lvalue_reference;
			static const bool s_is_rvalue_reference = StaticQualification<TYPE>::s_is_rvalue_reference;
		};

		// partial specialization for const volatile types
		template <typename TYPE> class StaticQualification<const volatile TYPE>
		{
		public:
			using UnderlyingType = typename  StaticQualification<TYPE>::UnderlyingType;
			static const size_t s_indirection_levels = StaticQualification<TYPE>::s_indirection_levels;
			static const size_t s_constness_word = StaticQualification<TYPE>::s_constness_word | 1;
			static const size_t s_volatileness_word = StaticQualification<TYPE>::s_volatileness_word | 1;
			static const bool s_is_lvalue_reference = StaticQualification<TYPE>::s_is_lvalue_reference;
			static const bool s_is_rvalue_reference = StaticQualification<TYPE>::s_is_rvalue_reference;
		};
	}

	constexpr qualified_type_ptr::qualified_type_ptr() noexcept
		: m_final_type(nullptr), m_indirection_levels(0), m_constness_word(0), m_volatileness_word(0)
	{

	}

    constexpr qualified_type_ptr::qualified_type_ptr(const type * i_final_type,
      size_t i_indirection_levels, size_t i_constness_word,
      size_t i_volatileness_word) noexcept
        : m_final_type(i_final_type),
          m_indirection_levels(i_indirection_levels),
          m_constness_word(i_constness_word),
          m_volatileness_word(i_volatileness_word)
    {
        EDIACARAN_INTERNAL_ASSERT(
          i_indirection_levels <= s_max_indirection_levels);
        EDIACARAN_INTERNAL_ASSERT(
          (i_constness_word & ~((2 << i_indirection_levels) - 1)) == 0);
        EDIACARAN_INTERNAL_ASSERT(
          (i_volatileness_word & ~((2 << i_indirection_levels) - 1)) == 0);
    }

    constexpr qualified_type_ptr::qualified_type_ptr(const type & i_final_type,
      const cv_qualification * i_cv_flags, size_t i_cv_flags_size)
        : m_final_type(&i_final_type), m_indirection_levels(0), m_constness_word(0), m_volatileness_word(0)
    {
        if (i_cv_flags_size > s_max_indirection_levels)
        {
            except<std::runtime_error>("The number of indirection levels (", i_cv_flags_size, ") exceeds "
                                     "s_max_indirection_levels (", s_max_indirection_levels, ")");
        }
        uintptr_t constness_word = 0, volatileness_word = 0;

        for (size_t index = 0; index < i_cv_flags_size; index++)
        {
            cv_qualification flags = i_cv_flags[index];
            if ((flags & cv_qualification::const_q) != cv_qualification::no_q)
            {
                constness_word |= uintptr_t(1) << index;
            }

            if ((flags & cv_qualification::volatile_q) != cv_qualification::no_q)
            {
                volatileness_word |= uintptr_t(1) << index;
            }
        }
        m_constness_word = constness_word;
        m_volatileness_word = volatileness_word;
        m_indirection_levels = i_cv_flags_size > 0 ? i_cv_flags_size - 1 : 0;

        EDIACARAN_INTERNAL_ASSERT(m_constness_word == constness_word &&
                                  m_volatileness_word == volatileness_word);
    }

    // forward
    constexpr const type & get_ptr_type() noexcept;

	constexpr const type * qualified_type_ptr::primary_type() const noexcept
	{
        if (m_indirection_levels == 0)
		{
			// the pointer is empty or it is not a pointer
			return m_final_type;
		}
		else
		{
			// the type is a pointer
			EDIACARAN_INTERNAL_ASSERT(m_final_type != nullptr);
			return &get_ptr_type();
		}
	}

	constexpr bool qualified_type_ptr::is_const(size_t i_indirection_level) const noexcept
	{
		EDIACARAN_ASSERT(i_indirection_level <= indirection_levels());
		return (m_constness_word & (static_cast<uintptr_t>(1) << i_indirection_level)) != 0;
	}

	constexpr bool qualified_type_ptr::is_volatile(size_t i_indirection_level) const noexcept
	{
		EDIACARAN_ASSERT(i_indirection_level <= indirection_levels());
		return (m_volatileness_word & (static_cast<uintptr_t>(1) << i_indirection_level)) != 0;
	}
}
