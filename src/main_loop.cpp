#include <algorithm>
#include <cmath>
#include <limits>
#include <iostream>

#include "main_loop.hpp"
#include "object/rocket.hpp"
namespace util {
  void interpolate_color(double p, double min_p, double max_p, uint8_t* r, uint8_t* g, uint8_t* b) {
    const double h = std::max(0.0, 6 - ((p - min_p) / (max_p - min_p) * 6 + 1));
    const double x = 1 - std::abs(std::fmod(h, 2.0f) - 1);
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
  : m_window_width   (width), 
    m_window_height  (height),

    // init object list
    objs  (),

    // init smoke simulator
    smoke (new smoke_sim(SIM_SIZE))

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
  
  for (object* obj : this->objs) {
    delete obj;
  }
  delete this->smoke;

  SDL_DestroyRenderer(m_renderer);
}

void main_loop::keydown_callback(const SDL_Scancode scancode) {

  switch (scancode) {
    case SDL_SCANCODE_Q:
      m_continue_loop = false;
      break;
    case SDL_SCANCODE_P:
      m_pause = !m_pause;
      break;
    default:
      // Do nothing
      break;
  }
}

void main_loop::init() {

  // create a rocket
  object* obj = dynamic_cast<object*> (new model::rocket(0.5, 1.0, 50, this->m_renderer));
  this->objs.emplace_back(obj);

  // set fluid configuration
  this->smoke
    ->set_diffuse   (5)
    ->set_viscosity (1)
    ->set_density   (0.001);

  // set pressure
  for (int i = 0; i <= (int) SIM_SIZE; ++i) {
    for (int j = 0; j <= (int) SIM_SIZE; ++j) {
      this->smoke->get_pressure()[i][j] = 0.0f;
    }
  }
  
  // set gravity
  for (int i = 0; i < (int) SIM_SIZE; ++i) {
    for (int j = 0; j < (int) SIM_SIZE; ++j) {
      this->smoke->get_vec_x()[i][j]    = 0;
      this->smoke->get_vec_y()[i][j]    = 0;

      this->smoke->get_force_x()[i][j]  = 0.0;
      this->smoke->get_force_y()[i][j]  = 0.3;
    }
  }
}

void main_loop::draw(double dt) {

  static const int R = 0xBB;
  static const int G = 0xBB;
  static const int B = 0xBB;

  // simulate the model
  if (!m_pause) {

    // add smoke from the rocket
    std::pair<int, int> rocket_pos_in_smoke = this->smoke->get_position(
      this->objs[0]->get_x(),
      this->objs[0]->get_y()
    );

    this->smoke->get_dens()
      [rocket_pos_in_smoke.first]
      [rocket_pos_in_smoke.second] += 25;
    this->smoke->get_vec_x()
      [rocket_pos_in_smoke.first]
      [rocket_pos_in_smoke.second] = 0;
    this->smoke->get_vec_y()
      [rocket_pos_in_smoke.first]
      [rocket_pos_in_smoke.second] += 300;
    this->smoke->get_vec_y()
      [rocket_pos_in_smoke.first+1]
      [rocket_pos_in_smoke.second] += 300;

    // simulate smoke
    for (object* obj : this->objs) {
      obj->simulate(dt / 100.0);
      obj->fix_force_x(SIM_SIZE, this->smoke->get_force_x());
      obj->fix_force_y(SIM_SIZE, this->smoke->get_force_y());
    }

    this->smoke->simulate(dt / 100.0);
  }

  // fill background
  const SDL_Rect window_rect = { 0, 0, m_window_width, m_window_height };
  SDL_SetRenderDrawColor (this->m_renderer, 0x00, 0x00, 0x00, 0xFF);
  SDL_RenderFillRect     (this->m_renderer, &window_rect);

  // render the smoke
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
      const uint8_t alpha = std::min(255, (int) floor(this->smoke->get_dens()[i][j] * 256));

      SDL_SetRenderDrawColor (this->m_renderer, R, G, B, alpha); 
      SDL_RenderFillRect     (this->m_renderer, &rect);
    }
  }

  // render the objects
  for (object* obj : this->objs) {
    obj->draw(m_window_width, m_window_height, this->m_renderer);
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
