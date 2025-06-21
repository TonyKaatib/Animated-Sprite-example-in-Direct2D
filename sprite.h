#pragma once

#ifndef SPRITE_H
#define SPRITE_H

#include "d3dengine.h"

typedef enum walkstate {
	IDLE,
	LEFT,
	RIGHT,
	UP,
	DOWN,
};

HRESULT LoadBitmapFromFile(ID2D1DeviceContext3* pD2D1Context, IWICImagingFactory* pIWICFactory, PCWSTR uri, UINT dWidth, UINT dHeight, ID2D1Bitmap** ppBitmap);
void RenderImage(ID2D1DeviceContext3* pDeviceContext, ID2D1Bitmap* pBmap, float playerX, float playerY, float playerZ);
void RenderSprite(ID2D1DeviceContext3* pDeviceContext, ID2D1SpriteBatch* spriteBatch, ID2D1Bitmap* pBitmap);

#endif // !SPRITE_H
