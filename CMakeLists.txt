cmake_minimum_required(VERSION 3.10)

#Set up project
project(
    rfrr 
    VERSION 1.0
)

include(${CMAKE_BINARY_DIR}/../conanbuildinfo.cmake)
conan_basic_setup()


#Set executable
add_executable(${PROJECT_NAME}     
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
    src/main.cpp
)

#Set C++17
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

#Set flags
if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "/O2")
    set(CMAKE_CXX_FLAGS_RELEASE "/Ox")
  	target_compile_options(${PROJECT_NAME} PRIVATE 
    	/W4)
else()
  	target_compile_options(${PROJECT_NAME} PRIVATE 
		-Wall -Wextra -pedantic)		
endif()


#Find libraries
find_package(Threads)

add_subdirectory(deps/imgui)
add_subdirectory(deps/imgui_impl)
add_subdirectory(deps/glad)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    deps
)

#Finally
target_link_libraries(${PROJECT_NAME} 
    glad
    imgui
    imgui_impl
    Threads::Threads 
    ${CMAKE_DL_LIBS}
    ${CONAN_LIBS}
)
