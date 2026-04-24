#include <chemish/renderer.hpp>

#include <vulkan/vulkan.h>

namespace chemish {

Renderer::Renderer(SDL_Window *window)
    : device(window), swapchain(device), commands(device), sync(device),
      imageSemaphores(device, (uint32_t)swapchain.getImages().size()),
      renderSemaphores(device, (uint32_t)swapchain.getImages().size()),
      shader(device, "build/shaders/triangle.spv"),
      pipeline(device, shader, swapchain.getFormat()),
      cameraBuffer(device, sizeof(CameraUniform),
                   VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) {}

Renderer::~Renderer() { vkDeviceWaitIdle(device.getLogical()); }

MeshHandle Renderer::createMesh(const std::vector<Vertex> &vertices) {
  MeshHandle handle{nextMeshId++};
  meshes.try_emplace(
      handle.id,
      rhi::vulkan::Buffer{device, vertices.size() * sizeof(Vertex),
                          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertices.data()},
      (uint32_t)vertices.size());
  return handle;
}

void Renderer::updateCamera(const Camera &camera) {
  CameraUniform data{};
  data.viewProj = camera.projection() * camera.view();
  cameraBuffer.write(&data, sizeof(data));
}

void Renderer::drawFrame(MeshHandle handle) {
  const Mesh &mesh = meshes.at(handle.id);

  VkFence fence = sync.getFence();
  vkWaitForFences(device.getLogical(), 1, &fence, VK_TRUE, UINT64_MAX);
  vkResetFences(device.getLogical(), 1, &fence);

  VkSemaphore imageAvailable = imageSemaphores[semaphoreIndex];
  semaphoreIndex = (semaphoreIndex + 1) % imageSemaphores.size();

  uint32_t imageIndex = 0;
  vkAcquireNextImageKHR(device.getLogical(), swapchain.getHandle(), UINT64_MAX,
                        imageAvailable, VK_NULL_HANDLE, &imageIndex);

  VkSemaphore renderFinished = renderSemaphores[imageIndex];

  vkResetCommandBuffer(commands.getBuffer(), 0);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  vkBeginCommandBuffer(commands.getBuffer(), &beginInfo);

  VkImageMemoryBarrier2 toDraw{};
  toDraw.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
  toDraw.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
  toDraw.srcAccessMask = 0;
  toDraw.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
  toDraw.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
  toDraw.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  toDraw.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  toDraw.image = swapchain.getImages()[imageIndex];
  toDraw.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  toDraw.subresourceRange.levelCount = 1;
  toDraw.subresourceRange.layerCount = 1;

  VkDependencyInfo dep1{};
  dep1.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
  dep1.imageMemoryBarrierCount = 1;
  dep1.pImageMemoryBarriers = &toDraw;
  vkCmdPipelineBarrier2(commands.getBuffer(), &dep1);

  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = swapchain.getImages()[imageIndex];
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = swapchain.getFormat();
  viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.layerCount = 1;

  VkImageView imageView = VK_NULL_HANDLE;
  vkCreateImageView(device.getLogical(), &viewInfo, nullptr, &imageView);

  VkRenderingAttachmentInfo colorAttachment{};
  colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
  colorAttachment.imageView = imageView;
  colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.clearValue.color = {{0.1f, 0.1f, 0.2f, 1.0f}};

  VkRenderingInfo renderingInfo{};
  renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
  renderingInfo.renderArea.extent = swapchain.getExtent();
  renderingInfo.layerCount = 1;
  renderingInfo.colorAttachmentCount = 1;
  renderingInfo.pColorAttachments = &colorAttachment;

  vkCmdBeginRendering(commands.getBuffer(), &renderingInfo);

  VkViewport vp{};
  vp.width = (float)swapchain.getExtent().width;
  vp.height = (float)swapchain.getExtent().height;
  vp.maxDepth = 1.0f;
  vkCmdSetViewport(commands.getBuffer(), 0, 1, &vp);

  VkRect2D scissor{};
  scissor.extent = swapchain.getExtent();
  vkCmdSetScissor(commands.getBuffer(), 0, 1, &scissor);

  vkCmdBindPipeline(commands.getBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipeline.getHandle());

  VkDeviceSize offset = 0;
  vkCmdBindVertexBuffers(commands.getBuffer(), 0, 1, &mesh.buffer.getHandle(),
                         &offset);
  vkCmdDraw(commands.getBuffer(), mesh.vertexCount, 1, 0, 0);

  vkCmdEndRendering(commands.getBuffer());

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
  vkCmdPipelineBarrier2(commands.getBuffer(), &dep2);

  vkEndCommandBuffer(commands.getBuffer());

  VkCommandBufferSubmitInfo cbSubmit{};
  cbSubmit.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
  cbSubmit.commandBuffer = commands.getBuffer();

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

  vkQueueSubmit2(device.getQueue(), 1, &submit, fence);

  VkSwapchainKHR swapchainHandle = swapchain.getHandle();
  VkPresentInfoKHR present{};
  present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present.waitSemaphoreCount = 1;
  present.pWaitSemaphores = &renderFinished;
  present.swapchainCount = 1;
  present.pSwapchains = &swapchainHandle;
  present.pImageIndices = &imageIndex;
  vkQueuePresentKHR(device.getQueue(), &present);

  vkDestroyImageView(device.getLogical(), imageView, nullptr);
}

} // namespace chemish
