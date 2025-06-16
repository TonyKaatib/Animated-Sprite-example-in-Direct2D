#pragma once

#ifndef GAME_H
#define GAME_H

int GameInit(void* parms, int num_parms);
int GameLogic(void* parms, int num_parms);
int GameShutdown(void* parms, int num_parms);

#endif // !GAME_H
