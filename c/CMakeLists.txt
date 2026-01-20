cmake_minimum_required(VERSION 3.24...4.2)

project(GameOfLife LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
option(BUILD_CLI "Build CLI version of GameOfLife" ON)
option(BUILD_GUI "Build GUI version of GameOfLife" ON)

# SIMD
add_compile_options(-mavx2 -O3)

if(BUILD_CLI)
    add_executable(${PROJECT_NAME}-cli
        src/cli.c
        src/game_logic.c
    )
    target_include_directories(${PROJECT_NAME}-cli PRIVATE include)
endif()

if(BUILD_GUI)
    add_executable(${PROJECT_NAME}-gui
        src/gui.c
        src/gui_logic.c
        src/gui_utils.c
        src/game_logic.c
        src/nuklear_impl.c
        external/glad/src/glad.c
    )

    target_include_directories(${PROJECT_NAME}-gui PRIVATE
        include
        src
        external/glfw/include
        external/glad/include
        external/nuklear
    )

    target_precompile_headers(${PROJECT_NAME}-gui PRIVATE
        <glad/glad.h>
        <GLFW/glfw3.h>
    )
# target_compile_options(${PROJECT_NAME}-gui PRIVATE -Os -fdata-sections -ffunction-sections)
# target_link_options(${PROJECT_NAME}-gui PRIVATE -s -Wl,--gc-sections)
# include(CheckIPOSupported)
# check_ipo_supported(RESULT supported OUTPUT error)

# if(supported)
#     set_property(TARGET ${PROJECT_NAME}-gui PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
# endif()

    target_link_directories(${PROJECT_NAME}-gui PRIVATE external/glfw/lib)

    if(WIN32)
        target_link_libraries(${PROJECT_NAME}-gui
            glfw3
            opengl32
            gdi32
        )
    elseif(UNIX)
        target_link_libraries(${PROJECT_NAME}-gui
            glfw
            GL
            m
            # pthread
            # dl
            # X11
        )
    endif()
endif()