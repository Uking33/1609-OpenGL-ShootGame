#include "world.h"

CWorld::CWorld()
{
	terrain = new CTerrain(32, 0.5);
}

CWorld::~CWorld()
{
	audioSystem->Shutdown();

	delete gui;
	delete terrain;
	delete audioSystem;
	delete worldSound;

	gui = NULL;
	terrain = NULL;
	audioSystem = NULL;
	worldSound = NULL;
}

CWorld::CWorld(CCamera *c)
{
	audioSystem = new CAudioSystem;     //������Ƶϵͳ
	audioSystem->InitDirectXAudio(NULL);//��ʼ����Ƶϵͳ
	worldSound = audioSystem->Create("AMBIENT.WAV", false); //������Ƶ�ļ�
	audioSystem->Play(worldSound, DMUS_SEG_REPEAT_INFINITE);//��������

	camera = c;                         //��ά�����е��������ֵ
	terrain = new CTerrain(32, 1.0f);	//������ά�����еĵ���
	player = new CPlayer;               //����Player

	//���⣺OpenGL��ʾ������Ϣ����ppt X.3.2.2

	gui = new CGUI;
	gameDone = false;		            //��Ϸ��ʼ
	player->AttachTo(terrain);          //����������ϣ�����Ϊ���ڵ�
	player->SetCamera(camera);
	player->SetTerrain(terrain);
	player->SetAudioSystem(audioSystem);

	LoadWorld();
	timeStart = 600.0;	 //��ʼ��ʣ��ʱ�䡣����Ϸ�涨��ôʱ���ڱ�������	
	timeElapsed = 0.0;   //�ѻ��ѵ�ʱ��
	gui->SetCurrentTime(timeStart);
	gui->SetEnemiesLeft(numOgros + numSods);
}

void CWorld::Animate(float deltaTime)
{
	// ���ݵ�������������ĸ߶�λ��
	camera->position.y = terrain->GetHeight(camera->position.x, camera->position.z) + player->size;                                                                                                                                           //s
	//�����߳��磬Ҳ��Ҫ��߽�̫��
	if (camera->position.x <= terrain->GetScanDepth())
		camera->position.x = terrain->GetScanDepth();
	if (camera->position.x >= terrain->GetWidth()*terrain->GetMul() - terrain->GetScanDepth())
		camera->position.x = terrain->GetWidth()*terrain->GetMul() - terrain->GetScanDepth();
	if (camera->position.z <= terrain->GetScanDepth())
		camera->position.z = terrain->GetScanDepth();
	if (camera->position.z >= terrain->GetWidth()*terrain->GetMul() - terrain->GetScanDepth())
		camera->position.z = terrain->GetWidth()*terrain->GetMul() - terrain->GetScanDepth();

	terrain->Animate(deltaTime);                 // ��������

	const type_info &ogro = typeid(COgroEnemy);  // ��ȡogro����id
	const type_info &sod = typeid(CSodEnemy);    // ��ȡsod����id

	numOgros = CountObjectTypes(ogro);           // ogro����
	numSods = CountObjectTypes(sod);             // sod����

	gui->SetEnemiesLeft(numOgros + numSods);
	gui->SetCurrentTime(timeStart - timeElapsed);

	if (!gameDone)
		timeElapsed += deltaTime;
//	else
//		timeElapsed = timeStart;  //ע�͵�����win�˺�ʱ�䲻����0���Ӷ�֪��ʱ���¼
}

void CWorld::FadeScreen()
{
	glPushMatrix();
	glLoadIdentity();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_TEXTURE_2D);
	//��һ����͸����ƽ�����������ǰ����ʵ������Ļ�ġ���ɫ��
	glColor4f(0.0, 0.0, 0.0, 0.7); //��͸��
	glBegin(GL_QUADS);             //��һ���ı��Σ����������ǰ��
	glVertex3f(-5, -5, -3.0);
	glVertex3f(5, -5, -3.0);
	glVertex3f(5, 5, -3.0);
	glVertex3f(-5, 5, -3.0);
	glEnd();
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glPopMatrix();
}

void CWorld::SetScreen(int width, int height)
{
	screenWidth = width;
	screenHeight = height;
}

