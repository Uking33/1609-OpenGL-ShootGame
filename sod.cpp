#include "sod.h"
#include "rocket.h"

void CSodEnemy::OnCollision(CObject *collisionObject)
{
		// ֻ��ģ��û�����������п���������������ײ
	if (modelState != MODEL_DIE)
	{
		// �������������ĵ�����ײ
		if ((typeid(*collisionObject) == typeid(CSodEnemy)) ||
			(typeid(*collisionObject) == typeid(COgroEnemy)))
		{
			aiState = AI_UNCARING;
		}

		// ��������������ײ
		else	if (typeid(*collisionObject) == typeid(CTerrain))
		{
			position.y = ((CTerrain*)collisionObject)->GetHeight(position.x, position.z) + size;
			//�����߳��磬Ҳ��Ҫ��߽�̫��
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
			// ɱ��sod
			aiState = AI_DEAD;
			velocity = CVector(0.0, 0.0, 0.0);
		}
	}
}

void CSodEnemy::OnPrepare() 
{
	float dirToPlayer;	// ��������Ϸ�ߵĽǶȷ���

	CVector diff;		// �ӵ��˵���Ϸ�ߵķ���ʸ��
	diff.x = position.x - player->position.x;
	diff.z = position.z - player->position.z;
	diff.Normalize();

	// �ڳ����м���ӵ��˵���Ϸ�ߵķ���ʸ����z�Ḻ����ļн�
	dirToPlayer = RAD2DEG(diff.Angle(CVector(0,0,-1)));

	// �������������
	srand((unsigned int)time(NULL));

	// ����Ƿ�Ӧ��Ϊ�ܾ�״̬����Player̫�����ܾ����ܣ�
	ProcessAI();
	// ����AIstate����ģ��״̬
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
			// ɱ��ħ��
		{
			isDead = true;
		}
		break;
	default:
		break;
	}

	// ׼��MD2ģ��
	CEntity::OnPrepare();
}

void CSodEnemy::Load()
{
	// װ����άģ��
	CMD2Model::Load("models\\sodf8\\tris.md2", "models\\sodf8\\abarlith.pcx");
}

void CSodEnemy::OnProcessAI()
{
	// ��������Ϸ��֮��ľ���
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



