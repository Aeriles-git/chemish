add_executable(chemish
    main.cpp
    debug.cpp
    device.cpp
    swapchain.cpp
    commands.cpp
    sync.cpp
)

target_include_directories(chemish PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

find_package(SDL3 REQUIRED CONFIG)
find_package(Vulkan REQUIRED)

target_link_libraries(chemish PRIVATE
    SDL3::SDL3
    Vulkan::Vulkan
)

target_compile_options(chemish PRIVATE
    -Wall -Wextra -Wpedantic
)
