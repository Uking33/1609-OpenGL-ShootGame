#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include <stdlib.h>

#include "engine.h"
#include "HiResTimer.h"
#include "camera.h"
#include "world.h"

void CEngine::CheckInput(float deltaTime)
{
	static float buttonDelta = 0.0f;

	int mouseDeltaX, mouseDeltaY;	// changes in the mouse position�����λ�ã�

	// update devices�������豸�������ݣ�
	inputSystem->Update();

	// retrieve the latest mouse movements(��ȡ���������˶�λ�ƣ�
	inputSystem->GetMouseMovement(mouseDeltaX, mouseDeltaY);
	OnMouseMove(mouseDeltaX, mouseDeltaY);

	if (inputSystem->KeyDown(DIK_W))  //�������Ƿ�����W��
		OnKeyDown(VK_UP);			  
	if (inputSystem->KeyDown(DIK_S))  //�������Ƿ�����S��
		OnKeyDown(VK_DOWN);			  
	if (inputSystem->KeyDown(DIK_A))  //�������Ƿ�����A��
		OnKeyDown(VK_LEFT);			  
	if (inputSystem->KeyDown(DIK_D))  //�������Ƿ�����D��
		OnKeyDown(VK_RIGHT);
	if (inputSystem->KeyDown(DIK_ADD))      //�������Ƿ�����+��(С���̣�
		OnKeyDown(VK_ADD);					
	if (inputSystem->KeyDown(DIK_SUBTRACT))	//�������Ƿ�����-��(С���̣�
		OnKeyDown(VK_SUBTRACT);				
	if (inputSystem->KeyDown(DIK_ESCAPE))	//�������Ƿ����˿ո��
		OnKeyDown(VK_ESCAPE);

	// decrease amount of time until next possible recognized button pressing������ʱ��ֱ����һ����갴�����£�
	buttonDelta -= deltaTime;   //(2)
	if (buttonDelta < 0.0f)	    //(3)
		buttonDelta = 0.0f;	   
	if (inputSystem->ButtonDown(0)) //����������Ƿ񱻰���
	{
		if (buttonDelta == 0.0f)//(4)
		{						
			//���⣺������

			OnMouseDownL(0,0);	//(1)(2)(3)(4)����ϣ���ʹ��ÿ0.5����һ�������ÿ��0.5����Ӧһ����������
			buttonDelta = 0.5f;	//(1)

			//debug---
			//�����������ʾһ�µ�����Ŀǰ�ж��ٸ����壬�ֱ���ʲô����
			CWorld *pWorld = OnGetWorld();		// get the world����ȡ������
			pWorld->ReportObjs();
			//end debug---
		}
	}
}

void CEngine::GameCycle(float deltaTime)
{
	CCamera *camera = OnGetCamera();	// get the camera����ȡ�������
	CWorld *world = OnGetWorld();		// get the world����ȡ������

	if (useDInput)
		CheckInput(deltaTime);

	// setup opengl for frame (clear, identity)��Ϊÿ֡����opengl���ã�
	OnPrepare();

	// prepare objects and perform collisions����ά���������ж�����һЩ׼������,������ɫ����״̬������,�����������ײ��⣩
	world->Prepare();              //in

	// move/orient camera���ƶ�/�����������
	camera->Animate(deltaTime);

	// move/orient objects���ƶ�/�������
	world->Animate(deltaTime);

	// draw objects�����ƶ���
	world->Draw(camera);

	// swap buffers��������������
	SwapBuffers(hDC);
}

// EnterMessageLoop()
// desc: the Windows message loop��Windows��Ϣѭ����
LRESULT CEngine::EnterMessageLoop()
{
	// Message loop����Ϣѭ����
	MSG msg;
	timer = new CHiResTimer;

	timer->Init();

	for (;;)
	{	
		GameCycle(timer->GetElapsedSeconds(1));
		SetWindowText(hWnd, "�ֲ�֮ս");
		//��Ϣѭ��
		while (PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE))  //�����û������Ϣ
		{
			// we always update if there are any events, even if we're paused
			if (!GetMessage (&msg, NULL, 0, 0))   //ȡ������Ϣ
			{
				delete timer;
				return msg.wParam;
			}

			TranslateMessage (&msg);  //������Ϣ
    		DispatchMessage (&msg);   //�ַ���Ϣ
		}
	}

	delete timer;

	return msg.wParam;
}