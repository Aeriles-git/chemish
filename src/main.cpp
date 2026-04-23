#include <cstdio>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <chemish/commands.hpp>
#include <chemish/debug.hpp>
#include <chemish/device.hpp>
#include <chemish/pipeline.hpp>
#include <chemish/shader.hpp>
#include <chemish/swapchain.hpp>
#include <chemish/sync.hpp>

int main() {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window =
      SDL_CreateWindow("chemish", 1280, 720, SDL_WINDOW_VULKAN);

  // Instance
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "chemish";
  appInfo.apiVersion = VK_API_VERSION_1_4;

  Uint32 extCount = 0;
  const char *const *sdlExts = SDL_Vulkan_GetInstanceExtensions(&extCount);
  std::vector<const char *> extensions(sdlExts, sdlExts + extCount);
  extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  const char *layers[] = {chemish::kValidationLayerName};

  VkDebugUtilsMessengerCreateInfoEXT dbgInfo;
  chemish::fillDebugMessengerInfo(dbgInfo);

  VkInstanceCreateInfo instanceInfo{};
  instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceInfo.pNext = &dbgInfo;
  instanceInfo.pApplicationInfo = &appInfo;
  instanceInfo.enabledLayerCount = 1;
  instanceInfo.ppEnabledLayerNames = layers;
  instanceInfo.enabledExtensionCount = (uint32_t)extensions.size();
  instanceInfo.ppEnabledExtensionNames = extensions.data();

  VkInstance instance = VK_NULL_HANDLE;
  if (vkCreateInstance(&instanceInfo, nullptr, &instance) != VK_SUCCESS) {
    std::fprintf(stderr, "vkCreateInstance failed\n");
    return 1;
  }

  VkDebugUtilsMessengerEXT messenger = chemish::createDebugMessenger(instance);

  // Surface
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  if (!SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface)) {
    std::fprintf(stderr, "SDL_Vulkan_CreateSurface failed: %s\n",
                 SDL_GetError());
    return 1;
  }

  // Device, swapchain, commands, sync
  chemish::Device device = chemish::createDevice(instance, surface);
  chemish::Swapchain swapchain =
      chemish::createSwapchain(device.physical, device.logical, surface);
  chemish::Commands commands =
      chemish::createCommands(device.logical, device.queueFamily);
  chemish::FrameSync sync = chemish::createFrameSync(device.logical);
  std::vector<VkSemaphore> imageSemaphores = chemish::createImageSemaphores(
      device.logical, (uint32_t)swapchain.images.size());
  uint32_t semaphoreIndex = 0;
  VkShaderModule shaderModule =
      chemish::loadShader(device.logical, "build/shaders/triangle.spv");
  chemish::Pipeline pipeline =
      chemish::createPipeline(device.logical, shaderModule, swapchain.format);

  // Render loop
  bool running = true;
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT)
        running = false;
      if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
        running = false;
    }

    // 1. Wait for the previous frame's GPU work to finish.
    vkWaitForFences(device.logical, 1, &sync.inFlight, VK_TRUE, UINT64_MAX);
    vkResetFences(device.logical, 1, &sync.inFlight);

    // 2. Acquire the next swapchain image.
    VkSemaphore imageAvailable = imageSemaphores[semaphoreIndex];
    semaphoreIndex = (semaphoreIndex + 1) % imageSemaphores.size();

    uint32_t imageIndex = 0;
    vkAcquireNextImageKHR(device.logical, swapchain.handle, UINT64_MAX,
                          imageAvailable, VK_NULL_HANDLE, &imageIndex);

    // 3. Record commands.
    vkResetCommandBuffer(commands.buffer, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    vkBeginCommandBuffer(commands.buffer, &beginInfo);

    // Transition: UNDEFINED -> TRANSFER_DST_OPTIMAL
    VkImageMemoryBarrier2 toClear{};
    toClear.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    toClear.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
    toClear.srcAccessMask = 0;
    toClear.dstStageMask = VK_PIPELINE_STAGE_2_CLEAR_BIT;
    toClear.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    toClear.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    toClear.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    toClear.image = swapchain.images[imageIndex];
    toClear.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    toClear.subresourceRange.levelCount = 1;
    toClear.subresourceRange.layerCount = 1;

    VkDependencyInfo dep1{};
    dep1.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dep1.imageMemoryBarrierCount = 1;
    dep1.pImageMemoryBarriers = &toClear;
    vkCmdPipelineBarrier2(commands.buffer, &dep1);

    // Clear to dark blue.
    VkClearColorValue clearColor{{0.1f, 0.1f, 0.2f, 1.0f}};
    VkImageSubresourceRange range{};
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    range.levelCount = 1;
    range.layerCount = 1;
    vkCmdClearColorImage(commands.buffer, swapchain.images[imageIndex],
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearColor, 1,
                         &range);

    // Transition: TRANSFER_DST_OPTIMAL -> PRESENT_SRC
    VkImageMemoryBarrier2 toPresent = toClear;
    toPresent.srcStageMask = VK_PIPELINE_STAGE_2_CLEAR_BIT;
    toPresent.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    toPresent.dstStageMask = VK_PIPELINE_STAGE_2_NONE;
    toPresent.dstAccessMask = 0;
    toPresent.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    toPresent.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkDependencyInfo dep2{};
    dep2.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dep2.imageMemoryBarrierCount = 1;
    dep2.pImageMemoryBarriers = &toPresent;
    vkCmdPipelineBarrier2(commands.buffer, &dep2);

    vkEndCommandBuffer(commands.buffer);

    // 4. Submit: wait on imageAvailable, signal renderFinished and the fence.
    VkCommandBufferSubmitInfo cbSubmit{};
    cbSubmit.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    cbSubmit.commandBuffer = commands.buffer;

    VkSemaphoreSubmitInfo waitSem{};
    waitSem.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    waitSem.semaphore = imageAvailable;
    waitSem.stageMask = VK_PIPELINE_STAGE_2_CLEAR_BIT;

    VkSemaphoreSubmitInfo signalSem{};
    signalSem.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    signalSem.semaphore = sync.renderFinished;
    signalSem.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;

    VkSubmitInfo2 submit{};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submit.waitSemaphoreInfoCount = 1;
    submit.pWaitSemaphoreInfos = &waitSem;
    submit.commandBufferInfoCount = 1;
    submit.pCommandBufferInfos = &cbSubmit;
    submit.signalSemaphoreInfoCount = 1;
    submit.pSignalSemaphoreInfos = &signalSem;

    vkQueueSubmit2(device.queue, 1, &submit, sync.inFlight);

    // 5. Present.
    VkPresentInfoKHR present{};
    present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.waitSemaphoreCount = 1;
    present.pWaitSemaphores = &sync.renderFinished;
    present.swapchainCount = 1;
    present.pSwapchains = &swapchain.handle;
    present.pImageIndices = &imageIndex;
    vkQueuePresentKHR(device.queue, &present);
  }

  // Wait for GPU before destroying anything.
  vkDeviceWaitIdle(device.logical);

  // Cleanup (reverse of creation).
  chemish::destroyPipeline(device.logical, pipeline);
  chemish::destroyShader(device.logical, shaderModule);
  chemish::destroyImageSemaphores(device.logical, imageSemaphores);
  chemish::destroyFrameSync(device.logical, sync);
  chemish::destroyCommands(device.logical, commands);
  chemish::destroySwapchain(device.logical, swapchain);
  chemish::destroyDevice(device);
  SDL_Vulkan_DestroySurface(instance, surface, nullptr);
  chemish::destroyDebugMessenger(instance, messenger);
  vkDestroyInstance(instance, nullptr);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
