#include "timer.h"
#include <sstream>
#include <chrono>
#include <thread>
#include <iostream>

using namespace std::chrono;

auto starttime = steady_clock::now();

duration<float> deltatime;

//
int fpsLimit() {
	return 10;
}

int stop() {
	return 1;
}

using dTime = duration<double>;
auto frameLimit = duration_cast<system_clock::duration>(dTime{ 1. / fpsLimit() });
auto startFrame = system_clock::now();
auto endFrame = startFrame + frameLimit;
auto prevTime = time_point_cast<seconds>(startFrame);
unsigned fps = 0;
//

void ResetTime() {
	startFrame = system_clock::now();
};

void StopTime() {
	auto frameLimit = duration_cast<system_clock::duration>(dTime{ 1. / stop() });
};

void Tick() {
	std::this_thread::sleep_until(endFrame);
	startFrame = endFrame;
	endFrame = startFrame + frameLimit;
}

unsigned timeCount() {
	auto time = time_point_cast<seconds>(system_clock::now());
	fps++;
	if (time > prevTime) {
		fps = 0;
		prevTime = time;
	}
	return(fps);
}