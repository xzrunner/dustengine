#include <windows.h>

#include "game.h"

#include <dust_game.h>
#include <fault.h>
#include <glp_loop.h>
#include <c_wrap_ur.h>
#include <c_wrap_gum.h>

#include <string.h>
#include <stdlib.h>

#define WIDTH 1024
#define HEIGHT 768

#define TOUCH_BEGIN 0
#define TOUCH_END 1
//#define TOUCH_RIGHT_BEGIN 2
//#define TOUCH_RIGHT_END 3
#define TOUCH_MOVE 2

#define KEY_DOWN 0
#define KEY_UP 1

#define CLASSNAME L"DUST"
#define WINDOWNAME L"DUST"
#define WINDOWSTYLE (WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX)

static int INTOUCH = 0;

static void
set_pixel_format_to_hdc(HDC hDC)
{
	int color_deep;
	PIXELFORMATDESCRIPTOR pfd;

	color_deep = GetDeviceCaps(hDC, BITSPIXEL);
	
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType	= PFD_TYPE_RGBA;
	pfd.cColorBits	= color_deep;
	pfd.cDepthBits	= 0;
	pfd.cStencilBits = 0;
	pfd.iLayerType	= PFD_MAIN_PLANE;

	int pixelFormat = ChoosePixelFormat(hDC, &pfd);

	SetPixelFormat(hDC, pixelFormat, &pfd);
}

static void
init_window(HWND hWnd) {
	HDC hDC = GetDC(hWnd);

	set_pixel_format_to_hdc(hDC);
	HGLRC glrc = wglCreateContext(hDC);

	if (glrc == 0) {
		exit(1);
	}

	wglMakeCurrent(hDC, glrc);

	void* rc = gum_get_render_context();
	if (!ur_init(rc)) {
		fault("Init gl error.");
	}

	ur_clear(rc, 0x88888888);
	ur_set_viewport(rc, 0, 0, WIDTH, HEIGHT);

	ReleaseDC(hWnd, hDC);
}

static void
get_xy(LPARAM lParam, int *x, int *y) {
	*x = (short)(lParam & 0xffff); 
	*y = (short)((lParam>>16) & 0xffff); 
}

 void
 dust_win_touch(int x, int y,int touch) {
 	switch (touch) {
 	case TOUCH_BEGIN: //case TOUCH_RIGHT_BEGIN:
 		INTOUCH = 1;
 		break;
 	case TOUCH_END: //case TOUCH_RIGHT_END:
 		INTOUCH = 0;
 		break;
 	case TOUCH_MOVE:
 		if (!INTOUCH) {
 			return;
		}
 		break;
 	}
 	// windows only support one touch id (0)
 	int id = 0;
 	dust_game_touch(game_instance(), id, x,y,touch);
}

LRESULT CALLBACK 
WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_CREATE:
		init_window(hWnd);
		SetTimer(hWnd,0,5,NULL);
		break;
	case WM_PAINT: {
		if (GetUpdateRect(hWnd, NULL, FALSE)) {
			HDC hDC = GetDC(hWnd);
//			update_frame(hDC);

			game_draw();
			SwapBuffers(hDC);

			ValidateRect(hWnd, NULL);
			ReleaseDC(hWnd, hDC);
		}
		return 0;
	}
	case WM_TIMER:
		game_update();
		glp_loop_update();
		InvalidateRect(hWnd, NULL , FALSE);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_LBUTTONUP: {
		int x,y;
		get_xy(lParam, &x, &y); 
		dust_win_touch(x,y,TOUCH_END);
		break;
	}
	case WM_LBUTTONDOWN: {
		int x,y;
		get_xy(lParam, &x, &y); 
		dust_win_touch(x,y,TOUCH_BEGIN);
		break;
	}
	//case WM_RBUTTONUP: {
	//	int x,y;
	//	get_xy(lParam, &x, &y); 
	//	dust_win_touch(x,y,TOUCH_RIGHT_END);
	//	break;
	//}
	//case WM_RBUTTONDOWN: {
	//	int x,y;
	//	get_xy(lParam, &x, &y); 
	//	dust_win_touch(x,y,TOUCH_RIGHT_BEGIN);
	//	break;
	//}
	case WM_MOUSEMOVE: {
		int x,y;
		get_xy(lParam, &x, &y); 
		dust_win_touch(x,y,TOUCH_MOVE);
		break;
	}
	case WM_CHAR: {
		game_key_messge("KEY_PRESS", (char)wParam);
		break;
	}
	case WM_KEYDOWN: {
		game_key_messge("KEY_DOWN", (char)wParam);
		break;
	}
	case WM_KEYUP: {
		game_key_messge("KEY_UP", (char)wParam);
		break;
	}
	case WM_MOUSEWHEEL: {
		int scroll;
		POINT p;
		get_xy(lParam, &p.x, &p.y); 
		scroll = GET_WHEEL_DELTA_WPARAM(wParam);
		ScreenToClient(hWnd, &p);
		game_scroll(p.x, p.y, scroll);
		break;
	}
	}
	return DefWindowProcW(hWnd, message, wParam, lParam);
}

static void
register_class()
{
	WNDCLASSW wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = GetModuleHandleW(0);
	wndclass.hIcon = 0;
	wndclass.hCursor = 0;
	wndclass.hbrBackground = 0;
	wndclass.lpszMenuName = 0; 
	wndclass.lpszClassName = CLASSNAME;

	RegisterClassW(&wndclass);
}

static HWND
create_window(int w, int h) {
	RECT rect;

	rect.left=0;
	rect.right=w;
	rect.top=0;
	rect.bottom=h;

	AdjustWindowRect(&rect,WINDOWSTYLE,0);

	HWND wnd=CreateWindowExW(0,CLASSNAME,WINDOWNAME,
		WINDOWSTYLE, CW_USEDEFAULT,0,
		rect.right-rect.left,rect.bottom-rect.top,
		0,0,
		GetModuleHandleW(0),
		0);

	return wnd;
}

int
main(int argc, char *argv[]) {
	register_class();
	HWND wnd = create_window(WIDTH,HEIGHT);
	HDC hDC = GetDC(wnd);

	game_init(argc, argv);
	glp_loop_init(30);

	ShowWindow(wnd, SW_SHOWDEFAULT);
	UpdateWindow(wnd);

	MSG msg;
	//while (GetMessage(&msg, NULL, 0, 0)) {
	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);
	//}

	BOOL done = FALSE;
	while(!done) {
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if(msg.message == WM_QUIT) {
				done = TRUE;
			}
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
// 			game_update(1.0f / 30);
// 			SwapBuffers(hDC);
		}
	}	

	return 0;
}
