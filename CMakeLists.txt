cmake_minimum_required(VERSION 3.10)
project(AhunCardGenerator CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-g -O0 -fsanitize=address -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address)
endif()

add_compile_options(-Wall -Wextra -Wno-unused-parameter)

find_package(PkgConfig REQUIRED)

pkg_check_modules(CJSON REQUIRED libcjson)
pkg_check_modules(MAGICWAND REQUIRED MagickWand)
pkg_check_modules(LIBXML2 REQUIRED libxml-2.0)

add_definitions(
  -DMAGICKCORE_QUANTUM_DEPTH=16
  -DMAGICKCORE_HDRI_ENABLE=1
)

set(SOURCES
  src/main.cpp
  src/card.cpp
  src/cli.cpp
  src/parse_calendar.cpp
  src/template.cpp
)

add_executable(ahun_card_generator ${SOURCES})

target_link_libraries(ahun_card_generator
  ${MAGICWAND_LIBRARIES}
  ${LIBXML2_LIBRARIES}
  ${CJSON_LIBRARIES}
)

target_include_directories(ahun_card_generator PRIVATE
  ${MAGICWAND_INCLUDE_DIRS}
  ${LIBXML2_INCLUDE_DIRS}
  ${CJSON_INCLUDE_DIRS}
  include
)
