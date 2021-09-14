
#include "GravityKernel.cuh"
#include "WorldGenerator.h"

#undef main

#include <iostream>

#define SCREEN_HEIGHT 800
#define SCREEN_WIDTH 1500


bool isDragging = false;

int offsetX = 0;
int offsetY = 0;

float zoom = 1;

int mPosXOne = SCREEN_WIDTH / 2;
int mPosYOne = SCREEN_HEIGHT / 2;

int mPosXTwo = -1;
int mPosYTwo = -1;

int main() {

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow("Gravity", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	SDL_Renderer* window_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (!window)
	{
		std::cout << "Failed to create window\n";
		return -1;
	}

	SDL_Surface* window_surface = SDL_GetWindowSurface(window);

	if (!window_surface)
	{
		std::cout << "Failed to get the surface from the window\n";
		return -1;
	}

	SDL_UpdateWindowSurface(window);

	World* world = new World(Generator::CLUSTERED, SCREEN_WIDTH, SCREEN_HEIGHT);
	GravityKernel gk = GravityKernel(world);
	
	std::cout << "Prepping CUDA" << std::endl;

	gk.cudaPrep();

	std::cout << "Running Kernel" << std::endl;

	bool keep_window_open = true;
	while (keep_window_open)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e) > 0)
		{
			switch (e.type)
			{
				case SDL_QUIT:
					keep_window_open = false;
					break;

				case SDL_MOUSEWHEEL:

					zoom += e.wheel.y > 0 ? 0.1 : -0.1;

					break;

				case SDL_KEYDOWN:
	
					//Select surfaces based on key press
					switch (e.key.keysym.sym)
					{
						case SDLK_r:
							gk.cudaClear();

							free(world);

							int generator = rand() % 4;
							world = new World(generator, SCREEN_WIDTH, SCREEN_HEIGHT);
							
							gk = GravityKernel(world);

							gk.cudaPrep();

							break;
						}
					break;

				case SDL_MOUSEBUTTONDOWN:
					
					if(e.button.button == SDL_BUTTON_LEFT)
						isDragging = true;
					break;
					
				case SDL_MOUSEMOTION:
					if (mPosXTwo == -1 || mPosYTwo == -1) {
						SDL_GetMouseState(&mPosXTwo, &mPosYTwo);
					}
					else {
						mPosXOne = mPosXTwo;
						mPosYOne = mPosYTwo;
						SDL_GetMouseState(&mPosXTwo, &mPosYTwo);
					}

					if (isDragging) {
						offsetX += (mPosXTwo - mPosXOne);
						offsetY += (mPosYTwo - mPosYOne);
					}

					break;

				case SDL_MOUSEBUTTONUP:
					if (e.button.button == SDL_BUTTON_LEFT)
						isDragging = false;
					break;

			}
		}

		gk.runKernel();
		world->render(window_renderer, offsetX, offsetY, zoom);
	}

	gk.cudaClear();

	return 0;

}