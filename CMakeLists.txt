include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
message(STATUS "Local .env.cmake: ${LOCAL_ENV}")

cmake_minimum_required(VERSION 3.11.0)
 
set(NAME Ethereal-Engine)
 
message(STATUS "using ${CMAKE_GENERATOR}")
if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
    set(USE_MINGW "True")
    set(CMAKE_C_COMPILER gcc.exe)
    set(CMAKE_CXX_COMPILER g++.exe)
endif()
 
project(${NAME} VERSION 1.0.0)

# Set VULKAN_SDK_PATH in .env.cmake to target specific vulkan version
if (DEFINED VULKAN_SDK_PATH)
    set(VULKAN_INCLUDE_DIRS "${VULKAN_SDK_PATH}/Include")
    set(VULKAN_LIBRARIES "${VULKAN_SDK_PATH}")
    set(VULKAN_FOUND "True")
else()
    set(VULKAN_INCLUDE_DIRS "${VCPKG_INCLUDE_DIR}/vulkan")
    set(VULKAN_LIBRARIES "${VCPKG_LIB_DIR}")
endif()

# 2. Set GLFW_PATH in .env.cmake to target specific glfw
if (DEFINED GLFW_MSVC_PATH)
  message(STATUS "Using GLFW path specified in .env")
  set(GLFW_INCLUDE_DIRS "${VCPKG_INCLUDE_DIR}")
  if (MSVC)
    set(GLFW_LIB "${GLFW_MSVC_LIB_PATH}/lib") # 2.1 Update lib-vc2022 to use same version as your visual studio, can be an issue with glfw.lib naming
  elseif (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
    message(STATUS "USING MINGW")
    set(GLFW_LIB "${VCPKG_LIB_DIR}/lib") # 2.2 make sure matches glfw mingw subdirectory
  endif()
else()
  set(GLFW_LIB glfw)
endif()
if (NOT GLFW_LIB)
    message(FATAL_ERROR "Could not find glfw library!")
else()
    message(STATUS "Using glfw lib at: ${GLFW_LIB}")
endif()


# Addtional preparations for compilation
include_directories(external)

message(STATUS ${PROJECT_SOURCE_DIR}/src)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})
 
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
 
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

# Compiling sources
if (WIN32)
  message(STATUS "CREATING BUILD FOR WINDOWS")
 
  if (USE_MINGW)
    target_include_directories(${PROJECT_NAME} PUBLIC
      ${MINGW_PATH}/include
    )
    target_link_directories(${PROJECT_NAME} PUBLIC
      ${MINGW_PATH}/lib
    )
  endif()
 set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src)
  target_include_directories(${PROJECT_NAME} PUBLIC
    "${PROJECT_SOURCE_DIR}/src"
    ${VCPKG_INCLUDE_DIR}
    ${VULKAN_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    ${GLM_PATH}
    )
 message(STATUS "${PROJECT_SOURCE_DIR}/src" )
  target_link_directories(${PROJECT_NAME} PUBLIC
    ${VCPKG_LIB_DIR}
    ${VCPKG_BIN_DIR}
    ${GLFW_MSVC_LIB_PATH}
  )
  target_link_libraries(${PROJECT_NAME} glfw3 vulkan-1)
elseif (UNIX)
    message(STATUS "CREATING BUILD FOR UNIX")
    target_include_directories(${PROJECT_NAME} PUBLIC
      ${PROJECT_SOURCE_DIR}/src
    )
    target_link_libraries(${PROJECT_NAME} glfw3 ${VULKAN_LIBRARIES})
endif()


# Copying DLLs to build directory
set(DLL_FILES
glfw3.dll
vulkan-1.dll
)

set(BUILD_DIR "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")

foreach(DLL ${DLL_FILES})
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/${VCPKG_BIN_DIR}/${DLL}"
        "${PROJECT_SOURCE_DIR}/build"
        COMMENT "Copying ${DLL} into ${BUILD_DIR}"
    )
endforeach()

############## Build SHADERS #######################
 
# Find all vertex and fragment sources within shaders directory
# taken from VBlancos vulkan tutorial
# https://github.com/vblanco20-1/vulkan-guide/blob/all-chapters/CMakeLists.txt
find_program(GLSL_VALIDATOR glslangValidator HINTS
  ${BIN_DIR}
  ${VCPKG_BIN_DIR}
  /usr/bin
  /usr/local/bin
)

# get all .vert and .frag files in shaders directory
file(GLOB_RECURSE GLSL_SOURCE_FILES
  "${PROJECT_SOURCE_DIR}/shaders/*.frag"
  "${PROJECT_SOURCE_DIR}/shaders/*.vert"
)
 
foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders
    DEPENDS ${SPIRV_BINARY_FILES}
)