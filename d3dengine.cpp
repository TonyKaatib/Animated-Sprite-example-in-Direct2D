#include "d3dengine.h"
#include <dwrite.h>
#include <sstream>
#include "timer.h"
#include "keyboard.h"
#include "sprite.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "Windowscodecs.lib")

HWND ghMainWnd;
//
D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL featureLevels = D3D_FEATURE_LEVEL_11_0;
ID3D11Device* pD3Device = nullptr;
ID3D11DeviceContext* pD3DContext = nullptr;
IDXGISwapChain1* pSwap = nullptr;
IDXGIAdapter* pAdapter = nullptr;
IDXGIFactory2* pDxgiFactory = nullptr;
ID3D11Texture2D* pBackBuffer = nullptr;
IDXGISurface* pSurface = nullptr;
ID2D1Bitmap1* pBitmap = nullptr;
//
IDWriteFactory* pWriteFactory = nullptr;
IDWriteTextFormat* pTextFormat = nullptr;
//
ID2D1Factory4* pFactory = nullptr;
IDXGIDevice1* pDxgi = nullptr;
ID2D1Device3* pDevice = nullptr;
ID2D1DeviceContext3* pContext = nullptr;
//ID2D1HwndRenderTarget* pRT = nullptr;
RECT rc;
//
ID2D1SolidColorBrush* pBlack = nullptr;
IWICImagingFactory* pWICFactory = nullptr;
//
ID2D1Bitmap* pImage = nullptr;
ID2D1Bitmap* pImage2 = nullptr;
ID2D1Bitmap* pWalk = nullptr;
ID2D1SpriteBatch* sBatch = nullptr;
ID2D1SpriteBatch* sWalk = nullptr;
//
UINT index;
UINT index2;

UINT x;
UINT y;

UINT xspeed;
UINT yspeed;

UINT animstate;
UINT lastpress;
bool isWalking;

//
/*D2D1_RECT_F pRectF = D2D1::RectF(64.f, 0, 128.f, 64.f);
D2D1_RECT_U pRectU = D2D1::RectU(0, 0, 64, 64);*/
//

