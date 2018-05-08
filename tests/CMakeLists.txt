cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 17)

include_directories(..)

#set (CMAKE_CXX_FLAGS "-fno-elide-constructors -Wall -Wextra -Wno-invalid-offsetof")
set (CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-invalid-offsetof")

add_executable(encelado_tests
	cambrian/serialization/test_types.cpp
	cambrian/serialization/test_types.h
	cambrian/serialization/tests.cpp
	cambrian/common_tests.cpp
	ediacaran/animalia.cpp
	ediacaran/class_templates_tests.cpp
	ediacaran/class_tests.cpp
	ediacaran/container_tests.cpp
	ediacaran/qualified_type_ptr_tests.cpp
	ediacaran/remove_noexcept_tests.cpp
	ediacaran/string_tests.cpp
	ediacaran/type_list_tests.cpp
	ediacaran/type_tests.cpp
	cambrian.cpp
	common.h
	main.cpp
)

target_link_libraries(encelado_tests
	ediacaran
	cambrian )
