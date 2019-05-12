#ifndef OBJECT_GLOBE_HPP
#define OBJECT_GLOBE_HPP

#include "object/object.hpp"

namespace model {
  
  class globe : public object {

    protected:
      const float x, y, r;
      float ang;

    public:

      void draw        (int w, int h, SDL_Renderer* renderer) const override;
      void fix_density (double** dens)                        const override;
      void simulate    (float dt)                             override;

      float get_x  () const noexcept { return x;  }
      float get_y  () const noexcept { return y;  }

      globe (float x, float y, float r);

      void cleanup ();
  };
}

#endif