HRESULT InitWindow(HINSTANCE hInstance, int cmdShow) {
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(0, IDI_HAND);
	wc.hCursor = LoadIcon(0, IDI_HAND);
	wc.hbrBackground = 0;
	wc.lpszMenuName = 0;
	wc.lpszClassName = WINDOW_CLASS_NAME;
	wc.hIconSm = LoadIcon(0, IDI_HAND);

	if (!RegisterClassEx(&wc)) {
		MessageBox(0, L"RegisterClass Failed! Time to debug >:)", 0, 0);
		return 1;
	}

	ghMainWnd = CreateWindowEx(
		0,
		WINDOW_CLASS_NAME,
		L"Action Pong",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN),
		0,
		0,
		hInstance,
		0);

	if (ghMainWnd == 0) {
		MessageBox(0, L"CreateWindow FAILED! Time to debug >:)", 0, 0);
		return 1;
	}

	ShowWindow(ghMainWnd, SW_SHOWMAXIMIZED);
	UpdateWindow(ghMainWnd);
	return S_OK;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hdc;

	switch (msg) {
	case WM_CREATE:
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CLOSE:
		int result = MessageBox(hWnd, L"Are you sure you want to quit?", L"Quitting", MB_YESNO | MB_ICONQUESTION);
		if (result == IDYES) {
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

HRESULT InitDevice() {
	HRESULT hr = S_OK;
	GetClientRect(ghMainWnd, &rc);

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);
	if (FAILED(hr)) {
		MessageBox(0, L"Failure to create D2D1 Factory!", 0, 0);
		return hr;
	}

	/*hr = pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(ghMainWnd, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)), &pRT);
	if (FAILED(hr)) {
		MessageBox(0, L"Failure to create HWND Render Target!", 0, 0);
		return hr;
	}*/

	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	D3D_DRIVER_TYPE driverTypes[] = {
		D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_SOFTWARE
	};

	UINT totalDriverTypes = ARRAYSIZE(driverTypes);

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		0,
		0,
		D3D11_SDK_VERSION,
		&pD3Device,
		nullptr,
		&pD3DContext
	);

	if (FAILED(hr)) {
		MessageBox(0, L"Failure to create D3D11 Device!", 0, 0);
		return hr;
	}

	hr = pD3Device->QueryInterface(__uuidof(IDXGIDevice1), (void**) &pDxgi);
	if (FAILED(hr)) {
		MessageBox(0, L"Failure to Query Interface DXGI Device!", 0, 0);
		return hr;
	}

	hr = pFactory->CreateDevice(pDxgi, &pDevice);
	if (FAILED(hr)) {
		MessageBox(0, L"Failure to create D2D1 Device!", 0, 0);
		return hr;
	}

	hr = pDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &pContext);
	if (FAILED(hr)) {
		MessageBox(0, L"Failure to create D2D1 Device Context!", 0, 0);
		return hr;
	}

	DXGI_SWAP_CHAIN_DESC1 sd = { };
	ZeroMemory(&sd, sizeof(sd));
	sd.Width = width;
	sd.Height = height;
	sd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.Stereo = false;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Scaling = DXGI_SCALING_NONE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	sd.Flags = 0;

	hr = pDxgi->GetAdapter(&pAdapter);
	if (FAILED(hr)) {
		MessageBox(0, L"Failure to create DXGI Adapter!", 0, 0);
		return hr;
	}

	hr = pAdapter->GetParent(IID_PPV_ARGS(&pDxgiFactory));
	if (FAILED(hr)) {
		MessageBox(0, L"Failure to get parent!", 0, 0);
		return hr;
	}

	hr = pDxgiFactory->CreateSwapChainForHwnd(pD3Device, ghMainWnd, &sd, NULL, NULL, &pSwap);
	if (FAILED(hr)) {
		MessageBox(0, L"Failure to create Swap Chain!", 0, 0);
		return hr;
	}

	hr = pDxgi->SetMaximumFrameLatency(1);

	hr = pSwap->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	if (FAILED(hr)) {
		MessageBox(0, L"Failure to get Back Buffer!", 0, 0);
		return hr;
	}

	D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET |
		D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE), 96.0f, 96.0f, NULL);

	hr = pSwap->GetBuffer(0, IID_PPV_ARGS(&pSurface));
	if (FAILED(hr)) {
		MessageBox(0, L"Failure to get DXGI Back Buffer!", 0, 0);
		return hr;
	}

	hr = pContext->CreateBitmapFromDxgiSurface(pSurface, NULL, &pBitmap);
	if (FAILED(hr)) {
		MessageBox(0, L"Failure to create Bitmap!", 0, 0);
		return hr;
	}

	hr = pContext->CreateSpriteBatch(&sBatch);
	if (FAILED(hr)) {
		MessageBox(0, L"Failure to create sprite batch!", 0, 0);
		return hr;
	}

	hr = pContext->CreateSpriteBatch(&sWalk);
	if (FAILED(hr)) {
		MessageBox(0, L"Failure to create sprite batch!", 0, 0);
		return hr;
	}

	pContext->SetTarget(pBitmap);

	hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, reinterpret_cast<void**> (&pWICFactory));

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(pWriteFactory), reinterpret_cast<IUnknown**>(&pWriteFactory));

	hr = LoadBitmapFromFile(pContext, pWICFactory, L"shipanimated.png", 0, 0, &pImage);
	if (FAILED(hr)) {
		MessageBox(0, L"Failure to load bitmap!", 0, 0);
		return hr;
	}

	hr = LoadBitmapFromFile(pContext, pWICFactory, L"macom.png", 0, 0, &pImage2);
	if (FAILED(hr)) {
		MessageBox(0, L"Failure to load bitmap!", 0, 0);
		return hr;
	}

	hr = LoadBitmapFromFile(pContext, pWICFactory, L"Walking.png", 0, 0, &pWalk);
	if (FAILED(hr)) {
		MessageBox(0, L"Failure to load bitmap!", 0, 0);
		return hr;
	}

	D2D1_SIZE_F size = pImage->GetSize();
	D2D1_SIZE_F newsize;

	newsize.height = 0;
	newsize.width = 0;

	D2D1_SIZE_F walksize = pWalk->GetSize();

	//D2D1_RECT_F pRectF = D2D1::RectF(0, 0, 128, 128.f);
	//D2D1_RECT_U pRectU = D2D1::RectU(0, 0, 64, 64);
	//D2D1_RECT_U pRectU = D2D1::RectU(size.height * index, 0, (size.height * index) + size.height, size.height);
	//D2D1_MATRIX_3X2_F pMatrix = D2D1::Matrix3x2F::Translation(newsize);

	//hr = sBatch->AddSprites(2, &pRectF, &pRectU, NULL, NULL, 0, 0, 0, 0);

	for (int i = 1; i < 4; i++) {
		D2D1_RECT_F pRectF = D2D1::RectF(0, 0, 128, 128.f);
		//D2D1_RECT_U pRectU = D2D1::RectU(0, 0, 64, 64);
		D2D1_RECT_U pRectU = D2D1::RectU(size.height * i, 0, (size.height * i) + size.height, size.height);
		hr = sBatch->AddSprites(4, &pRectF, &pRectU, NULL, NULL, 0, 0, 0, 0);
	}

	for (int i = 1; i < 8; i++) {
		D2D1_RECT_F pRectF = D2D1::RectF(0, 0, 54, 92);
		//D2D1_RECT_U pRectU = D2D1::RectU(0, 0, 64, 64);
		D2D1_RECT_U pRectU = D2D1::RectU(0, 0, 56, 92);
		hr = sWalk->AddSprites(8, &pRectF, &pRectU, NULL, NULL, 0, 0, 0, 0);
	}
	const WCHAR* text;
	UINT32 text_length;

	text = L"Coordinates: %1f";
	text_length = (UINT32)wcslen(text);

	//	pRT->DrawText(text, text_length, pTextFormat, layoutRect, pRectColour);

	hr = pWriteFactory->CreateTextFormat(
		L"Consolas",
		nullptr,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		72.0F,
		L"pt-pt",
		&pTextFormat
	);

	if (SUCCEEDED(hr)) {
		hr = pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	}

	if (SUCCEEDED(hr)) {
		hr = pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}

	hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, reinterpret_cast<void**> (&pWICFactory));

	return S_OK;
}

