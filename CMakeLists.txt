cmake_minimum_required(VERSION 3.10)
project(AhunCardGenerator C)

set(CMAKE_C_STANDARD)

find_package(PkgConfig REQUIRED)

pkg_check_modules(CJSON REQUIRED libcjson)
pkg_check_modules(MAGICWAND REQUIRED MagickWand)
pkg_check_modules(LIBXML2 REQUIRED libxml-2.0)

add_definitions(
  -DMAGICKCORE_QUANTUM_DEPTH=16
  -DMAGICKCORE_HDRI_ENABLE=1
)

set(SOURCES
  src/main.c
  src/card.c
  src/cli.c
  src/list_templates.c
  src/parse_calendar.c
  src/template.c
)

add_executable(ahun_card_generator ${SOURCES})

target_link_libraries(ahun_card_generator
  ${MAGICWAND_LIBRARIES}
  ${LIBXML2_LIBRARIES}
  ${cjson_LIBRARIES}
)

target_include_directories(ahun_card_generator PRIVATE
  ${MAGICWAND_INCLUDE_DIRS}
  ${LIBXML2_INCLUDE_DIRS}
  ${cjson_INCLUDE_DIRS}
  include
)
