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
	CVector  m_pos;           // ���ӵĵ�ǰ��λ��
	CVector  m_prevPos;       // ���ӵ�ǰһλ��
	CVector  m_velocity;      // ���ӵķ�����ٶ�
	CVector  m_acceleration;  // ���ӵļ��ٶ�

	float   m_energy;           // ���Ӵ���ʱ��
	float   m_size;             // ���ӵĴ�С
	float   m_sizeDelta;        // ���Ӵ�С�ĸı���
	float   m_weight;           // ���ӵ�����
	float   m_weightDelta;      // ���������ĸı�
	float   m_color[4];         // ���ӵ�ǰ����ɫ
	float   m_colorDelta[4];    // ������ɫ�ĸı���
};

class CParticleSystem
{
public:
	CParticleSystem(int maxParticles, CVector origin);
	// �麯��
	virtual void  Update(float elapsedTime)     = 0;
	virtual void  Render()                      = 0;
	virtual int   Emit(int numParticles);
	virtual void  InitializeSystem();
	virtual void  KillSystem();
	int         m_numParticles;       // �����������ӵ�����
protected:
	virtual void  InitializeParticle(int index) = 0;
	particle_t *m_particleList;		// �����������
	int         m_maxParticles;		// ���ӵ������Ŀ

	CVector     m_origin;             // ����ϵͳ������
	float       m_accumulatedTime;    // ������ӷ���󾭹���ʱ��
	CVector     m_force;              // ����������ϵͳ�ϵ���
};


#endif // __PARTICLES_H_INCLUDED__
