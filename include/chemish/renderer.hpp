#pragma once

#include <chemish/camera.hpp>
#include <chemish/mesh.hpp>
#include <chemish/rhi/vulkan/buffer.hpp>
#include <chemish/rhi/vulkan/commands.hpp>
#include <chemish/rhi/vulkan/device.hpp>
#include <chemish/rhi/vulkan/pipeline.hpp>
#include <chemish/rhi/vulkan/shader.hpp>
#include <chemish/rhi/vulkan/swapchain.hpp>
#include <chemish/rhi/vulkan/sync.hpp>
#include <chemish/vertex.hpp>

#include <unordered_map>
#include <vector>

struct SDL_Window;

namespace chemish {

class Renderer {
public:
  Renderer(SDL_Window *window);
  ~Renderer();

  Renderer(const Renderer &) = delete;
  Renderer &operator=(const Renderer &) = delete;

  MeshHandle createMesh(const std::vector<Vertex> &vertices);
  void drawFrame(MeshHandle mesh);
  void updateCamera(const Camera &camera);

private:
  struct Mesh {
    rhi::vulkan::Buffer buffer;
    uint32_t vertexCount;

    Mesh(rhi::vulkan::Buffer &&b, uint32_t c)
        : buffer(std::move(b)), vertexCount(c) {}
  };

  rhi::vulkan::Device device;
  rhi::vulkan::Swapchain swapchain;
  rhi::vulkan::Commands commands;
  rhi::vulkan::FrameSync sync;
  rhi::vulkan::ImageSemaphores imageSemaphores;
  rhi::vulkan::ImageSemaphores renderSemaphores;
  rhi::vulkan::Shader shader;
  rhi::vulkan::Pipeline pipeline;
  rhi::vulkan::Buffer cameraBuffer;

  std::unordered_map<uint32_t, Mesh> meshes;
  uint32_t nextMeshId = 1;
  uint32_t semaphoreIndex = 0;
};

} // namespace chemish
