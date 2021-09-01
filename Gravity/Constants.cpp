
#ifndef CONSTANT_CPP
#define CONSTANT_CPP

enum Constant {
	NUM_PARTICLES = 600,
	SPAWN = 500,
	DIMENSIONS = 2,
	MAX_MASS = 25
};

enum Generator {
	DEFAULT = 0,
	CIRCULAR = 1,
	RECTANGULAR = 2,
	CLUSTERED = 3,

	SPAWN_RANGE = 500
};


#endif