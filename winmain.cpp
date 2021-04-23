#define WIN32_MEAN_AND_LEAN
#define WIN32_EXTRA_LEAN


#include <windows.h>

#include "oglwindow.h"		// the OpenGL window class
#include "vector.h"
#include "engine.h"			// the engine's main class
#include "simpengine.h"


int WINAPI WinMain(HINSTANCE hInst,          //WinMain()的四个参数含义：hInst--本进程句柄
				   HINSTANCE hPrevInstance,  //                         hPrevInstance--前一个应用程序的句柄，一般都是NULL
				   LPSTR lpCmdLine,          //                         lpCmdLine--如果程序是命令行启动，并带有参数，lpCmdLine就记录了参数的字串
				   int nCmdShow)             //                         nCmdShow--程序是否需要显示界面   
{
	int loopRet;     //返回值
	bool fscreen;    //是否全屏
	char str[700];   //弹出消息字符串

	CoInitialize(NULL);  //程序用了DirectX,要使用DirectX，程序启动时要先初始化Com环境

	if (!COGLWindow::RegisterWindow(hInst))  //注册窗口类
	{
		MessageBox(NULL, "Failed to register window class", "Error", MB_OK);
		return -1;
	}

	CSimpEngine *engine = NULL;

	sprintf(str, "欢迎进入恐怖之战!\n\n键盘:\nW - 向前\nS - 向后\nA - 向左扫射\nD - 向右扫射");
	strcat(str, "\n\n按下鼠标左键发射火箭.\n移动鼠标左右查看\n\n");
	strcat(str, "数字小键盘'+' - 增加鼠标灵敏度\n数字小键盘'-' - 减小鼠标灵敏度\n\n");
		
	MessageBox(NULL, str, "欢迎!", MB_OK);

	sprintf(str, "恐怖之战\n\n\n您将只有10分钟杀死所有的敌人,充分使用您的侦察手段\n");
	strcat(str, "和您的武器,可以做到吗?");
	MessageBox(NULL, str, "欢迎!", MB_OK);

	try
	{	
		if (MessageBox(NULL, "显示模式将设置为800x600x16色, 希望在全屏模式下运行吗?", "全屏显示?", MB_YESNO) == IDYES)
			fscreen = true;
		else
			fscreen = false;

		engine = new CSimpEngine("OpenGL 游戏", fscreen, 800, 600, 16);  //创建游戏引擎实例
		loopRet = engine->EnterMessageLoop();   //在游戏消息循环中不断循环，直到按esc键返回
		delete engine;
		return loopRet;
	}
	catch(char *sz)
	{	
		MessageBox(NULL, sz, 0, 0);
		delete engine;
	}

	CoUninitialize();    //前面调用了CoInitialize(），程序结束时要关闭Com环境

	return -1;
}