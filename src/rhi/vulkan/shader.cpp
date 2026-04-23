#include <chemish/rhi/vulkan/device.hpp>
#include <chemish/rhi/vulkan/shader.hpp>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <vector>

namespace chemish::rhi::vulkan {

Shader::Shader(Device &dev, const std::string &path) : device(dev) {
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  if (!file) {
    std::fprintf(stderr, "failed to open shader: %s\n", path.c_str());
    std::exit(1);
  }

  size_t size = (size_t)file.tellg();
  std::vector<char> bytes(size);
  file.seekg(0);
  file.read(bytes.data(), size);

  VkShaderModuleCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  info.codeSize = size;
  info.pCode = reinterpret_cast<const uint32_t *>(bytes.data());

  vkCreateShaderModule(device.getLogical(), &info, nullptr, &module);
}

Shader::~Shader() {
  vkDestroyShaderModule(device.getLogical(), module, nullptr);
}

} // namespace chemish::rhi::vulkan
