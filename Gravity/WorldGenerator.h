#ifndef WORLD_H
#define WORLD_H

#include "Constants.cpp"
#include "Particle.h"
#include "SDL.h"

class World {

	
	public:

		World();
		World(int generator, int screenWidth, int screenHeight);

		~World();
		
		void render(SDL_Renderer* window_renderer, int offsetX, int offsetY);

		Particle particles[Constant::NUM_PARTICLES];

		int SCR_W, SCR_H;

	

};


#endif //PARTICLE_H