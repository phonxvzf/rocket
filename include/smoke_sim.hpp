#ifndef SMOKE_SIM_HPP
#define SMOKE_SIM_HPP

#include <cstddef>

class smoke_sim {

	private:

		const int T;

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

	public:

		float** get_dens  () const;
		float** get_vec_x () const;
		float** get_vec_y () const;

		void simulate (float dt);

		// constructors
		smoke_sim (int T);
		smoke_sim (const smoke_sim& sim);
};

void diffuse(int T, float** x, float** x0, float k, float dt);
void advect (int T, float** x, float** x0, float** u, float** v, float dt);

#endif