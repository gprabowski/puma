set(MILLING_SIMULATOR_SOURCES
  main.cpp
  logger.cpp
  interpolator.cpp
  glfw_impl.cpp
  interpolator_scene.cpp
  inputs.cpp
  gui.cpp
  utils.cpp
)

add_executable(milling)


find_program( CLANG_TIDY_EXE NAMES "clang-tidy" DOC "Path to clang-tidy executable" ) 
if(NOT CLANG_TIDY_EXE) 
    message(STATUS "clang-tidy not found.") 
else() 
    message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}") 
    message(STATUS "clang-tidy turned off")
    #set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}" "-checks=*,modernize*") 
endif()

set_target_properties(milling PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON )

if(CLANG_TIDY_EXE) 
    set_target_properties(milling PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}" ) 
endif()

target_compile_options(milling PUBLIC -Werror -Wall)
target_compile_features(milling PUBLIC cxx_std_20)
target_sources(milling PUBLIC ${MILLING_SIMULATOR_SOURCES})

target_include_directories(milling
  PUBLIC
  ${CMAKE_SOURCE_DIR}/thirdparty
  ${CMAKE_SOURCE_DIR}/thirdparty/glm/glm
  ${CMAKE_SOURCE_DIR}/thirdparty/implot
  ${CMAKE_SOURCE_DIR}/thirdparty/glfw/include
  ${CMAKE_SOURCE_DIR}/thirdparty/spdlog/include
  ${CMAKE_SOURCE_DIR}/thirdparty/std_image
  ${CMAKE_SOURCE_DIR}/thirdparty/file_dialog
  ${CMAKE_SOURCE_DIR}/include
)

find_package(OpenMP)
target_link_libraries(milling
  glad
  spdlog
  glfw
  glm
  imgui
  implot
  file_dialog
)

if(OpenMP_CXX_FOUND)
    target_link_libraries(milling OpenMP::OpenMP_CXX)
endif()

add_custom_command(TARGET milling PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_SOURCE_DIR}/resources/ $<TARGET_FILE_DIR:milling>/../resources)
