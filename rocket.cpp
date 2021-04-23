#include "rocket.h"

CRocket::CRocket()
{
	velocity = CVector(0.0, 0.0, 120.0);
	acceleration = CVector(0.0, 0.0, 0.0);
	distanceTravel = 0.0;
	size = 1.0f;                                                                                                                                                                      //s
	isExplosion = false;   //火箭状态刚产生时为发射状态
	explosion = NULL;      //未爆炸，粒子系统设为NULL
	explosionTex = new CTexture; //加载粒子的纹理用的纹理对象
	Load(); //加载火箭模型
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
	glGenTextures(1, &explosionTex->texID);            //生成一个纹理对象
	glBindTexture(GL_TEXTURE_2D, explosionTex->texID); //绑定纹理对象

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);     //设置横向重复贴图
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);     //设置纵向重复贴图
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //图像在屏幕变大时采用线性插值
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);//图像在屏幕缩小时采用最近Mipmap插值
//	glTexImage2D(GL_TEXTURE_2D, 0, 4, explosionTex->width, explosionTex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, explosionTex->data);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, explosionTex->width, explosionTex->height, GL_RGBA, GL_UNSIGNED_BYTE, explosionTex->data); //绑定纹理
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
		if(explosion->m_numParticles == 0)  //粒子数变回0时，爆炸已完毕，这个火箭就没用了，
			isDead = true;                  //准备删除
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
		// 爆炸
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
	// 如果火箭没有爆炸，则绘制火箭模型
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
	// 绘制爆炸
	else
	{
		glDisable(GL_FOG);
		explosion->Render();
		glEnable(GL_FOG);                                                                                                                                                                                  //s
	}
}


void CRocket::Load()
{
	CMD2Model::Load("models\\rocketair.md2", "models\\rocket.pcx"); //加载md2模型
	explosionTex->LoadTexture("explosion.bmp");    //加载爆炸粒子贴图文件
	SetupExplosionTexture();   //设置爆炸纹理的属性
}

void CRocket::Unload()
{
}

void CRocket::OnPrepare()
{
	// 进行该实体与其他对象之间的碰撞检测操作
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
