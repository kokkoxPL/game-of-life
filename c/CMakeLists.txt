cmake_minimum_required(VERSION 3.24...4.2)

project(GameOfLife LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(${PROJECT_NAME}-cli src/main.c)

# add_executable(${PROJECT_NAME}-gui
#     src/gui.c
#     external/glad/src/glad.c
# )

# target_include_directories(${PROJECT_NAME}-gui PRIVATE
#     src
#     external/glfw/include
#     external/glad/include
#     external/nuklear
# )

# target_link_directories(${PROJECT_NAME}-gui PRIVATE
#     external/glfw/lib
# )

# if(WIN32)
#     target_link_libraries(${PROJECT_NAME}-gui
#         glfw3
#         opengl32
#         gdi32
#     )
# elseif(UNIX)
#     target_link_libraries(${PROJECT_NAME}-gui
#         glfw
#         GL
#         m
#     )
# endif()
