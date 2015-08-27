#include <windows.h>
#include <windowsx.h>

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

		//now that we have a window we can go into the main loop

		//msg is a struct that holds windows events
		MSG msg = { 0 };

	//wait for a messasge, store it in msg
	while (TRUE)
	{
		// check for messages
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			//translate keystroke messages
			TranslateMessage(&msg);

			//send the message to WindowProc
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;
		}
		else
		{

		}
	}

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