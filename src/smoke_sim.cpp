#include <cmath>
#include <algorithm>
#include <iostream>

#include "smoke_sim.hpp"

namespace std {
  template <class T>
  constexpr const T& clamp(const T& v, const T& lo, const T& hi) {
    return std::min<T>(hi, std::max<T>(lo, v));
  }
}

namespace fluid {

  static const float MAX_VELOCITY = 20.0f;

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

    const float coef = -k * dt;
    for (int it = 0; it < iteration; ++it) {
      for (int i = 0; i < T; ++i) {
        for (int j = 0; j < T; ++j) {
          x[i][j] = (x0[i][j] - (
                (i   > 0 ? coef * x[i-1][j] : 0) +
                (i+1 < T ? coef * x[i+1][j] : 0) +
                (j   > 0 ? coef * x[i][j-1] : 0) +
                (j+1 < T ? coef * x[i][j+1] : 0)
                )) / (1 - 4 * coef);
        }
      }
    }
  }

  void pressure(int T, float** p, float** w_x, float** w_y, float** w_x0, float** w_y0, float density) {
    // calculate gradient of scalar field p using Gauss-Seidel method
    static const int iteration = 30;

    for (int it = 0; it < iteration; ++it) {
      for (int i = 0; i < T; ++i) {
        for (int j = 0; j < T; ++j) {
          const float div_w = (w_x0[i+1][j] - w_x0[i][j]) + (w_y0[i][j+1] - w_y0[i][j]);

          p[i][j] = (density * div_w - (
                (i   > 0 ? p[i-1][j] : 0) +
                (i+1 < T ? p[i+1][j] : 0) +
                (j   > 0 ? p[i][j-1] : 0) +
                (j+1 < T ? p[i][j+1] : 0)
                )) / -4.0f;
        }
      }
    }

    for (int i = 0; i < T; ++i) {
      for (int j = 0; j < T; ++j) {
        // update velocity field according to Helmholtz-Hodge decomposition
        if (i+1 < T) w_x[i][j] = w_x0[i][j] - (p[i+1][j] - p[i][j]) * density;
        if (j+1 < T) w_y[i][j] = w_y0[i][j] - (p[i][j+1] - p[i][j]) * density;
      }
    }
  }

  void body_force(
      int T,
      float** u,
      float** u0,
      float** force,
      float dt)
  {
    for (int i = 0; i < T; ++i) {
      for (int j = 0; j < T; ++j) {
        u[i][j] = std::min(u0[i][j] + force[i][j] * dt, MAX_VELOCITY);
      }
    }
  }

}

float** smoke_sim::get_dens () const noexcept {
  return this->dens;
}

float** smoke_sim::get_pressure () const noexcept {
  return this->pressure;
}

float** smoke_sim::get_vec_x () const noexcept {
  return this->vec_x;
}

float** smoke_sim::get_vec_y () const noexcept {
  return this->vec_y;
}

float** smoke_sim::get_force_x () const noexcept {
  return this->force_x;
}

float** smoke_sim::get_force_y () const noexcept {
  return this->force_y;
}

smoke_sim* smoke_sim::set_diffuse (float rate) noexcept {
  this->diffuse_rate = rate;
  return this;
}

smoke_sim* smoke_sim::set_viscosity (float rate) noexcept {
  this->viscosity = rate;
  return this;
}

smoke_sim* smoke_sim::set_density (float density) noexcept {
  this->density = density;
  return this;
}

void smoke_sim::evolve_vec  (float dt) {

  fluid::advect     (this->T, this->tmp_vec_x, this->vec_x, this->vec_x, this->vec_y, dt);
  fluid::advect     (this->T, this->tmp_vec_y, this->vec_y, this->vec_x, this->vec_y, dt);
  std::swap         (this->tmp_vec_x, this->vec_x);
  std::swap         (this->tmp_vec_y, this->vec_y);

  fluid::body_force (this->T, this->tmp_vec_x, this->vec_x, this->force_x, dt);
  fluid::body_force (this->T, this->tmp_vec_y, this->vec_y, this->force_y, dt);
  std::swap         (this->tmp_vec_x, this->vec_x);
  std::swap         (this->tmp_vec_y, this->vec_y);

  fluid::diffuse    (this->T, this->tmp_vec_x, this->vec_x, this->viscosity, dt);
  fluid::diffuse    (this->T, this->tmp_vec_y, this->vec_y, this->viscosity, dt);
  std::swap         (this->tmp_vec_x, this->vec_x);
  std::swap         (this->tmp_vec_y, this->vec_y);

  // enforce divergence free of velocity field
  // pressure is solved as a by-product
  fluid::pressure   (
      this->T,
      this->pressure,
      this->tmp_vec_x,
      this->tmp_vec_y,
      this->vec_x,
      this->vec_y,
      this->density
      );
  std::swap         (this->tmp_vec_x, this->vec_x);
  std::swap         (this->tmp_vec_y, this->vec_y);
}

void smoke_sim::evolve_dens (float dt) {
  fluid::advect   (this->T, this->tmp_dens, this->dens, this->vec_x, this->vec_y, dt);
  std::swap       (this->tmp_dens, this->dens);

  fluid::diffuse  (this->T, this->tmp_dens, this->dens, this->diffuse_rate, dt);
  std::swap       (this->tmp_dens, this->dens);
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
  pressure  = new float*[T+1];
  force_x   = new float*[T+1];
  force_y   = new float*[T+1];
  for (int i = 0; i < T+1; ++i) {
    tmp_vec_x[i] = new float[T+1]();
    tmp_vec_y[i] = new float[T+1]();
    tmp_dens [i] = new float[T+1]();
    vec_x    [i] = new float[T+1]();
    vec_y    [i] = new float[T+1]();
    dens     [i] = new float[T+1]();
    pressure [i] = new float[T+1]();
    force_x  [i] = new float[T+1]();
    force_y  [i] = new float[T+1]();
  }
}

smoke_sim::smoke_sim (const smoke_sim& sim) : T(sim.T), diffuse_rate(10), viscosity(10) {
  this->tmp_vec_x = sim.tmp_vec_x;
  this->tmp_vec_y = sim.tmp_vec_y;
  this->tmp_dens  = sim.tmp_dens;

  this->vec_x = sim.vec_x;
  this->vec_y = sim.vec_y;
  this->dens  = sim.dens;
  this->pressure = sim.pressure;
}
