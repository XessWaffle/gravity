#include<iostream>
#include "Particle.h"

Particle::Particle() {

	this->acceleration = new float[Constant::DIMENSIONS];
	this->velocity = new float[Constant::DIMENSIONS];
	this->position = new float[Constant::DIMENSIONS];

	for (int i = 0; i < Constant::DIMENSIONS; i++) {
		this->position[i] = (float)(rand() % Generator::SPAWN_RANGE - Generator::SPAWN_RANGE/2 + Constant::SPAWN);
	}

	for (int i = 0; i < Constant::DIMENSIONS; i++) {
		this->velocity[i] = (float)(rand() % 30) - 15;
	}

	this->mass = rand() % Constant::MAX_MASS + 1;
}

Particle::Particle(float* spawn) {

	this->acceleration = new float[Constant::DIMENSIONS];
	this->velocity = new float[Constant::DIMENSIONS];
	this->position = new float[Constant::DIMENSIONS];

	for (int i = 0; i < Constant::DIMENSIONS; i++) {
		this->position[i] = spawn[i];
	}

	for (int i = 0; i < Constant::DIMENSIONS; i++) {
		this->velocity[i] = (float)(rand() % 30) - 15;
	}

	this->mass = rand() % Constant::MAX_MASS;

}

Particle::Particle(float* spawn, float* vel) {
	
	this->acceleration = new float[Constant::DIMENSIONS];
	this->velocity = new float[Constant::DIMENSIONS];
	this->position = new float[Constant::DIMENSIONS];

	for (int i = 0; i < Constant::DIMENSIONS; i++) {
		this->position[i] = spawn[i];
	}

	for (int i = 0; i < Constant::DIMENSIONS; i++) {
		this->velocity[i] = vel[i];
	}

	this->mass = rand() % Constant::MAX_MASS;

}

float* Particle::getPosition() {
	return position;
}

float* Particle::getVelocity() {
	return velocity;
}


void Particle::setPosition(float* position) {
	this->position = position;
}

void Particle::setVelocity(float* velocity) {
	this->velocity = velocity;
}

int Particle::getMass() {
	return this->mass;
}






