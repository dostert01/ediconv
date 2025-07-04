cmake_minimum_required(VERSION 3.31)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

project(ediconv)

set(CMAKE_CXX_STANDARD 20)

add_executable(ediconv main.cpp
        EdiFile.cpp
)
target_link_libraries(ediconv PUBLIC pugixml)

add_subdirectory(_deps/googletest)
add_subdirectory(_deps/pugixml)
enable_testing()
add_subdirectory(tests)
