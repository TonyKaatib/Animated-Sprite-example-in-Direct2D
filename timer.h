#pragma once

#ifndef TIMER_H
#define TIMER_H

int fpsLimit();
int stop();
void ResetTime();
void StopTime();
void Tick();
unsigned timeCount();

#endif // !TIMER_H