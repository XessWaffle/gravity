#ifndef GRAVITY_KERNEL_CUH
#define GRAVITY_KERNEL_CUH


#include "Constants.cpp"
#include "WorldGenerator.h"
#include "Particle.h"

class GravityKernel {

public:
	GravityKernel();
	GravityKernel(World* world);

	void cudaPrep();
	void runKernel();
	void cudaClear();

private:

	World* _world;

	Particle* d_particles;
	float* d_acc_sum;
	float** d_positions;
	float** d_velocities;



};


#endif