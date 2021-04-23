#ifndef __ENEMY_H
#define __ENEMY_H


#include "entity.h"
#include "audiosystem.h"
#include "vector.h"

class CPlayer;
class CTerrain;

enum AIState_t
{
	AI_UNCARING,	// û���ܵ����ֵĵ���
	AI_SCARED,		// �ܵ����ֵĵ��ˣ�����Ѹ���ܿ�
	AI_DEAD
};

class CEnemy : public CEntity
{
private:

protected:
     int hitPoints;           // ������  //����û�õ��������
     float distFromPlayer;    // ����˵ľ���
     float runSpeed;          // �������ߵ��ٶ�
     AIState_t aiState;       // ���˵�״̬

     virtual void OnProcessAI() {}

     void OnCollision(CObject *collisionObject)
     {
          // ����֮���໥��ײ
          if (typeid(*collisionObject) == typeid(CEnemy))
          {
               modelState = MODEL_IDLE;
               velocity = CVector(0.0, 0.0, 0.0);
          }
          // �����������ײ
          else if (typeid(*collisionObject) == typeid(CTerrain))
          {
               position.y = ((CTerrain*)collisionObject)->GetHeight(position.x, position.z) + size;
          }
          else
          {
          }
     }

public:
     CPlayer *player;

     CEnemy() 
     {
          hitPoints = 100;    // 100��������  //����û�õ��������
          isDead = false;     // ���е��˶����
          velocity = CVector(0.0, 0.0, 0.0); // ���˵��ٶ�
          runSpeed = velocity.z;
          SetState(MODEL_IDLE);    // ����״̬
          direction = 0.0f;        // ������
          player = NULL;
     }

     ~CEnemy() {}

     void ProcessAI() { OnProcessAI(); }
     void SetPlayer(CPlayer *p) { player = p; }
};

#endif