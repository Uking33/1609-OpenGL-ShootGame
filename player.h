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
		if (typeid(*collisionObject) == typeid(CTerrain))  //Player��������ײ����Playerվ�ڵ�������
		{
			position.y = ((CTerrain*)collisionObject)->GetHeight(position.x, position.z) + size;                                                                               //s                                                              //s
		}
		else if (typeid(*collisionObject) == typeid(COgroEnemy)) //Player�ڵ���COgroEnemy��������ͣ����
		{
			if (camera)
			{
				if (camera->velocity.z > 0.0)
					camera->velocity = CVector(0.0, 0.0, 0.0);
			}
		}
		else if (typeid(*collisionObject) == typeid(CSodEnemy))  //Player�ڵ���CSodEnemy��������ͣ����
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
			position = camera->position;	// ��Ϸ�ߵ�λ���������λ��һ��
			direction = camera->yaw;
			pitch = camera->pitch;
		}
	}

public:
	float direction;	// ��Ϸ����Եķ���
	float pitch;		// ��Ϸ�����ߵ���б��

	CPlayer() { size = 7.0f; camera = NULL; terrain = NULL; audioSys = NULL; rocketSound = NULL; }
	~CPlayer() {}

	void SetCamera(CCamera *c) { camera = c; }
	void DetachCamera() { camera = NULL; }
	void SetTerrain(CTerrain *t) { terrain = t; }
	void FireWeapon();
	void SetAudioSystem(CAudioSystem *aSys) { audioSys = aSys; }
};

#endif