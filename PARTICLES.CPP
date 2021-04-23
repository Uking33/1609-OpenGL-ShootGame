#include "Particles.h"

// 构造函数
CParticleSystem::CParticleSystem(int maxParticles, CVector origin)
{
	m_maxParticles = maxParticles;
	m_origin = origin;
	m_particleList = NULL;
} 

// 生成新粒子
int CParticleSystem::Emit(int numParticles)
{
	// 生成numParticles个新粒子
	while (numParticles && (m_numParticles < m_maxParticles))
	{
		// 初始化当前粒子，并计数
		InitializeParticle(m_numParticles++);
		--numParticles;
	}
	return numParticles;
} 

// 在系统中为最大数目的粒子分配内存空间
void CParticleSystem::InitializeSystem()
{
	// 如果有粒子存在，则释放内存空间
	if (m_particleList)
	{
		delete[] m_particleList;
		m_particleList = NULL;
	}
	// 分配最大数目的粒子空间
	m_particleList = new particle_t[m_maxParticles];
	// 重置粒子数目和累积时间
	m_numParticles = 0;
	m_accumulatedTime = 0.0f;
} 

// 销毁粒子系统
void CParticleSystem::KillSystem()
{
	if (m_particleList)
	{
		delete[] m_particleList;
		m_particleList = NULL;
	}
	m_numParticles = 0;
} 