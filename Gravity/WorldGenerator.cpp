#include "WorldGenerator.h"
#include<iostream>
#include<time.h>

#define MAX_VEL_MAG 250
#define MAX_CLUSTERS 10

World::World() {

	std::cout << "Generating Particles..." << std::endl;

	for (int i = 0; i < Constant::NUM_PARTICLES; i++) {
		particles[i] = Particle();
	}

}

World::World(int generator, int screenWidth, int screenHeight) {

	srand(time(NULL));

	SCR_W = screenWidth;
	SCR_H = screenHeight;

	std::cout << "Generating Particles..." << std::endl;

	if (generator == Generator::DEFAULT) {
		for (int i = 0; i < Constant::NUM_PARTICLES; i++) {
			particles[i] = Particle();
		}
	} else if (generator == Generator::CIRCULAR) {

		int cX = SCR_W / 2, cY = SCR_H / 2;
		int rad = cX > cY ? cY : cX;

		rad = Generator::SPAWN_RANGE < rad ? Generator::SPAWN_RANGE : rad;

		for (int i = 0; i < Constant::NUM_PARTICLES; i++) {

			float spawn[2] = { 0, 0 };

			float deg = ((double)rand() / (RAND_MAX)) * 2 * 3.14151926;
			float randRad = rand() % rad;

			spawn[0] = randRad * cos(deg) + cX;
			spawn[1] = randRad * sin(deg) + cY;

			float vel[2] = { 0,0 };
			float velMag = rand() % 30;
			vel[0] = -velMag * sin(deg);
			vel[1] = velMag * cos(deg);

			particles[i] = Particle(spawn, vel);
		}


	} else if (generator == Generator::RECTANGULAR) {

		int cX = SCR_W / 2, cY = SCR_H / 2;

		int dX = Generator::SPAWN_RANGE < cX ? Generator::SPAWN_RANGE : cX;
		int dY = Generator::SPAWN_RANGE < cY ? Generator::SPAWN_RANGE : cY;

		for (int i = 0; i < Constant::NUM_PARTICLES; i++) {

			float spawn[2] = { 0, 0 };

			float x = (2 * ((double)rand() / (RAND_MAX)) - 1) * dX;
			float y = (2 * ((double)rand() / (RAND_MAX)) - 1) * dY;

			spawn[0] = x + cX;
			spawn[1] = y + cY;

			particles[i] = Particle(spawn);
		}

	} else if (generator == Generator::CLUSTERED) {

		std::cout << "Generating Clusters" << std::endl;

		int clusters = rand() % MAX_CLUSTERS + 1;

		int upperLimit = Constant::NUM_PARTICLES / clusters;
		int used = 0;


		for (int i = 0; i < clusters; i++) {

			float clusterX = rand() % SCR_W;
			float clusterY = rand() % SCR_H;
			
			int rad = rand() % Generator::SPAWN_RANGE / 8 + 1;

			float velMag = rand() % 20;
			float linearX = (rand() % 10) - 5;
			float linearY = (rand() % 10) - 5;

			for (int j = 0; j < upperLimit; j++) {

				float spawn[2] = { 0, 0 };

				float deg = ((double)rand() / (RAND_MAX)) * 2 * 3.14151926;
				float randRad = rand() % rad;

				spawn[0] = randRad * cos(deg) + clusterX;
				spawn[1] = randRad * sin(deg) + clusterY;

				float vel[2] = { 0,0 };
				
				vel[0] = -velMag * sin(deg);
				vel[1] = velMag * cos(deg);

				if ((rand() % 10) > 5) {
					vel[0] = -vel[0];
					vel[1] = -vel[1];
				}

				vel[0] += linearX;
				vel[1] += linearY;



				if (used < Constant::NUM_PARTICLES) {
					particles[used++] = Particle(spawn, vel);
				}
			}
			

			
		
		}

	}

}

World::~World() {
	free(this);
}


void World::render(SDL_Renderer* window_renderer, int offsetX, int offsetY, float zoom) {
	SDL_RenderClear(window_renderer);

	for (int i = 0; i < Constant::NUM_PARTICLES; i++) {
		SDL_Rect rect;
		rect.w = this->particles[i].mass / 4 + 1;
		rect.h = this->particles[i].mass / 4 + 1;
		rect.x = zoom * (this->particles[i].position[0]) + offsetX - rect.w/2;
		rect.y = zoom * (this->particles[i].position[1]) + offsetY - rect.h/2;


		float velMag = this->particles[i].velocity[0] * this->particles[i].velocity[0] + this->particles[i].velocity[1] * this->particles[i].velocity[1];

		if (velMag > MAX_VEL_MAG) {
			velMag = MAX_VEL_MAG;
		}

		velMag /= MAX_VEL_MAG * 2;

		//rect.x = 500;
		//rect.y = 300;
		//rect.w = 10;
		//rect.h = 10;

		SDL_SetRenderDrawColor(window_renderer, velMag * 225 + rand() % 30, velMag * 255,  velMag * 150 + 20, 255);
		//SDL_SetRenderDrawColor(window_renderer, velMag * 225 + rand() % 30, velMag * velMag * 150 + rand() % 50, rand() % 20, 255);
		SDL_RenderFillRect(window_renderer, &rect);

	}

	SDL_SetRenderDrawColor(window_renderer, 0, 0, 0, 255);

	SDL_RenderPresent(window_renderer);
}
