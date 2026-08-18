cmake_minimum_required(VERSION 3.20)
project(VkRender)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(VkRender main.cpp)

target_include_directories(VkRender PRIVATE
	"C:/cpp-libraries/glfw-3.5.1.bin.WIN64/include"
)

target_link_directories(VkRender PRIVATE
	"C:/cpp-libraries/glfw-3.5.1.bin.WIN64/lib-vc2026"
)

target_link_libraries(VkRender PRIVATE
	glfw3
)