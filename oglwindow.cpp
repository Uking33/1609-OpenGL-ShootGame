#define WIN32_MEAN_AND_LEAN
#define WIN32_EXTRA_LEAN

#include "oglwindow.h"

bool bLMB = false;		// left mouse button
bool bRMB = false;		// right mouse button
LPARAM Drag;			// mouse drag

// SetupPixelFormat()
// desc: setup the pixel format for OpenGL
void COGLWindow::SetupPixelFormat()
{
	int pixelFormat;

	PIXELFORMATDESCRIPTOR pfd =
	{	
		sizeof(PIXELFORMATDESCRIPTOR),	// size
		1,							// version
		PFD_SUPPORT_OPENGL |
		PFD_DRAW_TO_WINDOW |
		PFD_DOUBLEBUFFER,				// support float-buffering
		PFD_TYPE_RGBA,					// color type
		16,							// prefered color depth
		0, 0, 0, 0, 0, 0,				// color bits (ignored)
		0,							// no alpha buffer
		0,							// alpha bits (ignored)
		0,							// no accumulation buffer
		0, 0, 0, 0,					// accum bits (ignored)
		16,							// depth buffer
		0,							// no stencil buffer
		0,							// no auxiliary buffers
		PFD_MAIN_PLANE,				// main layer
		0,							// reserved
		0, 0, 0,						// no layer, visible, damage masks
	};

	pixelFormat = ChoosePixelFormat(hDC, &pfd);

	SetPixelFormat(hDC, pixelFormat, &pfd);
}

// SetupPalette()
// desc: setup a palette
void COGLWindow::SetupPalette()
{
	int pixelFormat = GetPixelFormat(hDC);
	PIXELFORMATDESCRIPTOR pfd;
	LOGPALETTE* pPal;
	int paletteSize;

	int redMask, greenMask, blueMask;
	int i;

	DescribePixelFormat(hDC, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	if (pfd.dwFlags & PFD_NEED_PALETTE)
		paletteSize = 1 << pfd.cColorBits;
	else
		return;

	pPal = (LOGPALETTE*)LocalAlloc(LPTR, sizeof(LOGPALETTE) + paletteSize * sizeof(PALETTEENTRY));
	pPal->palVersion = 0x300;
	pPal->palNumEntries = (short)paletteSize;

	//	build a simple RGB color palette
	redMask   = (1 << pfd.cRedBits)   - 1;
	greenMask = (1 << pfd.cGreenBits) - 1;
	blueMask  = (1 << pfd.cBlueBits)  - 1;

	for (i=0; i<paletteSize; ++i)
	{	pPal->palPalEntry[i].peRed = (BYTE)(
			(((i >> pfd.cRedShift) & redMask) * 255) / redMask);
		pPal->palPalEntry[i].peGreen = (BYTE)(
			(((i >> pfd.cGreenShift) & greenMask) * 255) / greenMask);
		pPal->palPalEntry[i].peBlue = (BYTE)(
			(((i >> pfd.cBlueShift) & blueMask) * 255) / blueMask);
		pPal->palPalEntry[i].peFlags = 0;
	}

	hPalette = CreatePalette(pPal);
	LocalFree(pPal);

	if (hPalette)
	{
		DeleteObject(SelectPalette(hDC, hPalette, FALSE));
		RealizePalette(hDC);
	}
}

// Create()
// desc: create the window
bool COGLWindow::Create()
{
	hDC = GetDC(hWnd);
	SetupPixelFormat();
	SetupPalette();
	hGLRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hGLRC);

	return !OnCreate();
}

// Destroy()
// desc: destroy the window
void COGLWindow::Destroy()
{
	if (hGLRC)
	{	
		wglMakeCurrent(hDC, NULL);
		wglDeleteContext(hGLRC);
	}
	if (hPalette)
	{	
		DeleteObject(hPalette);
	}

	SetWindowLong(hWnd, GWL_USERDATA, (LONG)NULL);

	DestroyWindow(hWnd);

	// if we're in fullscreen, get out of it
	EndFullScreen();
}

// PaletteChanged()
// desc: palette has changed
void COGLWindow::PaletteChanged(WPARAM wParam)
{
	if (hGLRC && hPalette && (HWND)wParam != hWnd)
	{	
		wglMakeCurrent(hDC, hGLRC);
		UnrealizeObject(hPalette);
		SelectPalette(hDC, hPalette, FALSE);
		RealizePalette(hDC);
	}
}

// QueryNewPalette()
// desc: query a new palette
BOOL COGLWindow::QueryNewPalette()
{
	if (hGLRC && hPalette)
	{	
		wglMakeCurrent(hDC, hGLRC);
		UnrealizeObject(hPalette);
		SelectPalette(hDC, hPalette, FALSE);
		RealizePalette(hDC);

		return TRUE;
	}
	return FALSE;
}

// Paint()
// desc: window paint
void COGLWindow::Paint()
{
	PAINTSTRUCT ps;
	BeginPaint(hWnd, &ps);
	EndPaint(hWnd, &ps);
}

