#pragma once

#include <chemish/rhi/vulkan/buffer.hpp>
#include <chemish/rhi/vulkan/commands.hpp>
#include <chemish/rhi/vulkan/device.hpp>
#include <chemish/rhi/vulkan/pipeline.hpp>
#include <chemish/rhi/vulkan/shader.hpp>
#include <chemish/rhi/vulkan/swapchain.hpp>
#include <chemish/rhi/vulkan/sync.hpp>

struct SDL_Window;

namespace chemish {

class Renderer {
public:
  Renderer(SDL_Window *window);
  ~Renderer();

  Renderer(const Renderer &) = delete;
  Renderer &operator=(const Renderer &) = delete;

  void drawFrame();

private:
  rhi::vulkan::Device device;
  rhi::vulkan::Swapchain swapchain;
  rhi::vulkan::Commands commands;
  rhi::vulkan::FrameSync sync;
  rhi::vulkan::ImageSemaphores imageSemaphores;
  rhi::vulkan::ImageSemaphores renderSemaphores;
  rhi::vulkan::Shader shader;
  rhi::vulkan::Pipeline pipeline;
  rhi::vulkan::Buffer vertexBuffer;

  uint32_t semaphoreIndex = 0;
};

} // namespace chemish
