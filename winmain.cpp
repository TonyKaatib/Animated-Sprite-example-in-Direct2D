#include "game.h"
#include "d3dengine.h"
#include "timer.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nShowCmd) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(pCmdLine);

	InitWindow(hInstance, nShowCmd);

	MSG msg;

	ResetTime();
	GameInit(NULL, 0);

	while (TRUE) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				return (int)msg.wParam;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} //end if

		//Game Logic will go here
		GameLogic(NULL, 0);

	}//end while
	GameShutdown(NULL, 0);
	return static_cast<int>(msg.wParam);
}