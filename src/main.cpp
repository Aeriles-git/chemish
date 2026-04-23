#include <cstdio>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <chemish/commands.hpp>
#include <chemish/debug.hpp>
#include <chemish/device.hpp>
#include <chemish/pipeline.hpp>
#include <chemish/rhi/vulkan/device.hpp>
#include <chemish/shader.hpp>
#include <chemish/swapchain.hpp>
#include <chemish/sync.hpp>

int main() {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window =
      SDL_CreateWindow("chemish", 1280, 720, SDL_WINDOW_VULKAN);

  // rhiDevice.getInstance()
  chemish::rhi::vulkan::Device rhiDevice{window};
  chemish::Swapchain swapchain = chemish::createSwapchain(
      rhiDevice.getPhysical(), rhiDevice.getLogical(), rhiDevice.getSurface());
  chemish::Commands commands = chemish::createCommands(
      rhiDevice.getLogical(), rhiDevice.getQueueFamily());
  chemish::FrameSync sync = chemish::createFrameSync(rhiDevice.getLogical());
  std::vector<VkSemaphore> imageSemaphores = chemish::createImageSemaphores(
      rhiDevice.getLogical(), (uint32_t)swapchain.images.size());
  std::vector<VkSemaphore> renderSemaphores = chemish::createImageSemaphores(
      rhiDevice.getLogical(), (uint32_t)swapchain.images.size());
  uint32_t semaphoreIndex = 0;
  VkShaderModule shaderModule =
      chemish::loadShader(rhiDevice.getLogical(), "build/shaders/triangle.spv");
  chemish::Pipeline pipeline = chemish::createPipeline(
      rhiDevice.getLogical(), shaderModule, swapchain.format);

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
    vkWaitForFences(rhiDevice.getLogical(), 1, &sync.inFlight, VK_TRUE,
                    UINT64_MAX);
    vkResetFences(rhiDevice.getLogical(), 1, &sync.inFlight);

    // 2. Acquire the next swapchain image.
    VkSemaphore imageAvailable = imageSemaphores[semaphoreIndex];
    semaphoreIndex = (semaphoreIndex + 1) % imageSemaphores.size();

    uint32_t imageIndex = 0;
    vkAcquireNextImageKHR(rhiDevice.getLogical(), swapchain.handle, UINT64_MAX,
                          imageAvailable, VK_NULL_HANDLE, &imageIndex);

    VkSemaphore renderFinished = renderSemaphores[imageIndex];

    // 3. Record commands.
    vkResetCommandBuffer(commands.buffer, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    vkBeginCommandBuffer(commands.buffer, &beginInfo);

    // Transition: UNDEFINED -> COLOR_ATTACHMENT_OPTIMAL
    VkImageMemoryBarrier2 toDraw{};
    toDraw.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    toDraw.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
    toDraw.srcAccessMask = 0;
    toDraw.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    toDraw.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
    toDraw.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    toDraw.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    toDraw.image = swapchain.images[imageIndex];
    toDraw.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    toDraw.subresourceRange.levelCount = 1;
    toDraw.subresourceRange.layerCount = 1;

    VkDependencyInfo dep1{};
    dep1.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dep1.imageMemoryBarrierCount = 1;
    dep1.pImageMemoryBarriers = &toDraw;
    vkCmdPipelineBarrier2(commands.buffer, &dep1);

    // We need a view of the swapchain image to render into.
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = swapchain.images[imageIndex];
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = swapchain.format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView = VK_NULL_HANDLE;
    vkCreateImageView(rhiDevice.getLogical(), &viewInfo, nullptr, &imageView);

    // Begin dynamic rendering.
    VkRenderingAttachmentInfo colorAttachment{};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachment.imageView = imageView;
    colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.clearValue.color = {{0.1f, 0.1f, 0.2f, 1.0f}};

    VkRenderingInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.renderArea.extent = swapchain.extent;
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachment;

    vkCmdBeginRendering(commands.buffer, &renderingInfo);

    // Viewport + scissor (set dynamically since the pipeline doesn't bake
    // them).
    VkViewport vp{};
    vp.width = (float)swapchain.extent.width;
    vp.height = (float)swapchain.extent.height;
    vp.maxDepth = 1.0f;
    vkCmdSetViewport(commands.buffer, 0, 1, &vp);

    VkRect2D scissor{};
    scissor.extent = swapchain.extent;
    vkCmdSetScissor(commands.buffer, 0, 1, &scissor);

    // Draw.
    vkCmdBindPipeline(commands.buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      pipeline.handle);
    vkCmdDraw(commands.buffer, 3, 1, 0, 0);

    vkCmdEndRendering(commands.buffer);

    // Transition: COLOR_ATTACHMENT_OPTIMAL -> PRESENT_SRC
    VkImageMemoryBarrier2 toPresent = toDraw;
    toPresent.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    toPresent.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
    toPresent.dstStageMask = VK_PIPELINE_STAGE_2_NONE;
    toPresent.dstAccessMask = 0;
    toPresent.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
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
    waitSem.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSemaphoreSubmitInfo signalSem{};
    signalSem.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    signalSem.semaphore = renderFinished;
    signalSem.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;

    VkSubmitInfo2 submit{};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submit.waitSemaphoreInfoCount = 1;
    submit.pWaitSemaphoreInfos = &waitSem;
    submit.commandBufferInfoCount = 1;
    submit.pCommandBufferInfos = &cbSubmit;
    submit.signalSemaphoreInfoCount = 1;
    submit.pSignalSemaphoreInfos = &signalSem;

    VkResult submitResult =
        vkQueueSubmit2(rhiDevice.getQueue(), 1, &submit, sync.inFlight);
    if (submitResult != VK_SUCCESS) {
      std::fprintf(stderr, "vkQueueSubmit2 failed: %d\n", submitResult);
    }

    // 5. Present.
    VkPresentInfoKHR present{};
    present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.waitSemaphoreCount = 1;
    present.pWaitSemaphores = &renderFinished;
    present.swapchainCount = 1;
    present.pSwapchains = &swapchain.handle;
    present.pImageIndices = &imageIndex;
    vkQueuePresentKHR(rhiDevice.getQueue(), &present);

    vkDestroyImageView(rhiDevice.getLogical(), imageView, nullptr);
  }

  // Wait for GPU before destroying anything.
  vkDeviceWaitIdle(rhiDevice.getLogical());

  // Cleanup (reverse of creation).
  chemish::destroyPipeline(rhiDevice.getLogical(), pipeline);
  chemish::destroyShader(rhiDevice.getLogical(), shaderModule);
  chemish::destroyImageSemaphores(rhiDevice.getLogical(), imageSemaphores);
  chemish::destroyImageSemaphores(rhiDevice.getLogical(), renderSemaphores);
  chemish::destroyFrameSync(rhiDevice.getLogical(), sync);
  chemish::destroyCommands(rhiDevice.getLogical(), commands);
  chemish::destroySwapchain(rhiDevice.getLogical(), swapchain);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
