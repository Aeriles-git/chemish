#include <chemish/shader.hpp>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <vector>

namespace chemish {

VkShaderModule loadShader(VkDevice device, const std::string &path) {
  // Read the file as binary.
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  if (!file) {
    std::fprintf(stderr, "failed to open shader: %s\n", path.c_str());
    std::exit(1);
  }

  size_t size = (size_t)file.tellg();
  std::vector<char> bytes(size);
  file.seekg(0);
  file.read(bytes.data(), size);

  // Create the shader module.
  VkShaderModuleCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  info.codeSize = size;
  info.pCode = reinterpret_cast<const uint32_t *>(bytes.data());

  VkShaderModule module = VK_NULL_HANDLE;
  vkCreateShaderModule(device, &info, nullptr, &module);
  return module;
}

void destroyShader(VkDevice device, VkShaderModule shader) {
  vkDestroyShaderModule(device, shader, nullptr);
}

} // namespace chemish
