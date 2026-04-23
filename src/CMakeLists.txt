find_package(SDL3 REQUIRED CONFIG)
find_package(Vulkan REQUIRED)

add_executable(chemish
    main.cpp
    debug.cpp
    sync.cpp
    shader.cpp
    pipeline.cpp
    rhi/vulkan/device.cpp
    rhi/vulkan/swapchain.cpp
    rhi/vulkan/commands.cpp
)

target_include_directories(chemish PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(chemish PRIVATE
    SDL3::SDL3
    Vulkan::Vulkan
)

target_compile_options(chemish PRIVATE
    -Wall -Wextra -Wpedantic
)

add_shader(chemish ${CMAKE_SOURCE_DIR}/shaders/triangle.slang)
