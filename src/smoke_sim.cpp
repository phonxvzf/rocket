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

  static const double MAX_VELOCITY = 5000000.0;

  inline double trace_position (double T, double cx, double v, double dt) {
    double x = cx + v * dt;
    /*
    while (0 > x || x > T) {
      if (0 > x) x = - x;
      if (x > T) x = 2 * T - x;
    }
    */
    return x;
  }

  inline bool valid(int T, int i) { return 0 < i && i < T; }

  // fluid advection
  void advect (int T, double** x, double** x0, double** u, double** v, double dt) {

    for (int i = 0; i < T; ++i) {
      for (int j = 0; j < T; ++j) {
        double cx = i + 0.5, cy = j + 0.5;
        
        double cu = (u[i][j] + u[i+1][j]) / 2.0;
        double cv = (v[i][j] + v[i][j+1]) / 2.0;

        double px = trace_position (T, cx, cu, -dt);
        double py = trace_position (T, cy, cv, -dt);

        int i0   = (int) floor(px - 0.5f);
        int i1   = i0 + 1;
        int j0   = (int) floor(py - 0.5f);
        int j1   = j0 + 1;

        double s1 =   px - i0 - 0.5;
        double s0 = 1.0 - s1;
        double t1 =   py - j0 - 0.5;
        double t0 = 1.0 - t1;

        x[i][j]  =  (valid(T, i0) && valid(T, j0) ? s0 * t0 * x0[i0][j0] : 0.0) 
                  + (valid(T, i0) && valid(T, j1) ? s0 * t1 * x0[i0][j1] : 0.0)
                  + (valid(T, i1) && valid(T, j0) ? s1 * t0 * x0[i1][j0] : 0.0)
                  + (valid(T, i1) && valid(T, j1) ? s1 * t1 * x0[i1][j1] : 0.0);
      }
    }
  }

  void diffuse (int T, double** x, double** x0, double k, double dt) {
    static const int iteration = 20;

    const double coef = k * dt;
    for (int it = 0; it < iteration; ++it) {
      for (int i = 0; i < T; ++i) {
        for (int j = 0; j < T; ++j) {
          // const int bound = (i == 0) + (i+1 == T) + (j == 0) + (j+1 == T);
          const int bound = (i == 0) + (i+1 == T) + (j+1 == T);
          x[i][j] = (x0[i][j] + coef * (
                (i   > 0 ? x[i-1][j] : 0.0) +
                (i+1 < T ? x[i+1][j] : 0.0) +
                (j   > 0 ? x[i][j-1] : 0.0) +
                (j+1 < T ? x[i][j+1] : 0.0)
                )) / (coef * (4 - bound) + 1);
        }
      }
    }
  }

  void pressure(int T, double** p, double** w_x, double** w_y, double** w_x0, double** w_y0, double density) {
    // calculate gradient of scalar field p using Gauss-Seidel method
    static const int iteration = 20;

    for (int it = 0; it < iteration; ++it) {
      for (int i = 0; i < T; ++i) {
        for (int j = 0; j < T; ++j) {
          // Neumann boundary condition will transform each affected neighbour to p[i][j]
          const double div_w = (w_x0[i+1][j] - w_x0[i][j]) + (w_y0[i][j+1] - w_y0[i][j]);
          // const int bound = (i == 0) + (i+1 == T) + (j == 0) + (j+1 == T);
          const int bound = (i == 0) + (i+1 == T) + (j+1 == T);

          p[i][j] = (density * div_w - (
                (i   > 0 ? p[i-1][j] : 0.0) +
                (i+1 < T ? p[i+1][j] : 0.0) +
                (j   > 0 ? p[i][j-1] : 0.0) +
                (j+1 < T ? p[i][j+1] : 0.0)
                )) / (bound - 4);
        }
      }
    }

    for (int i = 0; i < T; ++i) {
      for (int j = 0; j < T; ++j) {
        // update velocity field according to Helmholtz-Hodge decomposition
        const double grad_x = (i+1 == T) ? 0 : p[i+1][j] - p[i][j];
        const double grad_y = (j+1 == T) ? 0 : p[i][j+1] - p[i][j];
        w_x[i][j] = std::clamp(w_x0[i][j] - grad_x / density, -MAX_VELOCITY, MAX_VELOCITY);
        w_y[i][j] = std::clamp(w_y0[i][j] - grad_y / density, -MAX_VELOCITY, MAX_VELOCITY);
      }
    }
  }

  void body_force(
      int T,
      double** u,
      double** u0,
      double** force,
      double dt)
  {
    for (int i = 0; i < T; ++i) {
      for (int j = 0; j < T; ++j) {
        u[i][j] = u0[i][j] + force[i][j] * dt;
      }
    }
  }

}

