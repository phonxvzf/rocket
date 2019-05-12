#include "object/globe.hpp"

#include <cmath>

namespace model {

  void globe::draw (int w, int h, SDL_Renderer* renderer) const {
   
    static const int   N   = 100;
    static const float PI  = acos(-1.0f);
    static const float ang = 2 * PI / N;

    SDL_SetRenderDrawColor (renderer, 0x00, 0xFF, 0x00, 0xFF); 

    for (int i = 0; i < N; ++i) {

      const float x1 = w * (this->x + this->r * cos(ang * i));
      const float y1 = h * (this->y + this->r * sin(ang * i));

      const float x2 = w * (this->x + this->r * cos(ang * i + ang));
      const float y2 = h * (this->y + this->r * sin(ang * i + ang));

      SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }  
  }


  void globe::fix_density (double** dens) const {
  }

  void globe::simulate (float dt) {
    // rotate the object by dt
    static const float omega = 10.0f;
    this->ang += omega * dt;
  }

  globe::globe (float x, float y, float r) : x(x), y(y), r(r), ang(0.0f) {} 

  void globe::cleanup () {}
}