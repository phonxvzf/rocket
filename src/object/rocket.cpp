#include "object/rocket.hpp"

#include <iostream>
void rocket::draw (int w, int h, SDL_Renderer* renderer) const {
  SDL_Rect rect {
    (int) (w * this->x - 10), 
    (int) (h * this->y - 10), 
    20,
    20
  };

  SDL_SetRenderDrawColor (renderer, 0xFF, 0x00, 0x00, 0xFF); 
  SDL_RenderFillRect     (renderer, &rect);
}


void rocket::fix_density (float** dens) const {

}

void rocket::simulate (float dt) {
  // this->x = 0.5;
  this->x += dt / 1000.0f;
  if (this->x > 1) this->x -= 1.0f;
  this->y -= dt / 1000.0f;
  if (this->y < 0) this->y += 1.0f;
}

rocket* rocket::set_position (float x, float y) {
  this->x = x;
  this->y = y;
  return this;
}

rocket::rocket (float x, float y) : x(x), y(y) {}