#include "gui.h"
#include <math.h>

CGUI::CGUI()
{
	minutesLeft = secondsLeft = enemiesLeft = 0;
	font = new CFont("Arial", 16);
	crosshair = new CFont("Courier New", 28);
	endText = new CFont("Arial", 40);
}

CGUI::~CGUI()
{
	font->ClearFont();
	crosshair->ClearFont();
	endText->ClearFont();
	delete font;
	delete crosshair;
	delete endText;
}

void CGUI::SetCurrentTime(float timeLeft)
{
	// 经过的时间（以秒为单位）
	minutesLeft = (int)(timeLeft / 60.0);	
	secondsLeft = (int)floor(timeLeft) % 60;
	millisecondsLeft = (timeLeft - (float)floor(timeLeft)) * 60;
}

void CGUI::SetEnemiesLeft(int eLeft)
{
	enemiesLeft = eLeft;
}

void CGUI::DrawWinner()
{
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	endText->SetPos3D(-0.25, 0.15, -0.1);
	endText->SetRGBA(0.3, 1.0, 0.3, 0.8);
	endText->Print("YOU WIN!");

	endText->SetPos3D(-0.5, -0.2, -0.1);
	endText->SetRGBA(1.0, 1.0, 1.0, 0.8);
	endText->Print("Press the ESC key to exit");
	
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
}

void CGUI::DrawLoser()
{
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);        //启用混合功能
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //设置混合方式（这是最普通的混合方式）
	endText->SetPos3D(-0.25, 0.15, -0.1);
	endText->SetRGBA(1.0, 0.3, 0.3, 0.8);
	endText->Print("YOU LOSE!");

	endText->SetPos3D(-0.5, -0.2, -0.1);
	endText->SetRGBA(1.0, 1.0, 1.0, 0.8);
	endText->Print("Press the ESC key to exit");
	
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
}

void CGUI::Draw()
{
	glDisable(GL_TEXTURE_2D);
	font->SetPos3D(2.5, 2.9, -5.0);
	font->SetRGB(0.2, 0.0, 1.0);
	if (secondsLeft < 10)
	{
		if (millisecondsLeft < 10)
			font->Print("Time: %d:0%d.0%d", minutesLeft, secondsLeft, millisecondsLeft);
		else
			font->Print("Time: %d:0%d.%d", minutesLeft, secondsLeft, millisecondsLeft);
	}
	else
	{
		if (millisecondsLeft < 10)
			font->Print("Time: %d:%d.0%d", minutesLeft, secondsLeft, millisecondsLeft);
		else
			font->Print("Time: %d:%d.%d", minutesLeft, secondsLeft, millisecondsLeft);
	}

	font->SetPos3D(2.5, 2.7, -5.0);
	font->Print("Enemies: %d", enemiesLeft);

	glEnable(GL_BLEND);  //启用混合功能  
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  //设置混合方式
	crosshair->SetRGBA(1.0, 0.1, 0.1, 0.7);
	crosshair->SetPos3D(-0.03, -0.03, -5.0);
	crosshair->Print("+");
	
	glDisable(GL_BLEND); //关闭混合功能
	glEnable(GL_TEXTURE_2D);
}