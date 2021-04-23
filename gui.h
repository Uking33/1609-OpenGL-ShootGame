#ifndef __GUI_H
#define __GUI_H

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include <windows.h>
#include "font.h"
#include "texture.h"

class CGUI
{
private:
	int minutesLeft, secondsLeft, millisecondsLeft; //剩下的分钟数、秒数、毫秒数
	int enemiesLeft;   //剩下的敌人数量

	CFont *font;
	CFont *crosshair;
	CFont *endText;
public:
	CGUI();
	~CGUI();

	void SetCurrentTime(float timeLeft);
	void SetEnemiesLeft(int eLeft);
	void Draw();
	void Animate(float deltaTime);
	void DrawWinner();
	void DrawLoser();
};


#endif