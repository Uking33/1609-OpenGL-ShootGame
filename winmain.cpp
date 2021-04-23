#define WIN32_MEAN_AND_LEAN
#define WIN32_EXTRA_LEAN


#include <windows.h>

#include "oglwindow.h"		// the OpenGL window class
#include "vector.h"
#include "engine.h"			// the engine's main class
#include "simpengine.h"


int WINAPI WinMain(HINSTANCE hInst,          //WinMain()���ĸ��������壺hInst--�����̾��
				   HINSTANCE hPrevInstance,  //                         hPrevInstance--ǰһ��Ӧ�ó���ľ����һ�㶼��NULL
				   LPSTR lpCmdLine,          //                         lpCmdLine--��������������������������в�����lpCmdLine�ͼ�¼�˲������ִ�
				   int nCmdShow)             //                         nCmdShow--�����Ƿ���Ҫ��ʾ����   
{
	int loopRet;     //����ֵ
	bool fscreen;    //�Ƿ�ȫ��
	char str[700];   //������Ϣ�ַ���

	CoInitialize(NULL);  //��������DirectX,Ҫʹ��DirectX����������ʱҪ�ȳ�ʼ��Com����

	if (!COGLWindow::RegisterWindow(hInst))  //ע�ᴰ����
	{
		MessageBox(NULL, "Failed to register window class", "Error", MB_OK);
		return -1;
	}

	CSimpEngine *engine = NULL;

	sprintf(str, "��ӭ����ֲ�֮ս!\n\n����:\nW - ��ǰ\nS - ���\nA - ����ɨ��\nD - ����ɨ��");
	strcat(str, "\n\n����������������.\n�ƶ�������Ҳ鿴\n\n");
	strcat(str, "����С����'+' - �������������\n����С����'-' - ��С���������\n\n");
		
	MessageBox(NULL, str, "��ӭ!", MB_OK);

	sprintf(str, "�ֲ�֮ս\n\n\n����ֻ��10����ɱ�����еĵ���,���ʹ����������ֶ�\n");
	strcat(str, "����������,����������?");
	MessageBox(NULL, str, "��ӭ!", MB_OK);

	try
	{	
		if (MessageBox(NULL, "��ʾģʽ������Ϊ800x600x16ɫ, ϣ����ȫ��ģʽ��������?", "ȫ����ʾ?", MB_YESNO) == IDYES)
			fscreen = true;
		else
			fscreen = false;

		engine = new CSimpEngine("OpenGL ��Ϸ", fscreen, 800, 600, 16);  //������Ϸ����ʵ��
		loopRet = engine->EnterMessageLoop();   //����Ϸ��Ϣѭ���в���ѭ����ֱ����esc������
		delete engine;
		return loopRet;
	}
	catch(char *sz)
	{	
		MessageBox(NULL, sz, 0, 0);
		delete engine;
	}

	CoUninitialize();    //ǰ�������CoInitialize(�����������ʱҪ�ر�Com����

	return -1;
}