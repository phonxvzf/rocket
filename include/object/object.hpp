#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <SDL2/SDL.h>

// object interface
class object {

  public:
    virtual void draw        (int w, int h, SDL_Renderer* renderer) const = 0;
    virtual void fix_force_x (int T, double** vx) const  = 0;
    virtual void fix_force_y (int T, double** vy) const  = 0;
    virtual void simulate    (float dt)             = 0;

    virtual float get_x  () const = 0; 
    virtual float get_y  () const = 0;

    virtual void cleanup ();

    object  ();
    virtual ~object ();
};

#endif