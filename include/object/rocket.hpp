#ifndef OBJECT_ROCKET_HPP
#define OBJECT_ROCKET_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <utility>

#include "object/object.hpp"

namespace model {
  class rocket : public object {

    protected:

      SDL_Texture* img;
      float x, y;
      float vx, vy;
      float t;

      float ratio;
      int s;

    public:

      void draw        (int w, int h, SDL_Renderer* renderer) const override;
      void fix_force_x (int T, double** vx)                   const override;
      void fix_force_y (int T, double** vy)                   const override;
      void simulate    (float dt)                             override;

      float get_x  () const noexcept { return x;  }
      float get_y  () const noexcept { return y;  }

      std::pair<int, int> get_smoke_position (int T) const noexcept;
      rocket* set_position (float x, float y);

      rocket (float x, float y, int s, SDL_Renderer* renderer);

      void cleanup ();
  };
}

#endif