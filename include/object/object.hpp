#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <SDL2/SDL.h>

// object interface
class object {

  public:
    virtual void draw        (int w, int h, SDL_Renderer* renderer) const = 0;
    virtual void fix_density (float** dens) const  = 0;
    virtual void simulate    (float dt)            = 0;
};

#endif