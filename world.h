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
	int numOgros, numSods;         //���ֵ��˵�����
	int screenWidth, screenHeight; //��Ļ��ȡ��߶�
	bool gameDone;                 //��Ϸ�Ƿ����

protected:
	void OnAnimate(float deltaTime);
	void OnDraw(CCamera *camera);
	void OnPrepare();

public:
	HWND hwnd;

	CTerrain *terrain;			// ����
	CCamera *camera;			// �����
	CPlayer *player;			// ��Ϸ��
	CAudioSystem *audioSystem;	// ��Ƶϵͳ
	CAudio *worldSound;			// ��������
	CGUI *gui;

	COgroEnemy *ogroEnemy;		// ogro�����б�
	CSodEnemy *sodEnemy;		// sod�����б�

	float timeStart;
	float timeElapsed;

	CWorld();
	CWorld(CCamera *c);
	~CWorld();
	// ��ʼ�����Σ�װ�����ж���
	void LoadWorld();
	void UnloadWorld();

	int CountObjectTypes(const type_info &classID);
	// �������ж����������㣬������ײ���
	void Animate(float deltaTime);
	// ��Ⱦ���������ж���
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