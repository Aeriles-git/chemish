add_executable(chemish
    main.cpp
)

target_include_directories(chemish PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

find_package(SDL3 REQUIRED CONFIG)

target_link_libraries(chemish PRIVATE
    SDL3::SDL3
)

target_compile_options(chemish PRIVATE
    -Wall -Wextra -Wpedantic
)
