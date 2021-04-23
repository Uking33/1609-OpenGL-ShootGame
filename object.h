#ifndef __OBJECT_H
#define __OBJECT_H

#include "tree.h"
#include "vector.h"
#include "camera.h"


class CObject : public CNode
{
protected:

     // perform basic physics on the object
     virtual void OnAnimate(scalar_t deltaTime) 
     {
          position += velocity * deltaTime;
          velocity += acceleration * deltaTime;
     }

     // draw the object given the camera position
     virtual void OnDraw(CCamera *camera) {} 

     // collide with other objects
     virtual void OnCollision(CObject *collisionObject) {}

     // perform collision and other preparations for this object
     virtual void OnPrepare() 
     {
          ProcessCollisions(FindRoot());     // perform collisions starting with
                                             // root world object
     }

public:
     CVector position;        // object position
     CVector velocity;        // velocity of object
     CVector acceleration;    // acceleration
     scalar_t size;           // size of bounding sphere (radius)

     bool isDead;

     CObject() { isDead = false; }
     ~CObject() {}

     virtual void Load() {}
     virtual void Unload() {}

     // draw object
     void Draw(CCamera *camera) 
     {
          // push modelview matrix on stack
          glPushMatrix();
               OnDraw(camera);          // draw this object
               if (HasChild())          // draw children
                    ((CObject*)childNode)->Draw(camera);
          glPopMatrix();

          // draw siblings
          if (HasParent() && !IsLastChild())
               ((CObject*)nextNode)->Draw(camera);
     }

     // animate object
     void Animate(scalar_t deltaTime) 
     {
          OnAnimate(deltaTime);         // animate this object
     
          // animate children
          if (HasChild())
               ((CObject*)childNode)->Animate(deltaTime);

          // animate siblings
          if (HasParent() && !IsLastChild())
               ((CObject*)nextNode)->Animate(deltaTime);

          if (isDead)
               delete this;
     }

     // perform collision detection（碰撞检测操作）
     void ProcessCollisions(CObject *obj) 
     {
          // if this object's bounding sphere collides with obj's sphere   // 如果本对象的边界球与obj对象的边界球碰撞 &&
          // and obj is not this object                                    // 本对象不是obj对象
          if (((obj->position - position).Length() <= (obj->size + size)) &&
               (obj != ((CObject*)this)))
          {
               OnCollision(obj);        // perform this object's collision with obj( 执行本对象与obj对象的碰撞操作)

               // test child collisions with obj(测试子对象与obj对象的碰撞)
               if (HasChild())
                    ((CObject*)childNode)->ProcessCollisions(obj);

               // test sibling collisions with obj(测试兄弟对象与obj对象的碰撞)
               if (HasParent() && !IsLastChild())
                    ((CObject*)nextNode)->ProcessCollisions(obj);
          }

          // if obj has children, check collisions with these children(如果obj对象有子对象，则检测与所有子对象的碰撞)
          if (obj->HasChild())
               ProcessCollisions((CObject*)(obj->childNode));

          // if obj has siblings, check collisions with these siblings(如果obj对象有兄弟对象，则检测与所有兄弟对象的碰撞)
          if (obj->HasParent() && !obj->IsLastChild())
               ProcessCollisions((CObject*)(obj->nextNode));
     }

     // prepare object
     void Prepare()
     {
          OnPrepare();                            // prepare this object(本节点Prepare）

          if (HasChild())                         // prepare children(子节点也Prepare)
               ((CObject*)childNode)->Prepare();

          if (HasParent() && !IsLastChild())      // prepare siblings(兄弟节点也Prepare)
               ((CObject*)nextNode)->Prepare();
     }

     // find root object of cyclic linked list tree（查找循环链表的根节点）
     CObject *FindRoot()
     {
          // if this object has a parent node, return the root of the parent node（如果该节点有父节点，则返回父节点的根节点）
          if (parentNode)
               return ((CObject*)parentNode)->FindRoot();

          return this;
     }
};


#endif