HRESULT Render() {
	HRESULT hr = S_OK;

	DXGI_PRESENT_PARAMETERS parDxgi = {};
	parDxgi.DirtyRectsCount = 0;
	parDxgi.pDirtyRects = NULL;
	parDxgi.pScrollOffset = NULL;
	parDxgi.pScrollRect = NULL;

	pContext->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	ID2D1SolidColorBrush* pColour = nullptr;

	if (SUCCEEDED(hr)) {
		pContext->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Red),
			&pColour
		);
	}
	pContext->BeginDraw();
	pContext->Clear(D2D1::ColorF(0.0f, 0.0f, 1.0f));

	D2D1_RECT_F layoutRect = D2D1::RectF(
		static_cast<FLOAT>(rc.left),
		static_cast<FLOAT>(rc.top - 500),
		static_cast<FLOAT>(rc.right - rc.left),
		static_cast<FLOAT>(rc.bottom - rc.top)
	);

	static const int bufferLength = 256;
	static wchar_t wsbuffer[bufferLength];

	index++;
	if (index == 4)
		index = 0;

	index2++;
	if (index2 == 8)
		index2 = 0;

	x += xspeed;
	y += yspeed;

	if (KEYDOWN(VK_RIGHT)) {
		xspeed = 10;
		animstate = RIGHT;
		lastpress = 4;
	}
	if (KEYUP(VK_RIGHT)) {
		xspeed = 0;
	}

	if (KEYDOWN(VK_LEFT)) {
		xspeed -= 10;
		animstate = LEFT;
		lastpress = 3;
	}
	if (KEYUP(VK_LEFT)) {
		xspeed -= 0;
	}

	if (KEYDOWN(VK_DOWN)) {
		yspeed = 10;
		animstate = DOWN;
		lastpress = 0;
	}
	if (KEYUP(VK_DOWN)) {
		yspeed = 0;
	}

	if (KEYDOWN(VK_UP)) {
		yspeed -= 10;
		animstate = UP;
		lastpress = 7;
	}
	if (KEYUP(VK_UP)) {
		yspeed -= 0;
	}

	if (KEYUP(VK_UP) && KEYUP(VK_DOWN) && KEYUP(VK_RIGHT) && KEYUP(VK_LEFT)) {
		animstate = IDLE;
	}
	int length = swprintf_s(wsbuffer, bufferLength, L"Love me! %1u", animstate);

	pContext->DrawText(wsbuffer, length, pTextFormat, layoutRect, pColour);

	D2D1_SIZE_F spritesize;
	D2D1_SIZE_F walksize;

	spritesize.height = 64;
	spritesize.width = 256;

	walksize.height = 92;
	walksize.width = 440;

	D2D1_RECT_F pRectF = D2D1::RectF(0, 0, 256, 256);
	//D2D1_RECT_U pRectU = D2D1::RectU(0, 0, 64, 64);
	D2D1_RECT_U pRectU = D2D1::RectU(spritesize.height * index, 0, (spritesize.height * index) + spritesize.height, spritesize.height);
	D2D1_RECT_F pRectF2 = D2D1::RectF(x + 500, y + 500, 56 + x + 500, 95 + y + 500);
	//D2D1_RECT_U pRectU2 = D2D1::RectU(walksize.height * index, 0, (walksize.height * index) + walksize.height, walksize.height);

	D2D1_RECT_U pIdleLeft = D2D1::RectU(55 * lastpress, 380, (55 * lastpress) + 55, 475);

	D2D1_RECT_U pLeft = D2D1::RectU(55 * index2, 0, (55 * index2) + 55, 95);
	D2D1_RECT_U pRight = D2D1::RectU(55 * index2, 95, (55 * index2) + 55, 190);
	D2D1_RECT_U pUp = D2D1::RectU(55 * index2, 190, (55 * index2) + 55, 285);
	D2D1_RECT_U pDown = D2D1::RectU(55 * index2, 285, (55 * index2) + 55, 380);
	//hr = sBatch->SetSprites(index, 4, &pRectF, &pRectU, NULL, NULL, 0, 0, 0, 0);

	switch (animstate) {
	case (0):
		hr = sWalk->SetSprites(index2, 8, &pRectF2, &pIdleLeft, NULL, NULL, 0, 0, 0, 0);
		break;
	case (1):
		hr = sWalk->SetSprites(index2, 8, &pRectF2, &pLeft, NULL, NULL, 0, 0, 0, 0);
		break;
	case (2):
		hr = sWalk->SetSprites(index2, 8, &pRectF2, &pRight, NULL, NULL, 0, 0, 0, 0);
		break;
	case (3):
		hr = sWalk->SetSprites(index2, 8, &pRectF2, &pUp, NULL, NULL, 0, 0, 0, 0);
		break;
	case (4):
		hr = sWalk->SetSprites(index2, 8, &pRectF2, &pDown, NULL, NULL, 0, 0, 0, 0);
		break;
	default:break;
	}

	//hr = sWalk->SetSprites(index2, 8, &pRectF2, &pRectU2, NULL, NULL, 0, 0, 0, 0);

	//RenderImage(pContext, pImage2, 12, 12, index);
	pContext->DrawSpriteBatch(sBatch, index, 4, pImage, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1_SPRITE_OPTIONS_NONE);
	pContext->DrawSpriteBatch(sWalk, index2, 8, pWalk, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1_SPRITE_OPTIONS_NONE);

	hr = pContext->EndDraw();
	hr = pSwap->Present1(1, 0, &parDxgi);
	return hr;
}

