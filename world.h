#ifndef __WORLD_H
#define __WORLD_H


#include <dmusicc.h>               // DirectMusic includes
#include <dmusici.h>
#include <d3d8types.h>             // for D3DVECTOR
#include <cguid.h>                 // for GUID_NULL
#include <typeinfo.h>				// for RTTI

#include "gui.h"
#include "player.h"
#include "sod.h"
#include "ogro.h"
#include "entity.h"
#include "md2.h"
#include "object.h"
#include "camera.h"
#include "terrain.h"
#include "audiosystem.h"
#include "tree.h"

#define MAX_ENEMIES 5

class CWorld
{
private:
	int numOgros, numSods;         //两种敌人的数量
	int screenWidth, screenHeight; //屏幕宽度、高度
	bool gameDone;                 //游戏是否结束

protected:
	void OnAnimate(float deltaTime);
	void OnDraw(CCamera *camera);
	void OnPrepare();

public:
	HWND hwnd;

	CTerrain *terrain;			// 地形
	CCamera *camera;			// 摄像机
	CPlayer *player;			// 游戏者
	CAudioSystem *audioSystem;	// 音频系统
	CAudio *worldSound;			// 环境音乐
	CGUI *gui;

	COgroEnemy *ogroEnemy;		// ogro敌人列表
	CSodEnemy *sodEnemy;		// sod敌人列表

	float timeStart;
	float timeElapsed;

	CWorld();
	CWorld(CCamera *c);
	~CWorld();
	// 初始化地形，装入所有对象
	void LoadWorld();
	void UnloadWorld();

	int CountObjectTypes(const type_info &classID);
	// 进行所有对象的物理计算，包括碰撞检测
	void Animate(float deltaTime);
	// 渲染场景中所有对象
	void Draw(CCamera *camera);
	void Prepare() { OnPrepare(); }

	void FadeScreen();
	void SetScreen(int width, int height);

	bool IsGameDone() { return gameDone; }
	void QuitGame() { gameDone = true; }
	int GetOgros() { return numOgros; }
	int GetSods() { return numSods; }
	void ReportObjs();
};


#endif