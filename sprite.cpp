#include "sprite.h"

IWICBitmapDecoder* pDecoder = nullptr;
IWICBitmapFrameDecode* pSource = nullptr;
IWICStream* pStream = nullptr;
IWICFormatConverter* pConverter = nullptr;
IWICBitmapScaler* pScaler = nullptr;

HRESULT LoadBitmapFromFile(ID2D1DeviceContext3* pD2D1Context, IWICImagingFactory* pIWICFactory, PCWSTR uri, UINT dWidth, UINT dHeight, ID2D1Bitmap** ppBitmap) {
	HRESULT hr = pIWICFactory->CreateDecoderFromFilename(uri, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder);

	if (SUCCEEDED(hr)) {
		hr = pDecoder->GetFrame(0, &pSource);
	}

	if (SUCCEEDED(hr)) {
		hr = pIWICFactory->CreateFormatConverter(&pConverter);
	}

	if (SUCCEEDED(hr)) {
		hr = pConverter->Initialize(pSource, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.f, WICBitmapPaletteTypeMedianCut);
	};

	if (SUCCEEDED(hr)) {
		hr = pD2D1Context->CreateBitmapFromWicBitmap(pConverter, nullptr, ppBitmap);
	};

	if (pDecoder) pDecoder->Release();
	if (pSource) pSource->Release();
	if (pStream) pStream->Release();
	if (pConverter) pConverter->Release();
	if (pScaler) pScaler->Release();

	return hr;
};

void RenderSprite(ID2D1DeviceContext3* pDeviceContext, ID2D1SpriteBatch* spriteBatch, ID2D1Bitmap* pBitmap) {
	pDeviceContext->DrawSpriteBatch(spriteBatch, pBitmap);
}

void RenderImage(ID2D1DeviceContext3* pDeviceContext, ID2D1Bitmap* pBmap, float playerX, float playerY, float playerZ) {
	D2D1_SIZE_F size = pBmap->GetSize();
	float scale = size.width / 2;
	D2D1_POINT_2F imageCenter = D2D1::Point2F(playerX + scale, playerY + scale);

	pDeviceContext->SetTransform(D2D1::Matrix3x2F::Rotation(playerZ, imageCenter));
	pDeviceContext->DrawBitmap(pBmap, D2D1::RectF(playerX, playerY, 256 + playerX, 256 + playerY));
	pDeviceContext->SetTransform(D2D1::Matrix3x2F::Rotation(0, imageCenter));
};