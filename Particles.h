#ifndef __PARTICLES_H_INCLUDED__
#define __PARTICLES_H_INCLUDED__

#include <iostream>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "vector.h"

using namespace std;

class CParticleSystem;

struct particle_t
{
	CVector  m_pos;           // 粒子的当前的位置
	CVector  m_prevPos;       // 粒子的前一位置
	CVector  m_velocity;      // 粒子的方向和速度
	CVector  m_acceleration;  // 粒子的加速度

	float   m_energy;           // 粒子存活的时间
	float   m_size;             // 粒子的大小
	float   m_sizeDelta;        // 粒子大小的改变量
	float   m_weight;           // 粒子的重量
	float   m_weightDelta;      // 粒子重量的改变
	float   m_color[4];         // 粒子当前的颜色
	float   m_colorDelta[4];    // 粒子颜色的改变量
};

class CParticleSystem
{
public:
	CParticleSystem(int maxParticles, CVector origin);
	// 虚函数
	virtual void  Update(float elapsedTime)     = 0;
	virtual void  Render()                      = 0;
	virtual int   Emit(int numParticles);
	virtual void  InitializeSystem();
	virtual void  KillSystem();
	int         m_numParticles;       // 所有自由粒子的索引
protected:
	virtual void  InitializeParticle(int index) = 0;
	particle_t *m_particleList;		// 发射的粒子数
	int         m_maxParticles;		// 粒子的最大数目

	CVector     m_origin;             // 粒子系统的中心
	float       m_accumulatedTime;    // 最后粒子发射后经过的时间
	CVector     m_force;              // 作用在粒子系统上的力
};


#endif // __PARTICLES_H_INCLUDED__
