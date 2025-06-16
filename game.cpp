#include "game.h"
#include "timer.h"
#include "d3dengine.h"

int GameInit(void* parms, int num_parms) {
	ResetTime();
	InitDevice();
	return 0;
};

int GameLogic(void* parms, int num_parms) {
	Tick();
	Render();
	return 0;
};

int GameShutdown(void* parms, int num_parms) {
	CleanupDevice();
	return 0;
};

