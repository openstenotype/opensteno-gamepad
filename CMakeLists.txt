cmake_minimum_required(VERSION 2.8)
set(EXE_NAME opensteno)
project(${EXE_NAME})

find_package(GTK2 REQUIRED)
include_directories(${GTK2_INCLUDE_DIRS})
list(APPEND STENO_LIBRARIES ${GTK2_LIBRARIES})

if (CMAKE_VERSION VERSION_GREATER 2.8.5)
  include(FeatureSummary)
  feature_summary(WHAT ALL)
endif ()

add_executable(${EXE_NAME}
  source/Steno.cpp
  )

target_link_libraries(${EXE_NAME} ${STENO_LIBRARIES} )