#pragma once

#include <vulkan/vulkan.h>

namespace chemish::rhi::vulkan {

class Device;
class Shader;

class Pipeline {
public:
  Pipeline(Device &device, Shader &shader, VkFormat colorFormat);
  ~Pipeline();

  Pipeline(const Pipeline &) = delete;
  Pipeline &operator=(const Pipeline &) = delete;

  const VkPipeline &getHandle() const { return handle; }
  const VkPipelineLayout &getLayout() const { return layout; }

private:
  Device &device;
  VkPipeline handle = VK_NULL_HANDLE;
  VkPipelineLayout layout = VK_NULL_HANDLE;
};

} // namespace chemish::rhi::vulkan
