#define WIN32_MEAN_AND_LEAN
#define WIN32_EXTRA_LEAN

#include "font.h"

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <gl/gl.h>

CFont::CFont()
{
	screenX = 0;
	screenY = 0;
	xpos = 0.0;
	ypos = 0.0;
	zpos = 0.0;
}

CFont::CFont(char *name, int size)
{
	screenX = 0;
	screenY = 0;
	xpos = 0.0;
	ypos = 0.0;
	zpos = 0.0;
	Build(name, size);
}

CFont::~CFont()
{
	glDeleteLists(callList, 96);
}

void CFont::Build(char *name, int size)
{
	HFONT hFont;	// font ID
	HDC hDC;		// device context(�豸������/�豸������

	hDC = wglGetCurrentDC();   //��ÿͻ����豸�����ľ��
	callList = glGenLists(96); //����96����ʾ�б������ʼ�Ǹ���ʾ�б�ı��

	if (stricmp(name, "symbol") == 0)
	{
		hFont = CreateFont(-size, 0,0,0,FW_BOLD, FALSE, FALSE, FALSE, SYMBOL_CHARSET,
						OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
						FF_DONTCARE | DEFAULT_PITCH, name);
	}
	else
	{
		hFont = CreateFont(-size, 0,0,0,FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
					OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 
					FF_DONTCARE | DEFAULT_PITCH, name);
	}

	SelectObject(hDC, hFont);
	wglUseFontBitmaps(hDC, 32, 96, callList);  //��ASCII�ַ�װ����ʾ�б�Ȼ�����ʹ��glCallLists()����������ʾ�б�������ʾ�ı�
	           //�ӿո�ascii=32����ÿ���ַ�����һ����ʾ�б��浽���ΪcallList��ʼ����ʾ�б��У�һ��Ϊ96���ַ�������ʾ�б�
	           //�����ո�32��--callList����ʾ�б�
	           //    !(33) -- callList+1����ʾ�б�
	           //    "(34) -- callList+2����ʾ�б�
	           //    ......
	           //    1(48) -- callList+16����ʾ�б�
	           //    ......
	           //    A(65) -- callList+33����ʾ�б�
	           //    ......
	           //    a(97) -- callList+65����ʾ�б�
	           //    ......   
}

void CFont::Print(const char *str, ...)
{
	char text[256];
	va_list args;   //���������б�

	if (str == NULL)
		return;

	va_start(args, str); //ָ�����������б���ڴ�λ��(���Ǹ���ַ��ʼ)
		vsprintf(text, str, args); //argsָ��str����Ĳ��������str="Time:%d:%d.%d",args="5,0,0", ��text����ӡ��"Time:5:0.0"
	va_end(args);        //��ǰ��va_start()���ʹ�õ����

	glPushMatrix();
	glColor4f(r, g, b, a);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -1.0f);	// translate one unit into the screen
	if (xpos == 0.0 && ypos == 0.0 && zpos == 0.0)
		glRasterPos2i(screenX, screenY); //��դͼ��λ
	else
		glRasterPos3f(xpos, ypos, zpos); //��դͼ��λ    

	glPushAttrib(GL_LIST_BIT);     //��GL_LIST_BASE���£��Ա�һ��glPopAttrib���ָ�
		glListBase(callList - 32); //��Ϊ��ǰ��Build��������ʾ�б��ź��ַ�ascii��Ĺ�ϵ��������
		                       //asciiֵ��31(callList-32=1-32=-31)�������ַ���Ӧ����ʾ�б���ֵ��
		                       //  ascii       ��ʾ�б���
		                       //-------------------------------
		                       //�ո�32��         1
		                       //  !(33)            2
							   //  "(34)            3
							   //  ......
							   //  1(48)            17
							   //  ......
							   //  A(65)            34
							   //  ......
							   //  a(97)            66
							   //  ......   
		glCallLists(strlen(text), GL_UNSIGNED_BYTE, text); //ִ��strlen(text)����ʾ�б�����Ҫִ
		                       //���ĸ��ĸ���ʾ�б�Ҫ��text������ַ�ֵ��������ascii�룩.text��
		                       //��������0~255����GL_UNSIGNED_BYTE���͡�����
		                       //glCallLists(4,GL_UNSIGNED_BYTE,"Time") ����ʵ����Ҫִ�е�53��74��
		                       //78��68����ʾ�б���ΪT��asciiΪ84��84-31=53.��53����ʾ�б����þ�
		                       //��������Ļ�ϻ���T�ֵ��Ǹ���ʾ�б����Ǹղ�Build()���������õ�)��
		                       //i��m��e��������
	glPopAttrib(); //�ָ��ղ�glPushAttrib()���µ�GL_LIST_BASE
	glPopMatrix();
}

void CFont::ClearFont()
{
	glDeleteLists(callList, 96);
}
