add_executable(chemish
    main.cpp
    debug.cpp
    rhi/vulkan/pipeline.cpp
    rhi/vulkan/device.cpp
    rhi/vulkan/swapchain.cpp
    rhi/vulkan/commands.cpp
    rhi/vulkan/sync.cpp
    rhi/vulkan/shader.cpp
    rhi/vulkan/buffer.cpp
)

target_include_directories(chemish PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(chemish PRIVATE
    SDL3::SDL3
    Vulkan::Vulkan
    GPUOpen::VulkanMemoryAllocator
)

target_compile_options(chemish PRIVATE
    -Wall -Wextra -Wpedantic
)

add_shader(chemish ${CMAKE_SOURCE_DIR}/shaders/triangle.slang)
