#ifndef __SOD_H
#define __SOD_H

#include "enemy.h"
#include "ogro.h"
#include <time.h>

class CSodEnemy : public CEnemy
{
protected:
	void OnProcessAI();
	void OnCollision(CObject *collisionObject);
	void OnPrepare();

public:
	CSodEnemy() { Load(); }
	CSodEnemy(float x, float z) { position.x = x; position.z = z; Load(); }
	~CSodEnemy() {}

	void Load();
};


#endif