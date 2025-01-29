set(TargetName nm-gui-tool)

find_package(PkgConfig REQUIRED)
pkg_check_modules(gtkmm-4.0 REQUIRED IMPORTED_TARGET gtkmm-4.0)

find_package(fmt REQUIRED)

file(GLOB_RECURSE Sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE Headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_executable(${TargetName})
target_compile_features(${TargetName}
  PUBLIC cxx_std_20)
set_target_properties(${TargetName}
  PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)
target_compile_options(${TargetName}
  PRIVATE
    -Wall -Wextra -Wattributes -Wpedantic -Wstrict-aliasing -Wcast-align -g)
target_include_directories(${TargetName}
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${TargetName}
  PRIVATE
    PkgConfig::gtkmm-4.0 fmt::fmt-header-only)
target_sources(${TargetName}
  PRIVATE
    ${Sources} ${Headers})