double** smoke_sim::get_dens () const noexcept {
  return this->dens;
}

double** smoke_sim::get_pressure () const noexcept {
  return this->pressure;
}

double** smoke_sim::get_vec_x () const noexcept {
  return this->vec_x;
}

double** smoke_sim::get_vec_y () const noexcept {
  return this->vec_y;
}

double** smoke_sim::get_force_x () const noexcept {
  return this->force_x;
}

double** smoke_sim::get_force_y () const noexcept {
  return this->force_y;
}

std::pair<int, int> smoke_sim::get_position (float x, float y) const noexcept {
  return { (int) (x * this->T), (int) (y * this->T) };
}

smoke_sim* smoke_sim::set_diffuse (float rate) noexcept {
  this->diffuse_rate = rate;
  return this;
}

smoke_sim* smoke_sim::set_viscosity (float rate) noexcept {
  this->viscosity = rate;
  return this;
}

smoke_sim* smoke_sim::set_density (double density) noexcept {
  this->density = density;
  return this;
}

void smoke_sim::evolve_vec  (double dt) {

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

void smoke_sim::evolve_dens (double dt) {
  fluid::advect   (this->T, this->tmp_dens, this->dens, this->vec_x, this->vec_y, dt);
  std::swap       (this->tmp_dens, this->dens);

  fluid::diffuse  (this->T, this->tmp_dens, this->dens, this->diffuse_rate, dt);
  std::swap       (this->tmp_dens, this->dens);
}

void smoke_sim::simulate (double dt) {
  this->evolve_vec  (dt);
  this->evolve_dens (dt);
}

smoke_sim::smoke_sim (int T) : T(T), diffuse_rate(10), viscosity(10) {
  tmp_vec_x = new double*[T+1];
  tmp_vec_y = new double*[T+1];
  tmp_dens  = new double*[T+1];
  vec_x     = new double*[T+1];
  vec_y     = new double*[T+1];
  dens      = new double*[T+1];
  pressure  = new double*[T+1];
  force_x   = new double*[T+1];
  force_y   = new double*[T+1];
  for (int i = 0; i < T+1; ++i) {
    tmp_vec_x[i] = new double[T+1]();
    tmp_vec_y[i] = new double[T+1]();
    tmp_dens [i] = new double[T+1]();
    vec_x    [i] = new double[T+1]();
    vec_y    [i] = new double[T+1]();
    dens     [i] = new double[T+1]();
    pressure [i] = new double[T+1]();
    force_x  [i] = new double[T+1]();
    force_y  [i] = new double[T+1]();
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

smoke_sim::~smoke_sim () {
  for (int i = 0; i < T+1; ++i) {
    delete[] tmp_dens[i];
    delete[] tmp_vec_x[i];
    delete[] tmp_vec_y[i];

    delete[] dens[i];
    delete[] vec_x[i];
    delete[] vec_y[i];
    delete[] pressure[i];
  }

  delete[] tmp_dens;
  delete[] tmp_vec_x;
  delete[] tmp_vec_y;
  delete[] dens;
  delete[] vec_x;
  delete[] vec_y;
  delete[] pressure;
}
