#include "rocket.h"

CRocket::CRocket()
{
	velocity = CVector(0.0, 0.0, 120.0);
	acceleration = CVector(0.0, 0.0, 0.0);
	distanceTravel = 0.0;
	size = 1.0f;                                                                                                                                                                      //s
	isExplosion = false;   //���״̬�ղ���ʱΪ����״̬
	explosion = NULL;      //δ��ը������ϵͳ��ΪNULL
	explosionTex = new CTexture; //�������ӵ������õ��������
	Load(); //���ػ��ģ��
}

CRocket::~CRocket()
{
	if (explosion != NULL)
	{
		explosion->KillSystem();
		delete [] explosion;
		explosion = NULL;
	}
	if (explosionTex != NULL)
	{
		delete explosionTex;
		explosionTex = NULL;
	}
}

void CRocket::SetupExplosionTexture()
{
	glGenTextures(1, &explosionTex->texID);            //����һ���������
	glBindTexture(GL_TEXTURE_2D, explosionTex->texID); //���������

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);     //���ú����ظ���ͼ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);     //���������ظ���ͼ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //ͼ������Ļ���ʱ�������Բ�ֵ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);//ͼ������Ļ��Сʱ�������Mipmap��ֵ
//	glTexImage2D(GL_TEXTURE_2D, 0, 4, explosionTex->width, explosionTex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, explosionTex->data);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, explosionTex->width, explosionTex->height, GL_RGBA, GL_UNSIGNED_BYTE, explosionTex->data); //������
}

void CRocket::OnAnimate(scalar_t deltaTime)
{
	float cosYaw = (float)cos(DEG2RAD(direction));
	float sinYaw = (float)sin(DEG2RAD(direction)); 
	float sinPitch = (float)sin(DEG2RAD(pitch));

	float speed = velocity.z * deltaTime;

	position.x += float(cosYaw)*speed;
	position.y += float(sinPitch)*speed;
	position.z += float(sinYaw)*speed;

	distanceTravel += position.Length();

	if (isExplosion)
	{
		explosion->Update(deltaTime);
		if(explosion->m_numParticles == 0)  //���������0ʱ����ը����ϣ���������û���ˣ�
			isDead = true;                  //׼��ɾ��
	}

	if (!isExplosion)
	{
		if (distanceTravel >= 500000.0f)
		{
			isExplosion = true;
			velocity = CVector(0.0, 0.0, 0.0);
			explosion = new CExplosion(10, position, 0.1, explosionTex->texID);
		}
	}
}

void CRocket::OnCollision(CObject *collisionObject)
{
	if (!isExplosion)
	{
		if (typeid(*collisionObject) == typeid(CTerrain))
		{
			if (((CTerrain*)collisionObject)->GetHeight(position.x, position.z) + size >= position.y)
			{
				isExplosion = true;
				velocity = CVector(0.0, 0.0, 0.0);
				explosion = new CExplosion(500, position, 8.0, explosionTex->texID);
				PlaySound();
			}
		// ��ը
		}
		if (typeid(*collisionObject) == typeid(COgroEnemy))
		{
			isExplosion = true;
			velocity = CVector(0.0, 0.0, 0.0);
			explosion = new CExplosion(500, position, 8.0, explosionTex->texID);
			PlaySound();
		}
		if (typeid(*collisionObject) == typeid(CSodEnemy))
		{
			isExplosion = true;
			velocity = CVector(0.0, 0.0, 0.0);
			explosion = new CExplosion(500, position, 8.0, explosionTex->texID);
			PlaySound();
		}
	}
}

void CRocket::OnDraw(CCamera *camera)
{
	// ������û�б�ը������ƻ��ģ��
	if (!isExplosion)
	{
		glEnable(GL_TEXTURE_2D);
		glColor3f(1.0, 1.0, 1.0);
		glTranslatef(position.x, position.y, position.z);
		glRotatef(-direction, 0.0, 1.0, 0.0);
		glScalef(0.025f, 0.025f, 0.025f);
		RenderFrame(0);
		glDisable(GL_TEXTURE_2D);
	}
	// ���Ʊ�ը
	else
	{
		glDisable(GL_FOG);
		explosion->Render();
		glEnable(GL_FOG);                                                                                                                                                                                  //s
	}
}


void CRocket::Load()
{
	CMD2Model::Load("models\\rocketair.md2", "models\\rocket.pcx"); //����md2ģ��
	explosionTex->LoadTexture("explosion.bmp");    //���ر�ը������ͼ�ļ�
	SetupExplosionTexture();   //���ñ�ը���������
}

void CRocket::Unload()
{
}

void CRocket::OnPrepare()
{
	// ���и�ʵ������������֮�����ײ������
	if (!isExplosion)
		ProcessCollisions(FindRoot());
	if (isExplosion)
	{
		if (explosion->IsDead() && !audioSys->GetPerformance()->IsPlaying(entitySound->GetSegment(), NULL))
		{
			explosion->KillSystem();
			delete explosion;
			explosion = NULL;
			isExplosion = false;
			isDead = true;
		}
	}
}
