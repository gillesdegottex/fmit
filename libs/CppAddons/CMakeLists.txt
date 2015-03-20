SET(libCppAddons_SRCS
    CAMath.cpp
    Random.cpp
)

INCLUDE_DIRECTORIES(
    ../../libs
    ${CMAKE_CURRENT_BINARY_DIR}
)

set(CMAKE_CXX_FLAGS "-Wall -ansi -pedantic")

add_library(CppAddons STATIC ${libCppAddons_SRCS})
