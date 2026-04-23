#pragma once

#include <string>
#include <vulkan/vulkan.h>

namespace chemish {

// Load a compiled SPIR-V file and create a VkShaderModule from it.
VkShaderModule loadShader(VkDevice device, const std::string &path);

void destroyShader(VkDevice device, VkShaderModule shader);

} // namespace chemish
