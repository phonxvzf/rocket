#ifndef SMOKE_SIM_HPP
#define SMOKE_SIM_HPP

#include <cstddef>

class smoke_sim {

	private:

		const int T;

		float   diffuse_rate;
		float   viscosity;

		float** tmp_vec_x;
		float** tmp_vec_y;
		float** tmp_dens;

		float** vec_x;
		float** vec_y;
		float** dens;

		// overridable 
		virtual void evolve_vec_x () {};
		virtual void evolve_vec_y () {};
		virtual void evolve_dens  () {};

		void evolve_vec  (float dt);
		void evolve_dens (float dt);
		void project     ();

	public:

		float** get_dens  () const noexcept;
		float** get_vec_x () const noexcept;
		float** get_vec_y () const noexcept;

		void simulate (float dt);

		smoke_sim* set_diffuse  (float rate) noexcept;
		smoke_sim* set_viscosity (float rate) noexcept;

		// constructors
		smoke_sim (int T);
		smoke_sim (const smoke_sim& sim);
};
	
#endif