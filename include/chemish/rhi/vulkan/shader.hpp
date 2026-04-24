#pragma once

#include <vulkan/vulkan.h>

#include <string>

#include <chemish/rhi/shader.hpp>

namespace chemish::rhi::vulkan {

class Device;

class Shader : public rhi::Shader {
public:
  Shader(Device &device, const std::string &spvPath);
  ~Shader() override;

  Shader(const Shader &) = delete;
  Shader &operator=(const Shader &) = delete;

  const VkShaderModule &getModule() const { return module; }

private:
  Device &device;
  VkShaderModule module = VK_NULL_HANDLE;
};

} // namespace chemish::rhi::vulkan
