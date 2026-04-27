add_executable(chemish
    main.cpp
)

target_include_directories(chemish PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(chemish PRIVATE
    SDL3::SDL3
    Vulkan::Vulkan
    GPUOpen::VulkanMemoryAllocator
    glm::glm
)

target_compile_options(chemish PRIVATE
    -Wall -Wextra -Wpedantic
)

