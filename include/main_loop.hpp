#ifndef MAIN_LOOP_HPP
#define MAIN_LOOP_HPP

#include <cinttypes>
#include <SDL2/SDL.h>

class main_loop {
  private:
    int m_window_width;
    int m_window_height;
    uint32_t m_loop_tick_start;
    bool m_continue_loop = true;
    SDL_Renderer* m_renderer;

    void clean_up();
    void keydown_callback(SDL_Scancode scancode);
    void draw(uint32_t dt);

  public:
    main_loop(SDL_Window* window, int width, int height);
    void start();
};

#endif /* MAIN_LOOP_HPP */
