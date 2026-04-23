#pragma once

#include <vulkan/vulkan.h>

namespace chemish {

struct Pipeline {
  VkPipeline handle = VK_NULL_HANDLE;
  VkPipelineLayout layout = VK_NULL_HANDLE;
};

// Creates a graphics pipeline with the given shader module (vertex + fragment
// entry points assumed to be "vertexMain" and "fragmentMain") that renders
// into a color attachment of the given format. Uses dynamic rendering.
Pipeline createPipeline(VkDevice device, VkShaderModule shader,
                        VkFormat colorFormat);

void destroyPipeline(VkDevice device, Pipeline &pipeline);

} // namespace chemish
