#include <cmath>
#include <algorithm>

#include "smoke_sim.hpp"

namespace std {
  template <class T>
  constexpr const T& clamp(const T& v, const T& lo, const T& hi) {
    return std::min<T>(hi, std::max<T>(lo, v));
  }
}

namespace fluid {

  inline float trace_position (float T, float cx, float v, float dt) {
    float x = cx + v * dt;
    while (0 > x || x > T) {
      if (0 > x) x = - x;
      if (x > T) x = 2 * T - x;
    }
    return x;
  }

  inline bool valid(int T, int i) { return 0 <= i && i < T; }

  // fluid advection
  void advect (int T, float** x, float** x0, float** u, float** v, float dt) {

    for (int i = 0; i < T; ++i) {
      for (int j = 0; j < T; ++j) {
        float cx = i + 0.5f, cy = j + 0.5f;
        
        float cu = (u[i][j] + u[i+1][j]) / 2.0;
        float cv = (v[i][j] + v[i][j+1]) / 2.0;

        float px = trace_position (T, cx, cu, -dt);
        float py = trace_position (T, cy, cv, -dt);

        int i0   = (int) floor(px - 0.5f);
        int i1   = i0 + 1;
        int j0   = (int) floor(py - 0.5f);
        int j1   = j0 + 1;

        float s1 =   px - i0 - 0.5f;
        float s0 = 1.0f - s1;
        float t1 =   py - j0 - 0.5f;
        float t0 = 1.0f - t1;

        x[i][j]  =  (valid(T, i0) && valid(T, j0) ? s0 * t0 * x0[i0][j0] : 0.0f) 
                  + (valid(T, i0) && valid(T, j1) ? s0 * t1 * x0[i0][j1] : 0.0f)
                  + (valid(T, i1) && valid(T, j0) ? s1 * t0 * x0[i1][j0] : 0.0f)
                  + (valid(T, i1) && valid(T, j1) ? s1 * t1 * x0[i1][j1] : 0.0f);
      }
    }
  }

  void diffuse (int T, float** x, float** x0, float k, float dt) {
    static const int iteration = 20;

    const float a = k * dt;

    // solve the linear system using Gauss-Seidel method
    for (int it = 0; it < iteration; ++it) {
      for (int i = 0; i < T; ++i) {
        for (int j = 0; j < T; ++j) {
          const int neighbor = (i > 0) + (i < T-1) + (j > 0) + (j < T-1);

          x[i][j] = (x0[i][j] + a * (
            (i+1 < T ? x[i+1][j] : 0) + 
            (i   > 0 ? x[i-1][j] : 0) + 
            (j+1 < T ? x[i][j+1] : 0) + 
            (j   > 0 ? x[i][j-1] : 0) )
          ) / (1 + neighbor * a);
        }
      }
    }
  }
}

float** smoke_sim::get_dens () const noexcept {
  return this->dens;
}

float** smoke_sim::get_vec_x () const noexcept {
  return this->vec_x;
}

float** smoke_sim::get_vec_y () const noexcept {
  return this->vec_y;
}

smoke_sim* smoke_sim::set_diffuse (float rate) noexcept {
  this->diffuse_rate = rate;
  return this;
}

smoke_sim* smoke_sim::set_viscosity (float rate) noexcept {
  this->viscosity = rate;
  return this;
}

void smoke_sim::evolve_vec  (float dt) {

  fluid::diffuse  (this->T, this->tmp_vec_x, this->vec_x, this->viscosity, dt);
  std::swap       (this->tmp_vec_x, this->vec_x);

  fluid::diffuse  (this->T, this->tmp_vec_y, this->vec_y, this->viscosity, dt);
  std::swap       (this->tmp_vec_y, this->vec_y);

  this->project();

  fluid::advect   (this->T, this->tmp_vec_x, this->vec_x, this->vec_x, this->vec_y, dt);
  fluid::advect   (this->T, this->tmp_vec_y, this->vec_y, this->vec_x, this->vec_y, dt);

  this->project();
}

void smoke_sim::evolve_dens (float dt) {
  // body_force (dt);
  // TODO

  fluid::advect   (this->T, this->tmp_dens, this->dens, this->vec_x, this->vec_y, dt);
  std::swap       (this->tmp_dens, this->dens);

  fluid::diffuse  (this->T, this->tmp_dens, this->dens, this->diffuse_rate, dt);
  std::swap       (this->tmp_dens, this->dens);
  
  // pressure   (dt);
  // TODO
}

void smoke_sim::project () {
  for (int i = 0; i < this->T; ++i) {
    for (int j = 0; j < this->T; ++j) {
      // div[i][j] = -0.5 * h, 
    }
  }
}

void smoke_sim::simulate (float dt) {
  this->evolve_vec  (dt);
  this->evolve_dens (dt);
}

smoke_sim::smoke_sim (int T) : T(T), diffuse_rate(10), viscosity(10) {
  tmp_vec_x = new float*[T+1];
  tmp_vec_y = new float*[T+1];
  tmp_dens  = new float*[T+1];
  vec_x     = new float*[T+1];
  vec_y     = new float*[T+1];
  dens      = new float*[T+1];
  for (int i = 0; i < T+1; ++i) {
    tmp_vec_x[i] = new float[T+1]();
    tmp_vec_y[i] = new float[T+1]();
    tmp_dens [i] = new float[T+1]();
    vec_x    [i] = new float[T+1]();
    vec_y    [i] = new float[T+1]();
    dens     [i] = new float[T+1]();
  }
}

smoke_sim::smoke_sim (const smoke_sim& sim) : T(sim.T), diffuse_rate(10), viscosity(10) {
  this->tmp_vec_x = sim.tmp_vec_x;
  this->tmp_vec_y = sim.tmp_vec_y;
  this->tmp_dens  = sim.tmp_dens;

  this->vec_x = sim.vec_x;
  this->vec_y = sim.vec_y;
  this->dens  = sim.dens;
}
