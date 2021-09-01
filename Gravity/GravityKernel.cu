#include "GravityKernel.cuh"

#include<ctime>
#include<iostream>
#include<stdio.h>
#include<cuda.h>

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#define THREADS_PER_BLOCK 64

__global__ void acceleration_kernel(Particle* particle, float* acc_sum, Particle* particles, int count) {

	int positionID = blockDim.x * blockIdx.x + threadIdx.x;
	
	float gConstant = 5;

	if (positionID < count) {
		Particle ref = particles[positionID];

		float distX = ref.position[0] - particle->position[0];
		float distY = ref.position[1] - particle->position[1];

		float dist2 = distX * distX + distY * distY;
		
		float force = 0;

		if (dist2 > 100) {
			force = gConstant * ref.mass * particle->mass / (float)dist2;

			float rad = atan2(distY, distX);
		
			acc_sum[positionID] = force * cos(rad);
			acc_sum[positionID + Constant::NUM_PARTICLES] = force * sin(rad);
		}
		else {
			acc_sum[positionID] = 0;
			acc_sum[positionID + Constant::NUM_PARTICLES] = 0;
		}

	}

}

__global__ void sum_kernel(float* acc_sum) {

	const int tid = threadIdx.x;

	auto step_size = 1;
	int number_of_threads = blockDim.x;

	if (tid < Constant::NUM_PARTICLES) {

		while (number_of_threads > 0)
		{
			if (tid < number_of_threads) // still alive?
			{
				const auto fst = tid * step_size * 2;
				const auto snd = fst + step_size;
				acc_sum[fst] += acc_sum[snd];
			}

			step_size <<= 1;
			number_of_threads >>= 1;
		}
	}

}

__global__ void gravity_kernel(Particle* particles, float* acc_sum, int count) {

	float dt = .05;
	float drag = 0.01;

	int id = blockDim.x * blockIdx.x + threadIdx.x;

	if (id < count) {

		const int blocks = Constant::NUM_PARTICLES / THREADS_PER_BLOCK + 1;
		const int threads = Constant::NUM_PARTICLES / 2;

		acceleration_kernel << <blocks, THREADS_PER_BLOCK >> > (&particles[id], (acc_sum + 2 * id * Constant::NUM_PARTICLES), particles, count);

		sum_kernel << <1, threads >> > ((acc_sum + 2 * id * Constant::NUM_PARTICLES));
		sum_kernel << <1, threads >> > ((acc_sum + (2 * id + 1) * Constant::NUM_PARTICLES));

		float x = 0, y = 0;

		/*for (int i = 0; i < Constant::NUM_PARTICLES; i++) {
			x += acc_sum[2 * id * Constant::NUM_PARTICLES];
			y += acc_sum[2 * id * Constant::NUM_PARTICLES + Constant::NUM_PARTICLES];
		}*/

		x = acc_sum[2 * id * Constant::NUM_PARTICLES];
		y = acc_sum[(2 * id + 1) * Constant::NUM_PARTICLES];

		Particle ref = particles[id];

		float* vel = ref.velocity;
		float* pos = ref.position;

		vel[0] += x * dt;
		vel[1] += y * dt;

		vel[0] -= vel[0] * dt * drag;
		vel[1] -= vel[1] * dt * drag;

		pos[0] += vel[0] * dt;
		pos[1] += vel[1] * dt;

	}
}


GravityKernel::GravityKernel() {
	this->_world = new World();
}

GravityKernel::GravityKernel(World* world) {
	this->_world = world;
}

__host__ void GravityKernel::cudaPrep() {
	Particle* particles = this->_world->particles;

	Particle* d_particles;
	float* d_acc_sum;

	float** d_positions = new float* [Constant::NUM_PARTICLES];
	float** d_velocities = new float* [Constant::NUM_PARTICLES];
	
	if (cudaMalloc(&d_particles, sizeof(Particle) * Constant::NUM_PARTICLES) != cudaSuccess) {
		std::cout << "Particle Device Allocation Error" << std::endl;
		return;
	}

	if (cudaMalloc(&d_acc_sum, sizeof(float) * Constant::NUM_PARTICLES * Constant::NUM_PARTICLES * Constant::DIMENSIONS) != cudaSuccess) {
		std::cout << "Particle Accelerations Allocation Error" << std::endl;
		return;
	}


	for (int i = 0; i < Constant::NUM_PARTICLES; i++) {

		if (cudaMalloc(&(d_positions[i]), sizeof(float) * Constant::DIMENSIONS) != cudaSuccess) {
			std::cout << "Position Mapping Failure" << std::endl;
		}

		if (cudaMalloc(&(d_velocities[i]), sizeof(float) * Constant::DIMENSIONS) != cudaSuccess) {
			std::cout << "Velocity Mapping Failure" << std::endl;
		}


		if (cudaMemcpy(&(d_particles[i].position), &(d_positions[i]), sizeof(float*), cudaMemcpyHostToDevice) != cudaSuccess){
			std::cout << "Particle Position Allocation Error" << std::endl;
		}

		if (cudaMemcpy(&(d_particles[i].velocity), &(d_velocities[i]), sizeof(float*), cudaMemcpyHostToDevice) != cudaSuccess) {
			std::cout << "Particle Velocity Allocation Error" << std::endl;
		}

	}

	for (int i = 0; i < Constant::NUM_PARTICLES; i++) {
		if (cudaMemcpy(d_positions[i], particles[i].position, sizeof(float) * Constant::DIMENSIONS, cudaMemcpyHostToDevice) != cudaSuccess) {
			std::cout << "Particle Position Allocation Error" << std::endl;
		}

		if (cudaMemcpy(d_velocities[i], particles[i].velocity, sizeof(float) * Constant::DIMENSIONS, cudaMemcpyHostToDevice) != cudaSuccess) {
			std::cout << "Particle Velocity Allocation Error" << std::endl;
		}

		if (cudaMemcpy(&(d_particles[i].mass), &(particles[i].mass), sizeof(int), cudaMemcpyHostToDevice) != cudaSuccess) {
			std::cout << "Particle Mass Allocation Error" << std::endl;
		}
	}
	
	this->d_particles = d_particles;
	this->d_acc_sum = d_acc_sum;
	this->d_positions = d_positions;
	this->d_velocities = d_velocities;
}

__host__ void GravityKernel::runKernel() {
	
	int blocks = Constant::NUM_PARTICLES / THREADS_PER_BLOCK + 1;

	gravity_kernel<<<blocks, THREADS_PER_BLOCK>>>(d_particles, d_acc_sum, Constant::NUM_PARTICLES);
	cudaDeviceSynchronize();

	Particle* ref = this->_world->particles;

	for (int i = 0; i < Constant::NUM_PARTICLES; i++) {
		cudaMemcpy(ref[i].position, d_positions[i], sizeof(float) * Constant::DIMENSIONS, cudaMemcpyDeviceToHost);
		cudaMemcpy(ref[i].velocity, d_velocities[i], sizeof(float) * Constant::DIMENSIONS, cudaMemcpyDeviceToHost);
	}

}


__host__ void GravityKernel::cudaClear() {

	std::cout << "Clearing memory" << std::endl;

	for (int i = 0; i < Constant::NUM_PARTICLES; i++) {
		cudaFree(&(d_particles[i].position));
		cudaFree(&(d_particles[i].velocity));
	}

	cudaFree(d_particles);
}

