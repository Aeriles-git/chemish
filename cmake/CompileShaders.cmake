# Find slangc in third_party.
find_program(SLANGC_EXECUTABLE
    NAMES slangc
    PATHS "${CMAKE_SOURCE_DIR}/third_party/slang/bin"
    NO_DEFAULT_PATH
    REQUIRED
)

# Compile a .slang file to .spv at build time.
# The .spv file is added as a dependency of the given target, so it rebuilds
# whenever the shader source changes.
function(add_shader target shader_path)
    get_filename_component(shader_name ${shader_path} NAME_WE)
    set(spv_path ${CMAKE_BINARY_DIR}/shaders/${shader_name}.spv)

    add_custom_command(
        OUTPUT ${spv_path}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/shaders
        COMMAND ${SLANGC_EXECUTABLE}
                ${shader_path}
                -target spirv
                -profile spirv_1_4
                -emit-spirv-directly
                -o ${spv_path}
        DEPENDS ${shader_path}
        COMMENT "Compiling ${shader_name}.slang -> ${shader_name}.spv"
        VERBATIM
    )

    target_sources(${target} PRIVATE ${spv_path})
    set_source_files_properties(${spv_path} PROPERTIES GENERATED TRUE)
endfunction()
