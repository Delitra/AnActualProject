#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

IDXGISwapChain * swapchain; // swap chain pointer (swap chain is a series of buffers which get switched in and out)
ID3D11Device * dev; // device pointer (a device is a representation of the gpu and manages the vram)
ID3D11DeviceContext * devcon; // device context pointer (device context manages the gpu and the rendering pipeline(we use this to render))
ID3D11RenderTargetView * backbuffer;
float r = 1.0;
float g = 1.0;
float b = 1.0;
float a = 1.0;

void InitD3D(HWND HWnd); //prototype for the function that initialises direct3d
void CleanD3D(void); //prototype for function that closes direct3d and releases its memory
//filling this with comments makes me feel really awkward when I have to commit them to github
//hi github

void InitD3D(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC scd; //struct for swap chain info
	//clearing the memory for the struct just like the wndclassex in winmain
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferCount = 1;  // one buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32 bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // how to use the swap chain (this one is for drawing to the back buffer)
	scd.OutputWindow = hWnd; // window to use, this is from the argument
	scd.SampleDesc.Count = 4; // how many multisamples (antialiasing)
	scd.Windowed = TRUE; //windowed or fullscreen

	//create a device, device context and swap chain
	D3D11CreateDeviceAndSwapChain(NULL, //which graphics adapter to use? NULL lets DXGI decide
		D3D_DRIVER_TYPE_HARDWARE, //how to render
		NULL, //something about software rendering
		NULL, //some flags (multi/singlethread, debug, reference rendering, direct2d)
		NULL, //if you need the end user to have certain hardware capabilities, this is where you set that
		NULL, //how many did you have in your previous list you scrub
		D3D11_SDK_VERSION, //eh
		&scd, //swap chain struct
		&swapchain, //swap chain object
		&dev, //device object
		NULL, //feature level variable
		&devcon); //device context object

	//get backbuffer address
	ID3D11Texture2D * pBackBuffer;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	//create a render target using the back buffer address
	dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
	pBackBuffer->Release();
	//set the render target as the back buffer
	devcon->OMSetRenderTargets(1, &backbuffer, NULL);

	//set the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = 800;
	viewport.Height = 600;

	devcon->RSSetViewports(1, &viewport);
}

void RenderFrame(float r, float b, float g, float a)
{
	//clear the back buffer by filling it with blue
	devcon->ClearRenderTargetView(backbuffer, D3DXCOLOR(r, g, b, a));

	//do some rendering maybe eventually here

	//switch the front buffer and back buffer
	swapchain->Present(0, 0);
}

void CleanD3D()
{
	swapchain->Release();
	backbuffer->Release();
	dev->Release();
	devcon->Release();
}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam); //here we prototype the windowproc function so we can call it in WinMain before defining it

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	HWND hWnd; //create a handle to a window - we will fill this later on
	WNDCLASSEX wc; //wndclassex is a struct containing lots of variables concerning the window class
	ZeroMemory(&wc, sizeof(WNDCLASSEX)); //this is a function built into windows that will clear all memory starting from the address of wc and ending at the total size it will encompass

										 //now we will set all the variables within wc - we can now do this because we cleared memory for it
	wc.cbSize = sizeof(WNDCLASSEX); //cbSize is simply the size of the structure, so naturally we set this to the size of the strucutre.
	wc.style = CS_HREDRAW | CS_VREDRAW; //these are enums for window style in windows. the ones chosen cause the window to redraw itself if it is resized on either axis
	wc.lpfnWndProc = WindowProc; // lpfnWndProc wants a pointer to the function that creates windows. we already prototyped this above
	wc.hInstance = hInstance; //this just wants the instance handle for our program, which we get from the arguments of WinMain
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); //this sets the mouse cursor to a default
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW; //a handle to the background brush (background of our window) there are a large number of HBRUSH types available
	wc.lpszClassName = "WindowClass1"; //this should be a pointer to a null terminated string (typing a string "like this" does that for us.) this specifies the window class name.

	RegisterClassEx(&wc); //here we register our window class using our previously defined WNDCLASSEX struct

	RECT wr = { 0,0,500,400 };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	hWnd = CreateWindowEx(NULL, "WindowClass1", "Our First Window", WS_OVERLAPPEDWINDOW, 300, 300, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, hInstance, NULL);
	//extended window style, window class name, window name, window style, x position, y position, width, height, parent window, menu (popup), instance handle, used for multiple windows

	ShowWindow(hWnd, nShowCmd); //put our window on the screen

	InitD3D(hWnd);

		//now that we have a window we can go into the main loop

		//msg is a struct that holds windows events
		MSG msg = { 0 };

	//wait for a messasge, store it in msg
	while (TRUE)
	{
		r -= 0.1f;
		g -= 0.1f;
		b -= 0.1f;
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			//translate keystroke messages
			TranslateMessage(&msg);

			//send the message to WindowProc
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;
		}
		Sleep(1000);
		RenderFrame(r, g, b, a);
	}

	CleanD3D();

	return msg.wParam;
}

//this is the message handler
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//switch statement for finding what the message was
	switch (message)
	{
	case WM_DESTROY: //this is when the window is closed
	{
		//close the application
		PostQuitMessage(0);
		return 0;
	} break;
	}

	//handle any messages the switch statement aws not able to handle
	return DefWindowProc(hWnd, message, wParam, lParam);
}