#include <algorithm>
#include <cmath>

#include "main_loop.hpp"
#include "object/rocket.hpp"
#include "object/globe.hpp"

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
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
      );

  // set blend mode
  SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

  // TODO: sdl_exception
}

void main_loop::clean_up() {
  
  for (object* obj : this->objs) {
    delete obj;
  }
  delete this->smoke;

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

void main_loop::init() {

  // create a rocket
  object* rocket = dynamic_cast<object*> (new model::rocket(0.5, 0.3));
  this->objs.emplace_back (rocket);

  // create a globe
  object* globe  = dynamic_cast<object*> (new model::globe(0.5, 0.5, 0.2));
  this->objs.emplace_back (globe);

  // set fluid configuration
  this->smoke
    ->set_diffuse   (10)
    ->set_viscosity (1);
  
  // set gravity
  for (int i = 0; i <= (int) SIM_SIZE; ++i) {
    for (int j = 0; j <= (int) SIM_SIZE; ++j) {
      // this->smoke->get_vec_x()[i][j] = (0 <= j && j < 50 ? -3.0f : -5.0f);
      this->smoke->get_vec_y()[i][j] = 10.0f;
    }
  }
}

void main_loop::draw(uint32_t dt) {

  static const int R = 0xEE;
  static const int G = 0xEE;
  static const int B = 0xEE;

  // simulate the model

  const model::rocket* rock = dynamic_cast<model::rocket*> (this->objs[0]);
  
  std::pair<int, int> rocket_pos_in_smoke = this->smoke->get_position(
    rock->get_x(),
    rock->get_y()
  );

  this->smoke->get_dens()
    [rocket_pos_in_smoke.first]
    [rocket_pos_in_smoke.second] += 10;

  this->smoke->simulate(dt / 100.0);

  for (object* obj : this->objs) {
    obj->simulate(dt);
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
    SDL_RenderClear(m_renderer);
    draw(SDL_GetTicks() - m_loop_tick_start);
    SDL_RenderPresent(m_renderer);

    m_loop_tick_start = SDL_GetTicks();
  }
}
