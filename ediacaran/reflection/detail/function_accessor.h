
#ifndef EDIACARAN_FUNC_REFL_CV_QUALFICATION
#error "This is a private header, do not include it"
#endif

template <typename OWNING_CLASS, typename RETURN_TYPE, typename... PARAMETER_TYPE>
struct MethodTraits<RETURN_TYPE (OWNING_CLASS::*)(PARAMETER_TYPE...)
                      EDIACARAN_FUNC_REFL_CV_QUALFICATION EDIACARAN_FUNC_REFL_NOEXCEPT_SPEC>
{
    constexpr static size_t           parameter_count = sizeof...(PARAMETER_TYPE);
    constexpr static cv_qualification qualification   = EDIACARAN_FUNC_REFL_CV_QUALFICATION_V;
};

template <
  typename OWNING_CLASS,
  typename RETURN_TYPE,
  typename... PARAMETER_TYPE,
  RETURN_TYPE (OWNING_CLASS::*METHOD)(PARAMETER_TYPE...)
    EDIACARAN_FUNC_REFL_CV_QUALFICATION EDIACARAN_FUNC_REFL_NOEXCEPT_SPEC,
  size_t... INDEX>
struct FunctionInvoker<
  RETURN_TYPE (OWNING_CLASS::*)(PARAMETER_TYPE...)
    EDIACARAN_FUNC_REFL_CV_QUALFICATION EDIACARAN_FUNC_REFL_NOEXCEPT_SPEC,
  METHOD,
  std::index_sequence<INDEX...>>
{
    using return_type                                                       = RETURN_TYPE;
    constexpr static array<parameter, sizeof...(PARAMETER_TYPE)> parameters = {
      parameter{get_qualified_type<PARAMETER_TYPE>()}...};

    static void
      func(void * i_dest_object, void * o_return_value_dest, const void * const * i_parameters)
    {
        EDIACARAN_ASSERT(o_return_value_dest != nullptr);
        auto & object = *static_cast<OWNING_CLASS *>(i_dest_object);
        new (o_return_value_dest) RETURN_TYPE(
          (object.*METHOD)(*static_cast<const PARAMETER_TYPE *>(i_parameters[INDEX])...));
    }
};

template <
  typename OWNING_CLASS,
  typename... PARAMETER_TYPE,
  void (OWNING_CLASS::*METHOD)(PARAMETER_TYPE...)
    EDIACARAN_FUNC_REFL_CV_QUALFICATION EDIACARAN_FUNC_REFL_NOEXCEPT_SPEC,
  size_t... INDEX>
struct FunctionInvoker<
  void (OWNING_CLASS::*)(PARAMETER_TYPE...)
    EDIACARAN_FUNC_REFL_CV_QUALFICATION EDIACARAN_FUNC_REFL_NOEXCEPT_SPEC,
  METHOD,
  std::index_sequence<INDEX...>>
{
    using return_type                                                       = void;
    constexpr static array<parameter, sizeof...(PARAMETER_TYPE)> parameters = {
      parameter{get_qualified_type<PARAMETER_TYPE>()}...};

    static void
      func(void * i_dest_object, void * /*o_return_value_dest*/, const void * const * i_parameters)
    {
        auto & object = *static_cast<OWNING_CLASS *>(i_dest_object);
        (object.*METHOD)(*static_cast<const PARAMETER_TYPE *>(i_parameters[INDEX])...);
    }
};

#undef EDIACARAN_FUNC_REFL_CV_QUALFICATION
#undef EDIACARAN_FUNC_REFL_CV_QUALFICATION_V
#undef EDIACARAN_FUNC_REFL_NOEXCEPT_SPEC