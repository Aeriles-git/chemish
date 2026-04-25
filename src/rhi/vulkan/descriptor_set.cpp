#include <chemish/rhi/vulkan/buffer.hpp>
#include <chemish/rhi/vulkan/descriptor_set.hpp>
#include <chemish/rhi/vulkan/device.hpp>

namespace chemish::rhi::vulkan {

DescriptorSet::DescriptorSet(Device &dev, const Buffer &uniformBuffer)
    : device(dev) {
  VkDescriptorSetLayoutBinding binding{};
  binding.binding = 0;
  binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  binding.descriptorCount = 1;
  binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = 1;
  layoutInfo.pBindings = &binding;
  vkCreateDescriptorSetLayout(device.getLogical(), &layoutInfo, nullptr,
                              &layout);

  VkDescriptorPoolSize poolSize{};
  poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSize.descriptorCount = 1;

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.maxSets = 1;
  poolInfo.poolSizeCount = 1;
  poolInfo.pPoolSizes = &poolSize;
  vkCreateDescriptorPool(device.getLogical(), &poolInfo, nullptr, &pool);

  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = pool;
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = &layout;
  vkAllocateDescriptorSets(device.getLogical(), &allocInfo, &set);

  VkDescriptorBufferInfo bufferInfo{};
  bufferInfo.buffer = uniformBuffer.getHandle();
  bufferInfo.offset = 0;
  bufferInfo.range = uniformBuffer.getSize();

  VkWriteDescriptorSet write{};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.dstSet = set;
  write.dstBinding = 0;
  write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  write.descriptorCount = 1;
  write.pBufferInfo = &bufferInfo;
  vkUpdateDescriptorSets(device.getLogical(), 1, &write, 0, nullptr);
}

DescriptorSet::~DescriptorSet() {
  vkDestroyDescriptorPool(device.getLogical(), pool, nullptr);
  vkDestroyDescriptorSetLayout(device.getLogical(), layout, nullptr);
  // The set itself is freed automatically with the pool.
}

} // namespace chemish::rhi::vulkan
