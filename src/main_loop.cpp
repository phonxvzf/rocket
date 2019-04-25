#include "main_loop.hpp"
#include "sdl_exception.hpp"

main_loop::main_loop(SDL_Window *window, int width, int height)
  : m_window_width(width), m_window_height(height)
{
  m_renderer = SDL_CreateRenderer(
      window,
      -1,
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
      );
  if (m_renderer == nullptr) throw sdl_exception("Could not create renderer");
}

void main_loop::clean_up() {
  SDL_DestroyRenderer(m_renderer);
}

void main_loop::keydown_callback(SDL_Scancode scancode) {
  switch (scancode) {
    case SDL_SCANCODE_Q:
      m_continue_loop = false;
      break;
    default:
      // Do nothing
      break;
  }
}

void main_loop::draw(uint32_t dt) {
  // TODO
}

void main_loop::start() {
  m_loop_tick_start = SDL_GetTicks();
  while (m_continue_loop) {
    // Process events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_KEYDOWN:
          keydown_callback(event.key.keysym.scancode);
          break;
      }
    }

    // Draw graphics
    SDL_RenderClear(m_renderer);
    draw(SDL_GetTicks() - m_loop_tick_start);
    SDL_RenderPresent(m_renderer);

    m_loop_tick_start = SDL_GetTicks();
  }
}
