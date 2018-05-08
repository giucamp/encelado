cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 17)

include_directories(..)

#set (CMAKE_CXX_FLAGS "-fno-elide-constructors -Wall -Wextra -Wno-invalid-offsetof")
set (CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-invalid-offsetof")

add_library(cambrian STATIC
    data/directory.cpp
    data/directory.h
    data/path.cpp
    data/path.h
    data/serializer.cpp
    data/serializer.h
    data/type_registry.cpp
    data/type_registry.h
    storage/file_device.cpp
    storage/file_device.h
    storage/memory_device.cpp
    storage/memory_device.h
    storage/storage_device.cpp
    storage/storage_device.h
    cambrian_common.h
)
