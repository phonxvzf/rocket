#ifndef SDL_EXCEPTION_HPP
#define SDL_EXCEPTION_HPP

#include <stdexcept>
#include <SDL2/SDL_error.h>

class sdl_exception : public std::exception {
  private:
    std::string m_message;
  public:
    sdl_exception(const std::string& message)
      : m_message(message + ": " + std::string(SDL_GetError())) {}
    const char* what() const noexcept { return m_message.c_str(); };
};

#endif /* SDL_EXCEPTION_HPP */
