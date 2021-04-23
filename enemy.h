#ifndef __ENEMY_H
#define __ENEMY_H


#include "entity.h"
#include "audiosystem.h"
#include "vector.h"

class CPlayer;
class CTerrain;

enum AIState_t
{
	AI_UNCARING,	// 没有受到惊恐的敌人
	AI_SCARED,		// 受到惊恐的敌人，并且迅速跑开
	AI_DEAD
};

class CEnemy : public CEntity
{
private:

protected:
     int hitPoints;           // 攻击点  //程序没用到这个变量
     float distFromPlayer;    // 与敌人的距离
     float runSpeed;          // 敌人跑走的速度
     AIState_t aiState;       // 敌人的状态

     virtual void OnProcessAI() {}

     void OnCollision(CObject *collisionObject)
     {
          // 敌人之间相互碰撞
          if (typeid(*collisionObject) == typeid(CEnemy))
          {
               modelState = MODEL_IDLE;
               velocity = CVector(0.0, 0.0, 0.0);
          }
          // 敌人与地面碰撞
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
          hitPoints = 100;    // 100个攻击点  //程序没用到这个变量
          isDead = false;     // 所有敌人都存活
          velocity = CVector(0.0, 0.0, 0.0); // 敌人的速度
          runSpeed = velocity.z;
          SetState(MODEL_IDLE);    // 空闲状态
          direction = 0.0f;        // 方向向北
          player = NULL;
     }

     ~CEnemy() {}

     void ProcessAI() { OnProcessAI(); }
     void SetPlayer(CPlayer *p) { player = p; }
};

#endif