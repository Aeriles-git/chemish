#pragma once

#include <vulkan/vulkan.h>

#include <string>

namespace chemish::rhi::vulkan {

class Device;

class Shader {
public:
  Shader(Device &device, const std::string &spvPath);
  ~Shader();

  Shader(const Shader &) = delete;
  Shader &operator=(const Shader &) = delete;

  const VkShaderModule &getModule() const { return module; }

private:
  Device &device;
  VkShaderModule module = VK_NULL_HANDLE;
};

} // namespace chemish::rhi::vulkan
