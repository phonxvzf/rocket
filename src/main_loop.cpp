#include <algorithm>
#include <cmath>

#include "main_loop.hpp"

main_loop::main_loop(SDL_Window *window, int width, int height)
  : m_window_width(width), m_window_height(height),

    // init simulator
    simulator(SIM_SIZE)
{
  m_renderer = SDL_CreateRenderer(
      window,
      -1,
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
      );

  // set gravity
  for (int i = 0; i <= (int) SIM_SIZE; ++i) {
    for (int j = 0; j <= (int) SIM_SIZE; ++j) {

      this->simulator.get_vec_x()[i][j] = (0 <= j && j < 50 ? -10.0f : -5.0f);
      this->simulator.get_vec_y()[i][j] = 20.0f;
    }
  }

  // set blend mode
  SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

  // TODO: sdl_exception
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

  static const int R = 0xEE;
  static const int G = 0xEE;
  static const int B = 0xEE;

  // simulate the model
  static int rocket_x = SIM_SIZE / 2, rocket_y = 10;
  // rocket_x = (rocket_x + SIM_SIZE - 1) % SIM_SIZE;
  // rocket_y = (rocket_y + SIM_SIZE - 1) % SIM_SIZE;

  this->simulator.get_dens()[rocket_x][rocket_y] += 10;
  this->simulator.get_dens()[rocket_x + 10][rocket_y + 10] += 10;
  this->simulator.simulate(dt / 100.0);


  // fill background
  const SDL_Rect window_rect = { 0, 0, m_window_width, m_window_height };
  SDL_SetRenderDrawColor (this->m_renderer, 0x00, 0x00, 0x00, 0xFF);
  SDL_RenderFillRect     (this->m_renderer, &window_rect);

  // render the image
  const size_t  size_x = this->m_window_width  / SIM_SIZE;
  const size_t  pad_x  = this->m_window_width  % SIM_SIZE; 
  const size_t  size_y = this->m_window_height / SIM_SIZE; 
  const size_t  pad_y  = this->m_window_height % SIM_SIZE;
  
  for (size_t i = 0; i < SIM_SIZE; ++i) {
    for (size_t j = 0; j < SIM_SIZE; ++j) {

      const int x = i * size_x + std::min(i, pad_x);
      const int y = j * size_y + std::min(j, pad_y);

      // draw density
      SDL_Rect rect {x, y, (int) size_x + (i < pad_x), (int) size_y + (j < pad_y)};
      const uint8_t alpha = std::min(255, (int) floor(this->simulator.get_dens()[i][j] * 256));

      SDL_SetRenderDrawColor (this->m_renderer, R, G, B, alpha); 
      SDL_RenderFillRect     (this->m_renderer, &rect);
    }
  }

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