/*HRESULT Render() {
	HRESULT hr = S_OK;
	ID2D1SolidColorBrush* pColor;

	DXGI_PRESENT_PARAMETERS parDxgi = {};
	parDxgi.DirtyRectsCount = 0;
	parDxgi.pDirtyRects = NULL;
	parDxgi.pScrollOffset = NULL;
	parDxgi.pScrollRect = NULL;

	UINT number = 100;
	IncreaseIndex();
	//D2D1_RECT_U pRectU = D2D1::RectU(size.height * index, 0, (size.height * index) + size.height, size.height);

	D2D1_SIZE_F sizea = pImage->GetSize();
	D2D1_RECT_F pRectFa = D2D1::RectF(0, 0, 128, 128.f);
	//D2D1_RECT_U pRectU = D2D1::RectU(0, 0, 64, 64);
	D2D1_RECT_U pRectUa = D2D1::RectU(sizea.height * index, 0, (sizea.height * index) + sizea.height, sizea.height);

	//D2D1_RECT_U pRectU = D2D1::RectU(64.f, 0, 128, 64.f);
	hr = pContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &pColor);

	pContext->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	pContext->BeginDraw();
	sBatch->SetSprites(index, 4, &pRectFa, &pRectUa, NULL, NULL, 0, 0, 0, 0);
	//sBatch->Clear();
	if (FAILED(hr)) {
		MessageBox(0, L"Failure adding sprites!", 0, 0);
		return hr;
	}
	RenderImage(pContext, pImage2, 12, rotate, 13);
	pContext->DrawSpriteBatch(sBatch, pImage, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1_SPRITE_OPTIONS_NONE);
	hr = pContext->EndDraw();
	hr = pSwap->Present1(1, 0, &parDxgi);
	//hr = pSwap->Present(1, 0);
	return hr;
};*/

void CleanupDevice() {
	ReleaseCOM(pWriteFactory);
	ReleaseCOM(pWICFactory);
	ReleaseCOM(pDevice);
	ReleaseCOM(pContext);
	ReleaseCOM(pFactory);
	ReleaseCOM(pSwap);
	ReleaseCOM(sBatch);
	ReleaseCOM(sWalk);
	ReleaseCOM(pD3DContext);
	ReleaseCOM(pD3Device);
}