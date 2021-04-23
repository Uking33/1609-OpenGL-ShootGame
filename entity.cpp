#include "entity.h"

CEntity::CEntity()
{
	interpol = 3.0f;
	stateStart = 0;
	stateEnd = 39;
	animSpeed = 7.0f;
	direction = 0.0f;
	entitySound = NULL;

	position = CVector(0,0,0);
	velocity = CVector(0,0,0);
	acceleration = CVector(0,0,0);
	size = 6.25f;
}

CEntity::~CEntity()
{
	if (entitySound != NULL)
	{
		delete entitySound;
		entitySound = NULL;
	}
}

void CEntity::OnAnimate(float deltaTime)
{
	float cosYaw = (float)cos(DEG2RAD(direction));
	float sinYaw = (float)sin(DEG2RAD(direction)); 

	float speed = velocity.z * deltaTime;

	position.x += float(cosYaw)*speed;
	position.z += float(sinYaw)*speed;

	deltaT = deltaTime;	// used for interpolation
}

void CEntity::OnCollision(CObject *collisionObject) 
{
	if (typeid(*collisionObject) == typeid(CEntity))   //��ɫ��������ֹͣ�ܶ���Ϊ����״̬
	{
		modelState = MODEL_IDLE;
		velocity = CVector(0.0, 0.0, 0.0);
	}

	if (typeid(*collisionObject) == typeid(CTerrain))  //����ν�����ײ��⣬�ý�ɫվ�ڵ���֮��
	{
		position.y = ((CTerrain*)collisionObject)->GetHeight(position.x, position.z) + size;
	}
}

void CEntity::OnDraw(CCamera *camera)
{
	glTranslatef(position.x, position.y, position.z);//��ͼ����ϵƽ�Ƶ���ɫ����Ҫ������λ��
	glRotatef(-direction, 0.0, 1.0, 0.0); //��ͼ����ϵ��ת�ɽ�ɫҪ����ķ���
	glRotatef(90.0f, -1.0f, 0.0f, 0.0f);  //��ģʱ��z�����ϵģ����ڳ���ʱy�����ϣ����Ի�ͼ����ϵ��ת����һ��
	glScalef(0.25, 0.25, 0.25);			  //��ģʱ����̫���ˣ����1/4������
		
	AnimateModel(stateStart, stateEnd, deltaT*animSpeed); //���ùؼ�֡��ֵ������ɫ���γ�ƽ���Ķ���
}

void CEntity::OnPrepare()
{
	/*
	   Frame#  Action
	   ----------------
	   0-39    idle
	   40-46   running
	   47-60   getting shot but not falling (back bending)
	   61-66   getting shot in shoulder
	   67-73   jumping
	   74-95   idle
	   96-112  getting shot and falling down
	   113-122 idle
	   123-135 idle
	   136-154 crouch
	   155-161 crouch crawl
	   162-169 crouch adjust weapon (idle)
	   170-177 kneeling dying
	   178-185 falling back dying
	   186-190 falling forward dying
	   191-198 falling back slow dying
	*/

	switch (modelState)
	{
	case MODEL_IDLE:
		stateStart = 0;
		stateEnd = 39;
		break;
	case MODEL_CROUCH:
		break;
	case MODEL_RUN:
		stateStart = 40;
		stateEnd = 46;
		velocity = CVector(0.0, 0.0, 2.0);
		break;
	case MODEL_JUMP:
		stateStart = 67;
		stateEnd = 73;
		break;
	case MODEL_DIE:
		stateStart = 178;
		stateEnd = 184;
		break;
	default:
		stateStart = 0;
		stateEnd = 1;
		break;
	}

	// perform collision detection from this entity with all other objects in world
	ProcessCollisions(FindRoot());
}

void CEntity::LoadAudio(CAudioSystem *audioSystem, char *filename, bool is3DSound)
{
	if (entitySound != NULL)
	{
		delete [] entitySound;
		entitySound = new CAudio;
	}

	if (audioSystem != NULL)
		entitySound = audioSystem->Create(filename, is3DSound);
	else
	{
		delete [] entitySound;
		entitySound = NULL;
	}
}