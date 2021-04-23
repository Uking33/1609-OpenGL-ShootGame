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

	int mouseDeltaX, mouseDeltaY;	// changes in the mouse position（鼠标位置）

	// update devices（输入设备更新数据）
	inputSystem->Update();

	// retrieve the latest mouse movements(获取最近的鼠标运动位移）
	inputSystem->GetMouseMovement(mouseDeltaX, mouseDeltaY);
	OnMouseMove(mouseDeltaX, mouseDeltaY);

	if (inputSystem->KeyDown(DIK_W))  //检查键盘是否按下了W键
		OnKeyDown(VK_UP);			  
	if (inputSystem->KeyDown(DIK_S))  //检查键盘是否按下了S键
		OnKeyDown(VK_DOWN);			  
	if (inputSystem->KeyDown(DIK_A))  //检查键盘是否按下了A键
		OnKeyDown(VK_LEFT);			  
	if (inputSystem->KeyDown(DIK_D))  //检查键盘是否按下了D键
		OnKeyDown(VK_RIGHT);
	if (inputSystem->KeyDown(DIK_ADD))      //检查键盘是否按下了+键(小键盘）
		OnKeyDown(VK_ADD);					
	if (inputSystem->KeyDown(DIK_SUBTRACT))	//检查键盘是否按下了-键(小键盘）
		OnKeyDown(VK_SUBTRACT);				
	if (inputSystem->KeyDown(DIK_ESCAPE))	//检查键盘是否按下了空格键
		OnKeyDown(VK_ESCAPE);

	// decrease amount of time until next possible recognized button pressing（减少时间直到下一个鼠标按键按下）
	buttonDelta -= deltaTime;   //(2)
	if (buttonDelta < 0.0f)	    //(3)
		buttonDelta = 0.0f;	   
	if (inputSystem->ButtonDown(0)) //检查鼠标左键是否被按下
	{
		if (buttonDelta == 0.0f)//(4)
		{						
			//讲解：发射火箭

			OnMouseDownL(0,0);	//(1)(2)(3)(4)句配合，可使得每0.5秒来一次这里，即每隔0.5秒响应一次鼠标左键。
			buttonDelta = 0.5f;	//(1)

			//debug---
			//在输出窗口提示一下地形下目前有多少个物体，分别是什么物体
			CWorld *pWorld = OnGetWorld();		// get the world（获取场景）
			pWorld->ReportObjs();
			//end debug---
		}
	}
}

void CEngine::GameCycle(float deltaTime)
{
	CCamera *camera = OnGetCamera();	// get the camera（获取摄像机）
	CWorld *world = OnGetWorld();		// get the world（获取场景）

	if (useDInput)
		CheckInput(deltaTime);

	// setup opengl for frame (clear, identity)（为每帧进行opengl设置）
	OnPrepare();

	// prepare objects and perform collisions（三维世界里所有对象做一些准备工作,包括角色动画状态的设置,所有物体的碰撞检测）
	world->Prepare();              //in

	// move/orient camera（移动/定向摄像机）
	camera->Animate(deltaTime);

	// move/orient objects（移动/定向对象）
	world->Animate(deltaTime);

	// draw objects（绘制对象）
	world->Draw(camera);

	// swap buffers（交换缓冲区）
	SwapBuffers(hDC);
}

// EnterMessageLoop()
// desc: the Windows message loop（Windows消息循环）
LRESULT CEngine::EnterMessageLoop()
{
	// Message loop（消息循环）
	MSG msg;
	timer = new CHiResTimer;

	timer->Init();

	for (;;)
	{	
		GameCycle(timer->GetElapsedSeconds(1));
		SetWindowText(hWnd, "恐怖之战");
		//消息循环
		while (PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE))  //检查有没有新消息
		{
			// we always update if there are any events, even if we're paused
			if (!GetMessage (&msg, NULL, 0, 0))   //取出新消息
			{
				delete timer;
				return msg.wParam;
			}

			TranslateMessage (&msg);  //翻译消息
    		DispatchMessage (&msg);   //分发消息
		}
	}

	delete timer;

	return msg.wParam;
}