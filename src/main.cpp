#include <SDL3/SDL_video.h>
#include <cstdint>
#include <vulkan/vulkan.hpp>

#include <SDL3/SDL.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class HelloTriangleApplication {
public:
  void run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

private:
  SDL_Window *window = nullptr;

  void initWindow() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("chemish", WIDTH, HEIGHT, SDL_WINDOW_VULKAN);
  }

  void initVulkan() {}

  void mainLoop() {
    bool running = true;
    while (running) {
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT)
          running = false;
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
          running = false;
      }
    }
  }
  void cleanup() {
    SDL_DestroyWindow(window);
    SDL_Quit();
  }
};

int main() {
  try {
    HelloTriangleApplication app;
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
