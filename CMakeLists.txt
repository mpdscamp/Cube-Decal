cmake_minimum_required(VERSION 3.10)
project(RotatingCubeDecal VERSION 1.0 LANGUAGES CXX)

# Set C++ standard to C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Check if source directory exists
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src")
    message(FATAL_ERROR "Source directory '${CMAKE_CURRENT_SOURCE_DIR}/src' not found!")
endif()

# Define source files explicitly (now with our simplified structure)
set(SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Math.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Image.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Cube.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Logger.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ConfigManager.cpp"
)

# Verify source files exist
foreach(SOURCE_FILE ${SOURCES})
    if(NOT EXISTS "${SOURCE_FILE}")
        message(WARNING "Source file does not exist: ${SOURCE_FILE}")
    endif()
endforeach()

# Check if include directory exists
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include")
    message(FATAL_ERROR "Include directory '${CMAKE_CURRENT_SOURCE_DIR}/include' not found!")
endif()

# Define header files
file(GLOB HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

# Add include directory
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

# Define executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Define mathematical constants
target_compile_definitions(${PROJECT_NAME} PRIVATE _USE_MATH_DEFINES)

# Compiler flags
if(MSVC)
    # MSVC flags
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    # GCC/Clang flags
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

# Set build-specific flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Create output directories
set_target_properties(${PROJECT_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Add a custom target to create output directories
add_custom_target(create_output_dirs ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/bin/frames"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/bin/data"
    COMMENT "Creating output directories"
)

# Create dependency
add_dependencies(${PROJECT_NAME} create_output_dirs)

# Installation
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin)

# Check if resources directory exists
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources")
    # Create resources directory in the build directory
    add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/bin/resources"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/bin/resources/textures"
        COMMENT "Creating resources directories"
    )
    
    # Copy resource files to build directory
    file(GLOB_RECURSE RESOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/**/*.*")
    foreach(RESOURCE_FILE ${RESOURCE_FILES})
        file(RELATIVE_PATH REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}" "${RESOURCE_FILE}")
        add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                    "${RESOURCE_FILE}"
                    "${CMAKE_BINARY_DIR}/bin/${REL_PATH}"
            COMMENT "Copying resource file: ${REL_PATH}"
        )
    endforeach()
else()
    message(WARNING "Resources directory not found: ${CMAKE_CURRENT_SOURCE_DIR}/resources")
endif()

# Check for FFmpeg
find_program(FFMPEG_EXECUTABLE ffmpeg)
if(FFMPEG_EXECUTABLE)
    message(STATUS "Found FFmpeg: ${FFMPEG_EXECUTABLE}")
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_FFMPEG)
else()
    message(WARNING "FFmpeg not found: Video creation will be disabled. Please install FFmpeg for automatic video generation.")
endif()

# Print status message
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")