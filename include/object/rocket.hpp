#ifndef OBJECT_ROCKET_HPP
#define OBJECT_ROCKET_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "object/object.hpp"

namespace model {
  class rocket : public object {

  protected:

    SDL_Texture *img;

    float x, y;
    float t;

    float ratio, s;

  public:

    void draw        (int w, int h, SDL_Renderer* renderer) const override;
    void fix_density (double** dens)                        const override;
    void simulate    (float dt)                             override;

    float get_x  () const noexcept override { return x;  }
    float get_y  () const noexcept override { return y;  }

    rocket* set_position (float x, float y);

    rocket  (float x, float y, float s, SDL_Renderer* renderer);

    void cleanup();
    
  };
}

#endif