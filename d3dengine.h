#pragma once

#ifndef D3DENGINE_H
#define D3DENGINE_H

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1_3.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <wincodec.h>

#define WINDOW_CLASS_NAME L"BasicWndClass"
#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }

HRESULT InitDevice();
HRESULT InitWindow(HINSTANCE hInstance, int CmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HRESULT Render();
void CleanupDevice();

#endif // !D3DENGINE_H
