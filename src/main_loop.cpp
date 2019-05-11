#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>

#include "main_loop.hpp"

namespace util {
  void interpolate_color(float p, float min_p, float max_p, uint8_t* r, uint8_t* g, uint8_t* b) {
    const float h = 6 - ((p - min_p) / (max_p - min_p) * 6);
    const float x = 1 - std::abs(std::fmod(h, 2.0f) - 1);
    if (0 <= h && h <= 1) {
      *r = 255;
      *g = x * 255;
      *b = 0;
    } else if (1 < h && h <= 2) {
      *r = x * 255;
      *g = 255;
      *b = 0;
    } else if (2 < h && h <= 3) {
      *r = 0;
      *g = 255;
      *b = x * 255;
    } else if (3 < h && h <= 4) {
      *r = 0;
      *g = x * 255;
      *b = 255;
    } else if (4 < h && h <= 5) {
      *r = x * 255;
      *g = 0;
      *b = 255;
    } else if (5 < h && h <= 6) {
      *r = 255;
      *g = 0;
      *b = x * 255;
    } else {
      *r = *g = *b = 0;
    }
  }
}

main_loop::main_loop(SDL_Window *window, int width, int height)
  : m_window_width(width), m_window_height(height),

    // init simulator
    simulator(new smoke_sim(SIM_SIZE))
{
  m_renderer = SDL_CreateRenderer(
      window,
      -1,
      SDL_RENDERER_ACCELERATED
      );

  // set blend mode
  SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
}

void main_loop::clean_up() {
  SDL_DestroyRenderer(m_renderer);
}

void main_loop::keydown_callback(const SDL_Scancode scancode) {

  switch (scancode) {
    case SDL_SCANCODE_Q:
      m_continue_loop = false;
      break;
    case SDL_SCANCODE_P:
      m_show_pressure = !m_show_pressure;
      break;
    default:
      // Do nothing
      break;
  }
}

void main_loop::init() {

  // set fluid configuration
  this->simulator
    ->set_diffuse   (15.0f)
    ->set_viscosity (0.1f)
    ->set_density   (1.0f);

  // set pressure
  for (int i = 0; i <= (int) SIM_SIZE; ++i) {
    for (int j = 0; j <= (int) SIM_SIZE; ++j) {
      this->simulator->get_pressure()[i][j] = 0.0f;
    }
  }
  
  // set gravity
  for (int i = 0; i < (int) SIM_SIZE; ++i) {
    for (int j = 0; j < (int) SIM_SIZE; ++j) {
      this->simulator->get_vec_x()[i][j]    = 0.0f;
      this->simulator->get_vec_y()[i][j]    = 0.0f;

      this->simulator->get_force_x()[i][j]  = 0.0f;
      this->simulator->get_force_y()[i][j]  = 0.3f;
    }
  }
}

void main_loop::draw(float dt) {

  static const int R = 0xEE;
  static const int G = 0xEE;
  static const int B = 0xEE;
  uint8_t r, g, b;

  // simulate the model
  static int rocket_x = SIM_SIZE / 2 - 10, rocket_y = 10;
  // rocket_x = (rocket_x + SIM_SIZE - 1) % SIM_SIZE;
  // rocket_y = (rocket_y + SIM_SIZE - 1) % SIM_SIZE;

  this->simulator->get_dens()[rocket_x][rocket_y] += 25;
  this->simulator->get_dens()[rocket_x + 20][rocket_y] += 25;
  this->simulator->simulate(dt / 100.0);

  // render the image
  const size_t  size_x = this->m_window_width  / SIM_SIZE;
  const size_t  pad_x  = this->m_window_width  % SIM_SIZE; 
  const size_t  size_y = this->m_window_height / SIM_SIZE; 
  const size_t  pad_y  = this->m_window_height % SIM_SIZE;
  
  static float min_p = std::numeric_limits<float>::max();
  static float max_p = std::numeric_limits<float>::min();

  for (size_t i = 0; i < SIM_SIZE; ++i) {
    for (size_t j = 0; j < SIM_SIZE; ++j) {

      const int x = i * size_x + std::min(i, pad_x);
      const int y = j * size_y + std::min(j, pad_y);

      // draw density
      SDL_Rect rect {x, y, (int) size_x + (i < pad_x), (int) size_y + (j < pad_y)};
      const uint8_t alpha = std::min(255, (int) floor(this->simulator->get_dens()[i][j] * 256));

      if (this->simulator->get_pressure()[i][j] > max_p) {
        max_p = this->simulator->get_pressure()[i][j];
      }
      if (this->simulator->get_pressure()[i][j] < min_p) {
        min_p = this->simulator->get_pressure()[i][j];
      }

      if (this->m_show_pressure) {
        util::interpolate_color(this->simulator->get_pressure()[i][j], min_p, max_p, &r, &g, &b);
      } else {
        r = R;
        g = G;
        b = B;
      }

      SDL_SetRenderDrawColor (this->m_renderer, r, g, b, this->m_show_pressure ? 255 : alpha); 
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
    SDL_SetRenderDrawColor  (this->m_renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear         (m_renderer);

    this->draw              (DT);

    SDL_RenderPresent       (m_renderer);

    m_loop_tick_start = SDL_GetTicks();
  }
}