// Size()
// desc: window resizing
void COGLWindow::Size()
{
	if (width > height)
		aspect = width;
	else
		aspect = height;

	if (hGLRC)
	{
		glViewport(0, 0, width, height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		float nearClip = 1.0f / 100.0f;
		float farClip  = 256.0f;//127.0f;

		/*glFrustum(
			-0.50f * width  / aspect * nearClip,
			 0.50f * width  / aspect * nearClip,
			-0.50f * height / aspect * nearClip,
			 0.50f * height / aspect * nearClip, nearClip, farClip);
		*/
		gluPerspective(54.0f, (float)width/(float)height, 0.1f, 200.0f);                                                                                                                                                      //s

		OnSize();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
}

// GetPosX()
// desc: get mouse X normalized position
float COGLWindow::GetNormalizedPosX(LPARAM lParam)
{	
	return mouseSensitivity * (float)((short)LOWORD(lParam) - width/2) / aspect;
}

// GetPosY()
// desc: get mouse Y normalized position
float COGLWindow::GetNormalizedPosY(LPARAM lParam)
{
	return mouseSensitivity * (float)((short)HIWORD(lParam) - height/2) / aspect;
}

// GetMouseX()
// desc: get mouse cursor position in window
int COGLWindow::GetMouseX(LPARAM lParam)
{
	return LOWORD(lParam);
}

// GetMouseY()
// desc: get mouse cursor position in window
int COGLWindow::GetMouseY(LPARAM lParam)
{
	return HIWORD(lParam);
}

// WndProcOGL()
// desc: the WndProc for the OpenGL window
LRESULT APIENTRY WndProcOGL(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	COGLWindow *glWindow = (COGLWindow*)GetWindowLong(hWnd, GWL_USERDATA);

	// make sure window has been created
	if ((glWindow == NULL) && (uMsg != WM_CREATE))
	{	
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	// dispatch messages
	switch (uMsg)
	{	
		case WM_CREATE:			// window creation
		{
			HINSTANCE hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
			glWindow = (COGLWindow*)(((LPCREATESTRUCT)lParam)->lpCreateParams);

			SetWindowLong(hWnd, GWL_USERDATA, (LONG)glWindow);
			glWindow->hWnd = hWnd;

			return glWindow->Create();
		}
		case WM_QUIT:
		case WM_CLOSE:				// window close
			glWindow->Destroy();
			PostQuitMessage(0);
			return 0;

		case WM_DESTROY:			// window destroy
			glWindow->Destroy();
			PostQuitMessage(0);
			return 0;

		case WM_ACTIVATEAPP:		// activate app
			
			if (wParam)
			{	
				if (glWindow->fullscreen)
					glWindow->BeginFullScreen(glWindow->width, glWindow->height, glWindow->bits);

				ShowWindow(hWnd, SW_RESTORE);
				UpdateWindow(hWnd);
			}
			else
			{	
				ShowWindow(hWnd, SW_MINIMIZE);
				UpdateWindow(hWnd);
				
				if (glWindow->fullscreen)
					glWindow->EndFullScreen();
			}
			return 0;

		case WM_PALETTECHANGED:		// palette change
			glWindow->PaletteChanged(wParam);
			return 0;

		case WM_QUERYNEWPALETTE:		// new palette
			return glWindow->QueryNewPalette();

		case WM_PAINT:				// paint
			glWindow->Paint();
			return 0;

		case WM_SIZE:				// window size
			if (wParam != SIZE_MINIMIZED)
			{	
				glWindow->width = LOWORD(lParam);
				glWindow->height= HIWORD(lParam);
				glWindow->Size();
			}
			return 0;

		case WM_LBUTTONDOWN:		// left mouse button
			if (!glWindow->useDInput)
			{
				SetCapture(hWnd);
				Drag = lParam;
				bLMB = true;
				glWindow->OnMouseDownL(glWindow->GetNormalizedPosX(lParam), glWindow->GetNormalizedPosY(lParam));
			}
			break;

		case WM_RBUTTONDOWN:		// right mouse button
			if (!glWindow->useDInput)
			{
				SetCapture(hWnd);
				Drag = lParam;
				bRMB = true;
				glWindow->OnMouseDownR(glWindow->GetNormalizedPosX(lParam), glWindow->GetNormalizedPosY(lParam));
			}
			break;

		case WM_MOUSEMOVE:			// mouse movement
		{	
			if (!glWindow->useDInput)
			{
				int x  = glWindow->mouseX = glWindow->GetMouseX(lParam); 
				int y  = glWindow->mouseY = glWindow->GetMouseY(lParam);
				int dx = x - glWindow->GetMouseX(Drag);
				int dy = y - glWindow->GetMouseY(Drag);

				glWindow->OnMouseMove(x,y, glWindow->width, glWindow->height);

				if (GetCapture() == hWnd)
				{
					// left mouse button
					if (bLMB)
					{
						glWindow->OnMouseDragL(x,y, dx,dy);
					}
					
					// right mouse button
					if (bRMB)
					{	
						glWindow->OnMouseDragR(x,y, dx,dy);
					}

					Drag = lParam;
				}
			}
			break;
		}

		case WM_LBUTTONUP:			// left button release
			if (!glWindow->useDInput)
			{
				if ((GetCapture() == hWnd) && !bRMB)
				{
					ReleaseCapture();
				}

				bLMB = false;
				glWindow->OnMouseUpL();
			}
			break;

		case WM_RBUTTONUP:			// right button release
			if (!glWindow->useDInput)
			{
				if ((GetCapture() == hWnd) && !bLMB)
				{
					ReleaseCapture();
				}
				bRMB = false;
				glWindow->OnMouseUpR();
			}
			break;

		case WM_KEYUP:
			if (!glWindow->useDInput)
			{
				glWindow->OnKeyUp((int)wParam);
			}
			return 0;

		case WM_KEYDOWN:
			if (!glWindow->useDInput)
			{
				glWindow->OnKeyDown((int)wParam);
			}
			return 0;

		default:
			break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// RegisterWindow()
// desc: register the window class
bool COGLWindow::RegisterWindow(HINSTANCE hInst)
{
	WNDCLASS windowClass;		// the window class

	windowClass.style		 = 0;           //窗口类型为缺省类型
	windowClass.lpfnWndProc	 = WndProcOGL;	//指定窗口消息处理函数
	windowClass.cbClsExtra	 = 0;			//窗口类无扩展 
	windowClass.cbWndExtra	 = 0;			//窗口实例无扩展
	windowClass.hInstance	 = hInst;		//窗口句柄当前实例句柄
	windowClass.hIcon		 = 0;			//窗口为缺省图标
	windowClass.hCursor		 = LoadCursor(NULL, IDC_ARROW); //窗口采用箭头光标
	windowClass.hbrBackground = NULL;       //窗口背景为默认颜色
	windowClass.lpszMenuName	 = NULL;	//窗口无菜单
	windowClass.lpszClassName = "Engine";	//窗口类名为“Engine”

	// register the window class (注册窗口类，一会Create窗口的时候将按照注册的窗口类来创建窗口)
	if (!RegisterClass(&windowClass))
		return false;

	return true;
}

// BeginFullScreen()
// desc: go to full screen widthXheight resolution
//	    and save original state
void COGLWindow::BeginFullScreen(int width, int height, int bits)
{
	// Save current display state
	iPrevWidth = GetSystemMetrics(SM_CXSCREEN);
	iPrevHeight = GetSystemMetrics(SM_CYSCREEN);
	
	// Set new display state
	DEVMODE dm;
	memset(&dm, 0, sizeof(dm));
	dm.dmSize	= sizeof(DEVMODE);
	dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
	dm.dmPelsWidth	= width;
	dm.dmPelsHeight = height;
	dm.dmBitsPerPel = bits;
	ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
}

// EndFullScreen()
// desc: return to desktop at original resolution
void COGLWindow::EndFullScreen()
{
	ChangeDisplaySettings(NULL, 0);
}

// constructor
COGLWindow::COGLWindow(const char *szName, bool fscreen, int w, int h, int b)
{
	RECT windowRect;
	DWORD dwStyle;
	DWORD dwExStyle;

	fullscreen = fscreen;
	width = w;
	height = h;
	bits = b;

	windowRect.left = (long)0;
	windowRect.right = (long)width;
	windowRect.top = (long)0;
	windowRect.bottom = (long)height;

	if (fullscreen)
	{
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW;
	}

	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);
	
	// create the window(创建窗口）
	if (fullscreen)
	{
		BeginFullScreen(width, height, bits);
		hWnd = CreateWindowEx(NULL, "Engine", szName, dwStyle | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
						  0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
						  NULL, NULL, (HINSTANCE)GetModuleHandle(NULL), (void*)this);
	}
	else
	{

		hWnd = CreateWindowEx(NULL, "Engine", szName, dwStyle | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
						  0, 0, width, height, NULL, NULL, (HINSTANCE)GetModuleHandle(NULL),(void*)this);
	}

	if (hWnd == NULL) 
		throw "ERROR: Creating OpenGL Window!";

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	mouseSensitivity = 2.0f;

	//创建并初始化支持键盘和鼠标的输入系统
	//（可把CInputSystem当作代码资源）
	inputSystem = new CInputSystem;
	useDInput = inputSystem->Initialize(hWnd, (HINSTANCE)GetModuleHandle(NULL), true, IS_USEKEYBOARD | IS_USEMOUSE);

	SetForegroundWindow(hWnd); //窗口置于最前(hWnd);
	SetCapture(hWnd);     //在窗口里设置鼠标捕获
	SetFocus(hWnd);	      //窗口取得焦点

	if (useDInput)
	{
		inputSystem->AcquireAll();
		inputSystem->Update();
	}

	ShowCursor(FALSE);
}

// destructor
COGLWindow::~COGLWindow()
{
	delete inputSystem;

	if (hGLRC)
		Destroy();

	if (fullscreen)
		EndFullScreen();

	ShowCursor(TRUE);
}
