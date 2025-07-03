cmake_minimum_required(VERSION 3.10)
add_executable (LargeTextAnalysis "src/LargeTextAnalysis.cpp" "src/LargeTextAnalysis.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET LargeTextAnalysis PROPERTY CXX_STANDARD 20)
endif()


target_include_directories(LargeTextAnalysis PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../lib/bcppul/include)
add_library(BCPPUL_IMPORTED STATIC IMPORTED)
set_target_properties(BCPPUL_IMPORTED PROPERTIES
    IMPORTED_LOCATION_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/../lib/bcppul/lib/bcppul_static_debug.lib"
    IMPORTED_LOCATION_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../lib/bcppul/lib/bcppul_static_release.lib"
)
target_link_libraries(LargeTextAnalysis PRIVATE BCPPUL_IMPORTED)
