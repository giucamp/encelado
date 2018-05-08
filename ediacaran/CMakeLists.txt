cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 17)

include_directories(..)

#set (CMAKE_CXX_FLAGS "-fno-elide-constructors -Wall -Wextra -Wno-invalid-offsetof")
set (CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-invalid-offsetof")

add_library(ediacaran STATIC
	core/address.h
	core/array.h
	core/array_view.h
	core/byte_reader.h
	core/byte_writer.h
	core/char_reader.h
	core/char_writer.h
	core/comma_separated_names.h
	core/constexpr_string.h
	core/ediacaran_common.h
	core/expected.h
	core/remove_noexcept.h
	core/string_builder.h
	core/string_utils.h
	core/type_list.h
	core/byte_reader.cpp
	core/byte_writer.cpp
	core/char_reader.cpp
	core/char_writer.cpp
	core/string_builder.cpp
	reflection/detail/class_reflection.h
	reflection/detail/container_reflection.h
	reflection/detail/enum_reflection.h
	reflection/detail/function_accessor.h
	reflection/class_template_specialization.h
	reflection/class_type.h
	reflection/container.h
	reflection/cv_qualification.h
	reflection/enum_type.h
	reflection/function.h
	reflection/namespace.h
	reflection/parameter.h
	reflection/property.h
	reflection/qualified_type_ptr.h
	reflection/reflection.h
	reflection/special_functions.h
	reflection/type.h
	reflection/namespace.cpp
	reflection/qualified_type_ptr.cpp
	std_refl/allocator.h
	std_refl/list.h
	std_refl/string.h
	std_refl/vector.h
	utils/dyn_value.h
	utils/inspect.h
	utils/raw_ptr.h
	utils/universal_iterator.h
	utils/dyn_value.cpp
	utils/inspect.cpp
	utils/raw_ptr.cpp
	utils/universal_iterator.cpp

)
