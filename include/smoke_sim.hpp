#ifndef SMOKE_SIM_HPP
#define SMOKE_SIM_HPP

#include <cstddef>

class smoke_sim {

  private:

    const int T;

    double   diffuse_rate;
    double   viscosity;
    double   density;

    double** tmp_vec_x;
    double** tmp_vec_y;
    double** tmp_dens;

    double** vec_x;
    double** vec_y;
    double** dens;
    double** pressure;
    double** force_x;
    double** force_y;

    // overridable 
    virtual void evolve_vec_x () {};
    virtual void evolve_vec_y () {};
    virtual void evolve_dens  () {};

    void evolve_vec  (double dt);
    void evolve_dens (double dt);

  public:

    double** get_dens     () const noexcept;
    double** get_pressure () const noexcept;
    double** get_vec_x    () const noexcept;
    double** get_vec_y    () const noexcept;
    double** get_force_x  () const noexcept;
    double** get_force_y  () const noexcept;

    void simulate (double dt);

    smoke_sim* set_diffuse    (double rate)    noexcept;
    smoke_sim* set_viscosity  (double rate)    noexcept;
    smoke_sim* set_density    (double density) noexcept;

    // constructors
    smoke_sim (int T);
    smoke_sim (const smoke_sim& sim);
};
  
#endif
