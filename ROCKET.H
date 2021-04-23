#ifndef __ROCKET_H
#define __ROCKET_H


#include "explosion.h"
#include "terrain.h"
#include "player.h"
#include "camera.h"
#include "object.h"
#include "vector.h"

class CRocket : public CEntity
{
private:
	void SetupExplosionTexture();

protected:
	void OnAnimate(scalar_t deltaTime);
	void OnCollision(CObject *collisionObject);
	void OnDraw(CCamera *camera);
	void OnPrepare();

public:
	// 火箭指向的方向
	float pitch;
	float distanceTravel;	// 火箭飞行的距离
	CVector forward;		// 火箭向前飞行的方向
	bool isExplosion;

	CTexture *explosionTex;
	CExplosion *explosion;

	CRocket();
	~CRocket();

	void Load();
	void Unload();
};

#endif