#ifndef __PLAYER_H
#define __PLAYER_H

#include <typeinfo.h>

#include "audiosystem.h"
#include "sod.h"
#include "ogro.h"
#include "rocket.h"
#include "camera.h"
#include "object.h"
#include "terrain.h"

class CPlayer : public CObject
{
private:
	CCamera *camera;
	CTerrain *terrain;
	CAudioSystem *audioSys;
	CAudio *rocketSound;

protected:
	void OnCollision(CObject *collisionObject)
	{
		if (typeid(*collisionObject) == typeid(CTerrain))  //Player跟地形碰撞，让Player站在地形上面
		{
			position.y = ((CTerrain*)collisionObject)->GetHeight(position.x, position.z) + size;                                                                               //s                                                              //s
		}
		else if (typeid(*collisionObject) == typeid(COgroEnemy)) //Player第敌人COgroEnemy相碰，就停下来
		{
			if (camera)
			{
				if (camera->velocity.z > 0.0)
					camera->velocity = CVector(0.0, 0.0, 0.0);
			}
		}
		else if (typeid(*collisionObject) == typeid(CSodEnemy))  //Player第敌人CSodEnemy相碰，就停下来
		{
			if (camera)
			{
				if (camera->velocity.z > 0.0)
					camera->velocity = CVector(0.0, 0.0, 0.0);
			}
		}
		else if (typeid(*collisionObject) == typeid(CPlayer))
		{
		}
	}

	void OnPrepare()
	{
		if (camera)
		{
			position = camera->position;	// 游戏者的位置与摄像机位置一致
			direction = camera->yaw;
			pitch = camera->pitch;
		}
	}

public:
	float direction;	// 游戏者面对的方向
	float pitch;		// 游戏者视线的倾斜度

	CPlayer() { size = 7.0f; camera = NULL; terrain = NULL; audioSys = NULL; rocketSound = NULL; }
	~CPlayer() {}

	void SetCamera(CCamera *c) { camera = c; }
	void DetachCamera() { camera = NULL; }
	void SetTerrain(CTerrain *t) { terrain = t; }
	void FireWeapon();
	void SetAudioSystem(CAudioSystem *aSys) { audioSys = aSys; }
};

#endif