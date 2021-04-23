#ifndef __SIMPENGINE
#define __SIMPENGINE

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN


#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include "engine.h"
#include "object.h"
#include "camera.h"
#include "terrain.h"
#include "world.h"

class CSimpEngine : public CEngine
{
private:
	CCamera *gameCamera;
	CWorld *gameWorld;

protected:
	CCamera *OnGetCamera() { return gameCamera; }
	CWorld *OnGetWorld() { return gameWorld; }

	void OnPrepare();
	void OnMouseDownL(float x, float y);
	void OnMouseMove(int deltaX, int deltaY);
	void OnMouseMove(int x, int y, int centerX, int centerY);
	void OnKeyDown(int nVirtKey);

public:
	CSimpEngine() 
	{
		gameCamera = new CCamera;
		gameWorld = new CWorld;
	}

	CSimpEngine(const char *szName, bool fscreen, int w, int h, int b) :
		CEngine(szName, fscreen, w, h, b) 
	{
		gameCamera = new CCamera;
		gameWorld = new CWorld(gameCamera);                       //in
		gameCamera->centerX = centerX;
		gameCamera->centerY = centerY;
		gameWorld->SetScreen(w,h);
	}

	~CSimpEngine() 
	{
		delete gameWorld;
		delete gameCamera;
		gameWorld = NULL;
		gameCamera = NULL;
	}
};


#endif