add_executable(chemish
    main.cpp
)

target_include_directories(chemish PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_definitions(chemish PRIVATE
  VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
  VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
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

