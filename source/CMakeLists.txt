cmake_minimum_required(VERSION 3.15)

project(3DEngine VERSION 1.0 LANGUAGES CXX)

# -----------------------------
# C++ standard
# -----------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------
# Output directories (place everything in top-level bin)
# -----------------------------
# CMAKE_BINARY_DIR is bin/ (where cmake is invoked)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)

# -----------------------------
# Dependencies via FetchContent
# -----------------------------

find_package(OpenGL REQUIRED)

include(FetchContent)

# ----- GLAD -----
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG master
)
FetchContent_MakeAvailable(glad)

# ----- GLFW -----
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_USE_MSVC_RUNTIME_LIBRARY TRUE CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG latest
)
FetchContent_MakeAvailable(glfw)

# ----- FreeType -----
FetchContent_Declare(
    freetype
    GIT_REPOSITORY https://github.com/freetype/freetype.git
    GIT_TAG VER-2-13-1
)
FetchContent_MakeAvailable(freetype)

# ----- Assimp -----
FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG master
)
FetchContent_MakeAvailable(assimp)

# ----- GLM -----
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG master
)
FetchContent_MakeAvailable(glm)

# ----- STB -----
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)
FetchContent_MakeAvailable(stb)

# -----------------------------
# Collect all .cpp sources
# -----------------------------
file(GLOB_RECURSE PROJECT_SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

# -----------------------------
# Create executable
# -----------------------------
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_compile_definitions(${PROJECT_NAME} PRIVATE
    PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
    PROJECT_BINARY_DIR="${CMAKE_BINARY_DIR}"
)

# -----------------------------
# Include directories
# -----------------------------
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${PROJECT_SOURCE_DIR}/include
    ${glm_SOURCE_DIR}
    ${nuklear_SOURCE_DIR}
    ${stb_SOURCE_DIR}  

)

# -----------------------------
# Link libraries
# -----------------------------
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    glad
    glfw
    freetype
    assimp
)

# -----------------------------
# Platform-specific settings
# -----------------------------
if(APPLE)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo"
    )
endif()

if(WIN32)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE opengl32
    )

    # Copy SDL2.dll / SDL2d.dll to bin automatically
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:SDL2::SDL2>
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying SDL2.dll to output directory"
    )
endif()

# -----------------------------
# Copy resources to bin
# -----------------------------
set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)
set(RESOURCE_OUTPUT_DIR $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${RESOURCE_DIR}
            ${RESOURCE_OUTPUT_DIR}
    COMMENT "Copying resources to output directory"
)

# -----------------------------
# Compiler warnings
# -----------------------------
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
