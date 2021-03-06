#include <iostream>
#include <SDL2/SDL.h>

#include "main.hpp"
#include "sdl_exception.hpp"
#include "main_loop.hpp"

int main() {
  if (SDL_Init(SDL_INIT_VIDEO)) {
    throw sdl_exception("Could not initialize video");
    return EXIT_FAILURE;
  }

  SDL_Window* window = SDL_CreateWindow(
      "rocket",
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      800,
      800,
      0
      );

  main_loop m(window, 800, 800);
  m.init();
  m.start();

  SDL_DestroyWindow(window);
  SDL_Quit();
  return EXIT_SUCCESS;
}
