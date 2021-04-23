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
	audioSystem = new CAudioSystem;     //创建音频系统
	audioSystem->InitDirectXAudio(NULL);//初始化音频系统
	worldSound = audioSystem->Create("AMBIENT.WAV", false); //加载音频文件
	audioSystem->Play(worldSound, DMUS_SEG_REPEAT_INFINITE);//播放声音

	camera = c;                         //三维世界中的摄像机赋值
	terrain = new CTerrain(32, 1.0f);	//生成三维世界中的地形
	player = new CPlayer;               //创建Player

	//讲解：OpenGL显示文字信息。见ppt X.3.2.2

	gui = new CGUI;
	gameDone = false;		            //游戏开始
	player->AttachTo(terrain);          //附着与地形上，地形为父节点
	player->SetCamera(camera);
	player->SetTerrain(terrain);
	player->SetAudioSystem(audioSystem);

	LoadWorld();
	timeStart = 600.0;	 //开始的剩余时间。即游戏规定这么时间内必须玩完	
	timeElapsed = 0.0;   //已花费的时间
	gui->SetCurrentTime(timeStart);
	gui->SetEnemiesLeft(numOgros + numSods);
}

void CWorld::Animate(float deltaTime)
{
	// 根据地形设置摄像机的高度位置
	camera->position.y = terrain->GetHeight(camera->position.x, camera->position.z) + player->size;                                                                                                                                           //s
	//不能走出界，也不要离边界太近
	if (camera->position.x <= terrain->GetScanDepth())
		camera->position.x = terrain->GetScanDepth();
	if (camera->position.x >= terrain->GetWidth()*terrain->GetMul() - terrain->GetScanDepth())
		camera->position.x = terrain->GetWidth()*terrain->GetMul() - terrain->GetScanDepth();
	if (camera->position.z <= terrain->GetScanDepth())
		camera->position.z = terrain->GetScanDepth();
	if (camera->position.z >= terrain->GetWidth()*terrain->GetMul() - terrain->GetScanDepth())
		camera->position.z = terrain->GetWidth()*terrain->GetMul() - terrain->GetScanDepth();

	terrain->Animate(deltaTime);                 // 动画地形

	const type_info &ogro = typeid(COgroEnemy);  // 获取ogro类型id
	const type_info &sod = typeid(CSodEnemy);    // 获取sod类型id

	numOgros = CountObjectTypes(ogro);           // ogro计数
	numSods = CountObjectTypes(sod);             // sod计数

	gui->SetEnemiesLeft(numOgros + numSods);
	gui->SetCurrentTime(timeStart - timeElapsed);

	if (!gameDone)
		timeElapsed += deltaTime;
//	else
//		timeElapsed = timeStart;  //注释掉这里win了后时间不会变成0，从而知道时间记录
}

void CWorld::FadeScreen()
{
	glPushMatrix();
	glLoadIdentity();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_TEXTURE_2D);
	//画一个半透明的平面在摄像机面前，就实现了屏幕的“褪色”
	glColor4f(0.0, 0.0, 0.0, 0.7); //半透明
	glBegin(GL_QUADS);             //画一个四边形（在摄像机面前）
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
	terrain->Prepare();   //指针调用，不同类的对象将调用各自的虚函数

	if ((numOgros + numSods <= 0) || (timeElapsed >= timeStart))
		gameDone = true;
}

// 初始化场景
void CWorld::LoadWorld()
{
	int enemyIdx = 0;  //循环变量
	int rndInt = 0;    //随机值

	numOgros = 0;
	numSods = 0;

	srand((unsigned int)time(NULL));   //为随机函数rand()撒个随机的种子
	
	rndInt = (rand() % (MAX_ENEMIES-2)) + 2; //产生随机数，随机数在2到5之间
	numOgros = numSods = rndInt;       //Ogro敌人和Sod敌人数目都等于此数
	// 生成ogros敌人
	for (enemyIdx = 0; enemyIdx < numOgros; enemyIdx++)
	{
		ogroEnemy = new COgroEnemy;
		ogroEnemy->AttachTo(terrain);
		ogroEnemy->SetPlayer(player);
		ogroEnemy->SetAudioSystem(audioSystem);
		//随机设置敌人的位置
		ogroEnemy->position.x = (float)(rand() % (int)(terrain->GetWidth() * terrain->GetMul()));
		ogroEnemy->position.y = 0.0f;
		ogroEnemy->position.z = (float)(rand() % (int)(terrain->GetWidth() * terrain->GetMul()));
	}
	// 生成sods敌人
	for (enemyIdx = 0; enemyIdx < numSods; enemyIdx++)
	{
		sodEnemy = new CSodEnemy;
		sodEnemy->AttachTo(terrain);
		sodEnemy->SetPlayer(player);
		sodEnemy->SetAudioSystem(audioSystem);
		//随机设置敌人的位置
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
		} while (!c2->IsFirstChild()); //最末个与首个也相连接，串成一个环，所以查一圈后会再回到FirstChild，从而退出内循环

		//count--;  //bug

		c1 = (CObject*)c1->childNode;   //走到c1没有子对象时外循环就退出了
	}

	return count;
}

//debug---
//在输出窗口中提示地形下有多少个子节点，分别是什么物体
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