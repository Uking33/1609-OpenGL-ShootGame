#include "explosion.h"


CExplosion::CExplosion(int numParticles, CVector origin, float spread, GLuint texture)
  : m_texture(texture), m_spread(spread), CParticleSystem(numParticles, origin)
{
	srand(timeGetTime());
	CParticleSystem::InitializeSystem();
	Emit(numParticles);
} 

// ���ó�ʼ���ӵ�����
void CExplosion::InitializeParticle(int index)
{
	// �ڷ�����������λ�ÿ�ʼ�����˶�
	m_particleList[index].m_pos.x = m_origin.x + FRAND * m_spread;  //FRANDΪ-1��1֮������С��
	m_particleList[index].m_pos.y = m_origin.y + FRAND * m_spread;
	m_particleList[index].m_pos.z = m_origin.z + FRAND * m_spread;
	// �������ӵĴ�С
	m_particleList[index].m_size = PARTICLE_SIZE + FRAND * SIZE_VARIATION;
	// ��������һ������ٶ�
	m_particleList[index].m_velocity.x = PARTICLE_VELOCITY.x + FRAND * VELOCITY_VARIATION.x;
	m_particleList[index].m_velocity.y = PARTICLE_VELOCITY.y + FRAND * VELOCITY_VARIATION.y;
	m_particleList[index].m_velocity.z = PARTICLE_VELOCITY.z + FRAND * VELOCITY_VARIATION.z;
	m_particleList[index].m_acceleration = PARTICLE_ACCELERATION;
	m_particleList[index].m_color[0] = 1.0;
	m_particleList[index].m_color[1] = 0.5 + FRAND * 0.5;   //�е����ӻ�һ�㣬�е����Ӻ�һ��
	m_particleList[index].m_color[2] = 0.0;
	m_particleList[index].m_color[3] = 1.0;
	m_particleList[index].m_energy = 1.5 + FRAND/2.0;
	m_particleList[index].m_colorDelta[0] = 0.0;
	m_particleList[index].m_colorDelta[1] = -(m_particleList[index].m_color[1]/2.0)/m_particleList[index].m_energy;  
	m_particleList[index].m_colorDelta[2] = 0.0;
	m_particleList[index].m_colorDelta[3] = -1.0/m_particleList[index].m_energy;  //��������ݼ����������ľ�ʱ��Լ�ܼ���0����ȫ͸��
	m_particleList[index].m_sizeDelta = -m_particleList[index].m_size/m_particleList[index].m_energy;   //��������ݼ����������ľ�ʱ��Լ�ܼ���0
} 

// ���´��ڵ����ӣ��������������Ӻʹ����µ�����
void CExplosion::Update(float elapsedTime)
{
	for (int i = 0; i < m_numParticles; )
	{
		// �����ٶȺ�ʱ�䣬�������ӵ�λ��
		m_particleList[i].m_pos = m_particleList[i].m_pos + m_particleList[i].m_velocity * elapsedTime;
		m_particleList[i].m_velocity = m_particleList[i].m_velocity + m_particleList[i].m_acceleration * elapsedTime;
		m_particleList[i].m_energy -= elapsedTime;
		m_particleList[i].m_size += m_particleList[i].m_sizeDelta * elapsedTime;
		m_particleList[i].m_color[3] += m_particleList[i].m_colorDelta[3] * elapsedTime;
		m_particleList[i].m_color[1] += m_particleList[i].m_colorDelta[1] * elapsedTime;
		// ������������ľ��������������
		if (m_particleList[i].m_energy <= 0.0)
		{
			// �����������˶�����ǰ��λ�ã�����������
			--m_numParticles;
			m_particleList[i] = m_particleList[m_numParticles];   //��i�����������ˣ������������ӵ������������λ���ϡ���
			                    //����һ��Render()��ʱ��ͻ����ӵ�ʱ��ֻ��ǰm_numParticles���Ϳ����ˣ��Ǿ���ʣ��û�������ӡ�
		}
		else
		{
			++i;
		}
	}
} 

// ��������
void CExplosion::Render()
{
	float viewMatrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, viewMatrix); //ȡ��ģ����ͼ�任����ģ����ͼ�任��������������ϵ�ĵ��������ˣ��Ͱѵ������任�ɾ�ͷ����ϵ��������

	//               ��m0  m1  m2  m3 ��                 �� m0 ��                       
	//ģ����ͼ����(m=��m4  m5  m6  m7 ��)��������������R=�� m4 ��Ϊ��ͷ����ϵ��x�᷽��
	//               ��m8  m9  m10 m11��                 �� m8 ��						
	//				 ��m12 m13 m14 m15��
	//
	//  �� m1 ��                          �� m2 ��
	//U=�� m5 ��Ϊ��ͷ����ϵ��y�᷽��,-V=-�� m6 ��Ϊ��ͷ����ϵ��z�᷽��.���������Ĺ��ɣ�
	//  �� m9 ��					      �� m10��
	//
	//��������R��U��������һ��������������۲췽��ֱ���棬�Ӷ�ʹ������棨�Լ��������
	//
	//�������ܹ��������������
	CVector right(viewMatrix[0], viewMatrix[4], viewMatrix[8]);  //R����
	CVector up(viewMatrix[1], viewMatrix[5], viewMatrix[9]);     //U����

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);   //�ʺϱ�ը����Ļ�Ϸ�ʽ

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	GLfloat size;
	CVector pos;

	glBegin(GL_QUADS);
	for (int i = 0; i < m_numParticles; ++i)   //����������
	{
		//ÿ�����ӣ���ʵ����һ�����������ֱ���棬�����ϸ�����
		size = m_particleList[i].m_size/2; //��Լ���Ĵ�С�����棬��Ĵ�С����ʱ���С
		pos = m_particleList[i].m_pos;     //ÿ���������λ�ö���һ�����������λ��Ҳ����ʱ��仯
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