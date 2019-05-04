#ifndef OBJECT_ROCKET_HPP
#define OBJECT_ROCKET_HPP

#include "object/object.hpp"

class rocket : public object {

  protected:
    float x, y;
    float vx, vy;
    float t;

  public:

    void draw        (int w, int h, SDL_Renderer* renderer) const override;
    void fix_density (float** dens)                         const override;
    void simulate    (float dt)                             override;

    float get_x  () const noexcept override { return x;  }
    float get_y  () const noexcept override { return y;  }


    float get_vx () { return vx; }
    float get_vy () { return vy; }
    rocket* set_position (float x, float y);

    rocket (float x, float y);
};

#endif