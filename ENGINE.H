#ifndef __ENGINE_H
#define __ENGINE_H

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include "world.h"
#include "oglwindow.h"
#include "camera.h"
#include "HiResTimer.h"

#include <windows.h>
#include <dinput.h>
#include <gl/gl.h>
#include <gl/glu.h>

class CEngine : public COGLWindow
{
private:

protected:
	CHiResTimer *timer;							// high performance timer

	virtual void GameCycle(float deltaTime);

	virtual void OnPrepare() {}					// setup OpenGL for frame
	virtual CCamera *OnGetCamera() { return NULL; }	// override in derived engine
	virtual CWorld *OnGetWorld() { return NULL; }

	virtual void CheckInput(float deltaTime);

public:
	CEngine() {}
	CEngine(const char *szName, bool fscreen, int w, int h, int b) : 
			COGLWindow(szName, fscreen, w, h, b) {}
	~CEngine() {}
	LRESULT EnterMessageLoop();
};

#endif