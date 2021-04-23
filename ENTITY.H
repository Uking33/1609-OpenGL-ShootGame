#ifndef __ENTITY_H
#define __ENTITY_H

#include <stdlib.h>
#include <time.h>
#include <typeinfo.h>

#include "md2.h"
#include "object.h"
#include "camera.h"
#include "audiosystem.h"
#include "terrain.h"

class CEntity : public CMD2Model
{
protected:
	void OnAnimate(float deltaTime);
	void OnDraw(CCamera *camera);
	void OnCollision(CObject *collisionObject);
	void OnPrepare();

	void PlaySound() { audioSys->Play(entitySound, 1); }

public:
	float direction;		// angle the entity is facing (in degrees)
	CAudio *entitySound;	// the sound the entity makes
						// currently only supports one sound
						// per entity
	CAudioSystem *audioSys;	// a pointer to the audio system


	CEntity();
	~CEntity();

	int stateStart, stateEnd;	// state keyframe start/end
	float deltaT;
	float animSpeed;

	void LoadAudio(CAudioSystem *audioSystem, char *filename, bool is3DSound);
	void SetAudioSystem(CAudioSystem *asys) { audioSys = asys; }
};

#endif