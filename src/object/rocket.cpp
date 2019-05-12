#include "object/rocket.hpp"

namespace model {

  void rocket::draw (int WIDTH, int HEIGHT, SDL_Renderer* renderer) const {
    // SDL_Rect rect {
    //   (int) (w * this->x - 10), 
    //   (int) (h * this->y - 10), 
    //   20,
    //   20
    // };

    // SDL_SetRenderDrawColor (renderer, 0xFF, 0x00, 0x00, 0xFF); 
    // SDL_RenderFillRect     (renderer, &rect);


    SDL_Rect texr { 
      (int) (  WIDTH * this->x - (this->s * this->ratio) / 2.0), 
      (int) ( HEIGHT * this->y - (this->s) + 5), 
      (int) (this->s * this->ratio), 
      (int) (this->s)
    }; // texr.w = this->; texr.h = h*2; 

    SDL_RenderCopy(renderer, img, NULL, &texr);  
  }


  void rocket::fix_density (double** dens) const {
  }

  void rocket::simulate (float dt) {
    this->y -= dt / 1000;
  }

  rocket* rocket::set_position (float x, float y) {
    this->x = x;
    this->y = y;
    return this;
  }

  rocket::rocket  (float x, float y, float s, SDL_Renderer *renderer) : img(nullptr), x(x), y(y), s(s) {
    this->img = IMG_LoadTexture(renderer, "./rocket.png");

    int w, h;
    SDL_QueryTexture(this->img, NULL, NULL, &w, &h); // get the width and height of the texture

    this->ratio = (float) w / h;
  }

  void rocket::cleanup () {
    // delete this->img; 
  }
}