void CWorld::Draw(CCamera *camera)
{
	terrain->Draw(camera);
	gui->Draw();

	if (gameDone)
	{
		FadeScreen();
		if (numOgros + numSods <=0)
			gui->DrawWinner();
		else
			gui->DrawLoser();
	}
}

void CWorld::OnPrepare()
{
	glClearColor(terrain->fogColor[0], terrain->fogColor[1], terrain->fogColor[2], terrain->fogColor[3]);
	terrain->Prepare();   //ָ����ã���ͬ��Ķ��󽫵��ø��Ե��麯��

	if ((numOgros + numSods <= 0) || (timeElapsed >= timeStart))
		gameDone = true;
}

// ��ʼ������
void CWorld::LoadWorld()
{
	int enemyIdx = 0;  //ѭ������
	int rndInt = 0;    //���ֵ

	numOgros = 0;
	numSods = 0;

	srand((unsigned int)time(NULL));   //Ϊ�������rand()�������������
	
	rndInt = (rand() % (MAX_ENEMIES-2)) + 2; //������������������2��5֮��
	numOgros = numSods = rndInt;       //Ogro���˺�Sod������Ŀ�����ڴ���
	// ����ogros����
	for (enemyIdx = 0; enemyIdx < numOgros; enemyIdx++)
	{
		ogroEnemy = new COgroEnemy;
		ogroEnemy->AttachTo(terrain);
		ogroEnemy->SetPlayer(player);
		ogroEnemy->SetAudioSystem(audioSystem);
		//������õ��˵�λ��
		ogroEnemy->position.x = (float)(rand() % (int)(terrain->GetWidth() * terrain->GetMul()));
		ogroEnemy->position.y = 0.0f;
		ogroEnemy->position.z = (float)(rand() % (int)(terrain->GetWidth() * terrain->GetMul()));
	}
	// ����sods����
	for (enemyIdx = 0; enemyIdx < numSods; enemyIdx++)
	{
		sodEnemy = new CSodEnemy;
		sodEnemy->AttachTo(terrain);
		sodEnemy->SetPlayer(player);
		sodEnemy->SetAudioSystem(audioSystem);
		//������õ��˵�λ��
		sodEnemy->position.x = (float)(rand() % (int)(terrain->GetWidth() * terrain->GetMul()));
		sodEnemy->position.y = 0.0f;
		sodEnemy->position.z = (float)(rand() % (int)(terrain->GetWidth() * terrain->GetMul()));
	}
}

int CWorld::CountObjectTypes(const type_info &classID)
{
	int count = 0;
	CObject *c1;
	CObject *c2;

	c1 = (CObject*)terrain->childNode;

	while (c1 != NULL)
	{
		c2 = c1;
		do
		{
			if (typeid(*c2) == classID)
				count++;
			c2 = (CObject*)c2->nextNode;
		} while (!c2->IsFirstChild()); //��ĩ�����׸�Ҳ�����ӣ�����һ���������Բ�һȦ����ٻص�FirstChild���Ӷ��˳���ѭ��

		//count--;  //bug

		c1 = (CObject*)c1->childNode;   //�ߵ�c1û���Ӷ���ʱ��ѭ�����˳���
	}

	return count;
}

//debug---
//�������������ʾ�������ж��ٸ��ӽڵ㣬�ֱ���ʲô����
void CWorld::ReportObjs()
{
	CNode* pNextNode;
	int nObjNum = 0;
	char sMsg[256];
	OutputDebugString("Objects under terrain:");
	if(terrain->HasChild())
	{
		pNextNode = terrain->childNode;
		while(!pNextNode->IsLastChild())
		{
			nObjNum ++;
			if(typeid(*pNextNode) == typeid(CTerrain))
				OutputDebugString("T");
			else if(typeid(*pNextNode) == typeid(CSodEnemy))
				OutputDebugString("S");
			else if(typeid(*pNextNode) == typeid(COgroEnemy))
				OutputDebugString("O");
			else if(typeid(*pNextNode) == typeid(CPlayer))
				OutputDebugString("P");
			else if(typeid(*pNextNode) == typeid(CRocket))
				OutputDebugString("R");
			else
				OutputDebugString("U");    //unknown
			pNextNode = pNextNode->nextNode;
		}
		sprintf(sMsg,"--%d objects.\n",nObjNum);
		//Trace(_T("--%d objects.\n"),nObjNum);
		OutputDebugString(sMsg);
	}
}
//end debug---