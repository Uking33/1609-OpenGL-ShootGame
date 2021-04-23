#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include "simpengine.h"

void CSimpEngine::OnPrepare()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_SMOOTH);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
}

void CSimpEngine::OnMouseDownL(float x, float y) 
{
	if (!gameWorld->IsGameDone())
		gameWorld->player->FireWeapon();
}

void CSimpEngine::OnKeyDown(int nVirtKey)
{
	if (!gameWorld->IsGameDone())
	{
		switch (nVirtKey)
		{
		case VK_UP:
			gameCamera->velocity += CVector(0,0,2.0);
			break;
		case VK_DOWN:
			gameCamera->velocity += CVector(0,0,-2.0);
			break;
		case VK_RIGHT:
			gameCamera->velocity += CVector(1.0, 0.0, 0.0);
			break;
		case VK_LEFT:
			gameCamera->velocity += CVector(-1.0, 0.0, 0.0);
			break;
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case VK_ADD:
			mouseSensitivity += 0.05f;
			break;
		case VK_SUBTRACT:
			mouseSensitivity -= 0.05f;
			if (mouseSensitivity < 0.05)
				mouseSensitivity = 0.05;
			break;
		default:
			break;
		}
	}
	else
	{
		if ((nVirtKey) == VK_ESCAPE)
			PostQuitMessage(0);
	}
}

void CSimpEngine::OnMouseMove(int deltaX, int deltaY)
{
	if (!gameWorld->IsGameDone())
	{
		gameCamera->yaw += deltaX * mouseSensitivity;
		gameCamera->pitch -= deltaY * mouseSensitivity;
	}
}

void CSimpEngine::OnMouseMove(int x, int y, int centerX, int centerY)
{
	static float oldX;
	static float oldY;
	static float yaw = 0.0f;
	static float pitch = 0.0f;

	float mX, mY;

	mX = (float)x;
	mY = (float)y;

	if (mX < centerX/2)
		gameCamera->yaw -= 0.25f*mouseSensitivity;
	if (mX > centerX/2)
		gameCamera->yaw += 0.25f*mouseSensitivity;

	if (mY < centerY/2)
		gameCamera->pitch += 0.25f*mouseSensitivity;
	if (mY > centerY/2)
		gameCamera->pitch -= 0.25f*mouseSensitivity;

	oldX = mX;
	oldY = mY;
}