#ifndef PARTICLE_H
#define PARTICLE_H

#include "Constants.cpp"

class Particle {

	public:

		Particle();

		Particle(float* spawn);

		Particle(float* spawn, float* vel);

		float* getPosition();
		float* getVelocity();

		void setPosition(float* position);
		void setVelocity(float* velocity);

		int getMass();

		float* position;
		float* velocity;
		float* acceleration;

		int mass;
};

#endif //PARTICLE_H