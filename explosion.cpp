#include "explosion.h"


CExplosion::CExplosion(int numParticles, CVector origin, float spread, GLuint texture)
  : m_texture(texture), m_spread(spread), CParticleSystem(numParticles, origin)
{
	srand(timeGetTime());
	CParticleSystem::InitializeSystem();
	Emit(numParticles);
} 

// 设置初始粒子的属性
void CExplosion::InitializeParticle(int index)
{
	// 在发射区域的随机位置开始粒子运动
	m_particleList[index].m_pos.x = m_origin.x + FRAND * m_spread;  //FRAND为-1到1之间的随机小数
	m_particleList[index].m_pos.y = m_origin.y + FRAND * m_spread;
	m_particleList[index].m_pos.z = m_origin.z + FRAND * m_spread;
	// 设置粒子的大小
	m_particleList[index].m_size = PARTICLE_SIZE + FRAND * SIZE_VARIATION;
	// 给定粒子一个随机速度
	m_particleList[index].m_velocity.x = PARTICLE_VELOCITY.x + FRAND * VELOCITY_VARIATION.x;
	m_particleList[index].m_velocity.y = PARTICLE_VELOCITY.y + FRAND * VELOCITY_VARIATION.y;
	m_particleList[index].m_velocity.z = PARTICLE_VELOCITY.z + FRAND * VELOCITY_VARIATION.z;
	m_particleList[index].m_acceleration = PARTICLE_ACCELERATION;
	m_particleList[index].m_color[0] = 1.0;
	m_particleList[index].m_color[1] = 0.5 + FRAND * 0.5;   //有点粒子黄一点，有的例子红一点
	m_particleList[index].m_color[2] = 0.0;
	m_particleList[index].m_color[3] = 1.0;
	m_particleList[index].m_energy = 1.5 + FRAND/2.0;
	m_particleList[index].m_colorDelta[0] = 0.0;
	m_particleList[index].m_colorDelta[1] = -(m_particleList[index].m_color[1]/2.0)/m_particleList[index].m_energy;  
	m_particleList[index].m_colorDelta[2] = 0.0;
	m_particleList[index].m_colorDelta[3] = -1.0/m_particleList[index].m_energy;  //以这个数递减，到能量耗尽时大约能减到0，即全透明
	m_particleList[index].m_sizeDelta = -m_particleList[index].m_size/m_particleList[index].m_energy;   //以这个数递减，到能量耗尽时大约能减到0
} 

// 更新存在的粒子，消灭死亡的粒子和创建新的粒子
void CExplosion::Update(float elapsedTime)
{
	for (int i = 0; i < m_numParticles; )
	{
		// 根据速度和时间，更新粒子的位置
		m_particleList[i].m_pos = m_particleList[i].m_pos + m_particleList[i].m_velocity * elapsedTime;
		m_particleList[i].m_velocity = m_particleList[i].m_velocity + m_particleList[i].m_acceleration * elapsedTime;
		m_particleList[i].m_energy -= elapsedTime;
		m_particleList[i].m_size += m_particleList[i].m_sizeDelta * elapsedTime;
		m_particleList[i].m_color[3] += m_particleList[i].m_colorDelta[3] * elapsedTime;
		m_particleList[i].m_color[1] += m_particleList[i].m_colorDelta[1] * elapsedTime;
		// 如果粒子能量耗尽，则该粒子死亡
		if (m_particleList[i].m_energy <= 0.0)
		{
			// 将最后的粒子运动到当前的位置，并减少数量
			--m_numParticles;
			m_particleList[i] = m_particleList[m_numParticles];   //第i个粒子死掉了，把最后面的粒子调上来填在这个位置上。这
			                    //样，一会Render()的时候就画粒子的时候只画前m_numParticles个就可以了，那就是剩下没死的粒子。
		}
		else
		{
			++i;
		}
	}
} 

// 绘制粒子
void CExplosion::Render()
{
	float viewMatrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, viewMatrix); //取得模型视图变换矩阵。模型视图变换矩阵与世界坐标系的点的坐标相乘，就把点的坐标变换成镜头坐标系的坐标了

	//               ┌m0  m1  m2  m3 ┐                 ┌ m0 ┐                       
	//模型视图矩阵(m=│m4  m5  m6  m7 │)还可以这样看，R=│ m4 │为镜头坐标系的x轴方向，
	//               │m8  m9  m10 m11│                 └ m8 ┘						
	//				 └m12 m13 m14 m15┘
	//
	//  ┌ m1 ┐                          ┌ m2 ┐
	//U=│ m5 │为镜头坐标系的y轴方向,-V=-│ m6 │为镜头坐标系的z轴方向.根据这样的规律，
	//  └ m9 ┘					      └ m10┘
	//
	//可以利用R和U向量构造一个总是与摄像机观察方向垂直的面，从而使得这个面（以及上面的纹
	//
	//理总是能够被摄像机看见。
	CVector right(viewMatrix[0], viewMatrix[4], viewMatrix[8]);  //R向量
	CVector up(viewMatrix[1], viewMatrix[5], viewMatrix[9]);     //U向量

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);   //适合爆炸发光的混合方式

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	GLfloat size;
	CVector pos;

	glBegin(GL_QUADS);
	for (int i = 0; i < m_numParticles; ++i)   //对所有粒子
	{
		//每个粒子，其实都是一个与摄像机垂直的面，并贴上个纹理
		size = m_particleList[i].m_size/2; //按约定的大小来画面，面的大小会随时间减小
		pos = m_particleList[i].m_pos;     //每个面的中心位置都不一样，面的中心位置也会随时间变化
		glColor4fv(m_particleList[i].m_color);
		glTexCoord2f(0.0, 0.0); glVertex3fv((pos + (right + up) * -size).v);
		glTexCoord2f(1.0, 0.0); glVertex3fv((pos + (right - up) * size).v);
		glTexCoord2f(1.0, 1.0); glVertex3fv((pos + (right + up) * size).v);
		glTexCoord2f(0.0, 1.0); glVertex3fv((pos + (up - right) * size).v);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
} 