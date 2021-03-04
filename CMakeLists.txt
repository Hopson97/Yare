cmake_minimum_required(VERSION 3.10)

#Set up project
project(yare VERSION 1.0)

#Set executable
add_library(yare-lib
    src/Client/Renderer/Framebuffer.cpp
    src/Client/Renderer/GLDebug.cpp 
    src/Client/Renderer/Mesh.cpp
    src/Client/Renderer/Shader.cpp 
    src/Client/Renderer/Texture.cpp
    src/Client/Renderer/VertexArray.cpp

    src/Client/Screen/InGameScreen.cpp 
    src/Client/Screen/MainMenuScreen.cpp
    src/Client/Screen/Screen.cpp

    src/Client/ClientSettings.cpp
    src/Client/Input.cpp
    src/Client/Camera.cpp



    src/Maths.cpp
)

add_executable(yare
    src/main.cpp
)

#Set module path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

#Find libraries
find_package(Threads)
find_package(SFML REQUIRED audio network graphics window system)

add_subdirectory(deps/imgui)
add_subdirectory(deps/imgui_impl)
add_subdirectory(deps/glad)
add_subdirectory(deps/glm)

function(setup_target TARGET)
    #Set C++17
    target_compile_features(${TARGET} PUBLIC cxx_std_17)
    set_target_properties(${TARGET} PROPERTIES CXX_EXTENSIONS OFF)

    #Set flags
    if(MSVC)
        target_compile_options(${TARGET} PRIVATE 
            /W4 /WX)
    else()
        target_compile_options(${TARGET} PRIVATE 
            -Wall -Wextra -pedantic -Werror)
    endif()

    target_include_directories(
        ${TARGET}
        PRIVATE
        deps
    )
endfunction(setup_target)

setup_target(yare-lib)
setup_target(yare)

target_link_libraries(yare
yare-lib
    glad
    imgui
    glm
    imgui_impl
    Threads::Threads 
    ${SFML_LIBRARIES} 
    ${SFML_DEPENDENCIES}
    ${CMAKE_DL_LIBS}
)