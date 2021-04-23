#include "sod.h"
#include "rocket.h"

void CSodEnemy::OnCollision(CObject *collisionObject)
{
		// 只有模型没有死亡，才有可能与其它对象碰撞
	if (modelState != MODEL_DIE)
	{
		// 如果敌人与另外的敌人碰撞
		if ((typeid(*collisionObject) == typeid(CSodEnemy)) ||
			(typeid(*collisionObject) == typeid(COgroEnemy)))
		{
			aiState = AI_UNCARING;
		}

		// 如果敌人与地面碰撞
		else	if (typeid(*collisionObject) == typeid(CTerrain))
		{
			position.y = ((CTerrain*)collisionObject)->GetHeight(position.x, position.z) + size;
			//不能走出界，也不要离边界太近
			if (position.x <= ((CTerrain*)collisionObject)->GetScanDepth())
				position.x = ((CTerrain*)collisionObject)->GetScanDepth();
			if (position.x >= ((CTerrain*)collisionObject)->GetWidth()*((CTerrain*)collisionObject)->GetMul() - ((CTerrain*)collisionObject)->GetScanDepth())
				position.x = ((CTerrain*)collisionObject)->GetWidth()*((CTerrain*)collisionObject)->GetMul() - ((CTerrain*)collisionObject)->GetScanDepth();
			if (position.z <= ((CTerrain*)collisionObject)->GetScanDepth())
				position.z = ((CTerrain*)collisionObject)->GetScanDepth();
			if (position.z >= ((CTerrain*)collisionObject)->GetWidth()*((CTerrain*)collisionObject)->GetMul() - ((CTerrain*)collisionObject)->GetScanDepth())
				position.z = ((CTerrain*)collisionObject)->GetWidth()*((CTerrain*)collisionObject)->GetMul() - ((CTerrain*)collisionObject)->GetScanDepth();
		}
		else if (typeid(*collisionObject) == typeid(CRocket))
		{
			// 杀死sod
			aiState = AI_DEAD;
			velocity = CVector(0.0, 0.0, 0.0);
		}
	}
}

void CSodEnemy::OnPrepare() 
{
	float dirToPlayer;	// 敌人与游戏者的角度方向

	CVector diff;		// 从敌人到游戏者的方向矢量
	diff.x = position.x - player->position.x;
	diff.z = position.z - player->position.z;
	diff.Normalize();

	// 在场景中计算从敌人到游戏者的方向矢量与z轴负方向的夹角
	dirToPlayer = RAD2DEG(diff.Angle(CVector(0,0,-1)));

	// 产生随机数种子
	srand((unsigned int)time(NULL));

	// 检查是否应变为受惊状态（离Player太近会受惊逃跑）
	ProcessAI();
	// 根据AIstate设置模型状态
	switch (aiState)
	{
	case AI_SCARED:
		direction = (dirToPlayer - 90) + ((rand()%30)-15);		// set the direction of the enemy
		modelState = MODEL_RUN;
		velocity = CVector(0.0, 0.0, 15.0);
		break;
	case AI_UNCARING:
		direction = float(rand() % 360);
		if ((rand() % 3) != 0)
		{
			modelState = MODEL_IDLE;
			velocity = CVector(0.0, 0.0, 0.0);
		}
		else
		{
			velocity = CVector(0.0, 0.0, 15.0);
			modelState = MODEL_RUN;
		}
		break;
	case AI_DEAD:
		modelState = MODEL_DIE;
		velocity = CVector(0.0, 0.0, 0.0);
		if (nextFrame == stateStart)
			// 杀死魔鬼
		{
			isDead = true;
		}
		break;
	default:
		break;
	}

	// 准备MD2模型
	CEntity::OnPrepare();
}

void CSodEnemy::Load()
{
	// 装人三维模型
	CMD2Model::Load("models\\sodf8\\tris.md2", "models\\sodf8\\abarlith.pcx");
}

void CSodEnemy::OnProcessAI()
{
	// 计算与游戏者之间的距离
	CVector diff = player->position - position;

	if (aiState != AI_DEAD)
	{
		distFromPlayer = sqrt(diff.x*diff.x + diff.y*diff.y + diff.z*diff.z);
		if (distFromPlayer < 125.0)
			aiState = AI_SCARED;
		else
			aiState = AI_UNCARING;
	}
}



