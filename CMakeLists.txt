cmake_minimum_required(VERSION 3.20)
project(VkRender)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)

find_program(GLSLC glslc HINTS "$ENV{VULKAN_SDK}/Bin")



set(SHADER_SOURCES
	${CMAKE_SOURCE_DIR}/src/shader/vertex.vert
	${CMAKE_SOURCE_DIR}/src/shader/fragment.frag
)

foreach(SHADER ${SHADER_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    set(SPIRV ${CMAKE_BINARY_DIR}/shader/${SHADER_NAME}.spv)
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/shader
        COMMAND ${GLSLC} ${SHADER} -o ${SPIRV}
        DEPENDS ${SHADER}
    )
    list(APPEND SPIRV_BINARIES ${SPIRV})
endforeach()

add_custom_target(Shaders DEPENDS ${SPIRV_BINARIES})



add_executable(VkRender 
	main.cpp
	src/vulkan/instance.cpp
	src/vulkan/device.cpp
	src/vulkan/swapchain.cpp
	src/vulkan/pipeline.cpp
	src/vulkan/command.cpp
	src/vulkan/vertex.cpp
	src/vulkan/memory.cpp
	src/vulkan/data/model.cpp
)

add_dependencies(VkRender Shaders)

target_include_directories(VkRender PRIVATE
	"C:/cpp-libraries/glfw-3.5.1.bin.WIN64/include"
	${Vulkan_INCLUDE_DIRS}
)

target_link_directories(VkRender PRIVATE
	"C:/cpp-libraries/glfw-3.5.1.bin.WIN64/lib-vc2026"
)

target_link_libraries(VkRender PRIVATE
	glfw3
	${Vulkan_LIBRARIES}
)