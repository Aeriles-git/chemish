#include <chemish/renderer.hpp>

#include <SDL3/SDL.h>

int main() {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window =
      SDL_CreateWindow("chemish", 1280, 720, SDL_WINDOW_VULKAN);

  {
    chemish::Renderer renderer{window};

    bool running = true;
    while (running) {
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT)
          running = false;
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
          running = false;
      }
      renderer.drawFrame();
    }
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
