#pragma once

#include <vulkan/vulkan.h>

namespace chemish::rhi::vulkan {

class Device;
class Buffer;

// Single descriptor set with one uniform buffer binding at slot 0.
// Owns the layout and pool together.
class DescriptorSet {
public:
  DescriptorSet(Device &device, const Buffer &uniformBuffer);
  ~DescriptorSet();

  DescriptorSet(const DescriptorSet &) = delete;
  DescriptorSet &operator=(const DescriptorSet &) = delete;

  const VkDescriptorSetLayout &getLayout() const { return layout; }
  const VkDescriptorSet &getHandle() const { return set; }

private:
  Device &device;
  VkDescriptorSetLayout layout = VK_NULL_HANDLE;
  VkDescriptorPool pool = VK_NULL_HANDLE;
  VkDescriptorSet set = VK_NULL_HANDLE;
};

} // namespace chemish::rhi::vulkan
