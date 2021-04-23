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
	HDC hDC;		// device context(设备上下文/设备描述表）

	hDC = wglGetCurrentDC();   //获得客户区设备上下文句柄
	callList = glGenLists(96); //创建96个显示列表，返回最开始那个显示列表的编号

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
	wglUseFontBitmaps(hDC, 32, 96, callList);  //将ASCII字符装入显示列表，然后可以使用glCallLists()函数利用显示列表序列显示文本
	           //从空格（ascii=32）起，每个字符创建一个显示列表，存到编号为callList开始的显示列表中，一共为96个字符创建显示列表
	           //即：空格（32）--callList号显示列表
	           //    !(33) -- callList+1号显示列表
	           //    "(34) -- callList+2号显示列表
	           //    ......
	           //    1(48) -- callList+16号显示列表
	           //    ......
	           //    A(65) -- callList+33号显示列表
	           //    ......
	           //    a(97) -- callList+65号显示列表
	           //    ......   
}

void CFont::Print(const char *str, ...)
{
	char text[256];
	va_list args;   //函数参数列表

	if (str == NULL)
		return;

	va_start(args, str); //指定函数参数列表的内存位置(从那个地址开始)
		vsprintf(text, str, args); //args指到str后面的参数，如果str="Time:%d:%d.%d",args="5,0,0", 则text被打印成"Time:5:0.0"
	va_end(args);        //与前面va_start()配对使用的语句

	glPushMatrix();
	glColor4f(r, g, b, a);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -1.0f);	// translate one unit into the screen
	if (xpos == 0.0 && ypos == 0.0 && zpos == 0.0)
		glRasterPos2i(screenX, screenY); //光栅图像定位
	else
		glRasterPos3f(xpos, ypos, zpos); //光栅图像定位    

	glPushAttrib(GL_LIST_BIT);     //把GL_LIST_BASE记下，以备一会glPopAttrib来恢复
		glListBase(callList - 32); //因为按前面Build（）中显示列表编号和字符ascii码的关系，正好是
		                       //ascii值减31(callList-32=1-32=-31)正好是字符对应的显示列表编号值。
		                       //  ascii       显示列表编号
		                       //-------------------------------
		                       //空格（32）         1
		                       //  !(33)            2
							   //  "(34)            3
							   //  ......
							   //  1(48)            17
							   //  ......
							   //  A(65)            34
							   //  ......
							   //  a(97)            66
							   //  ......   
		glCallLists(strlen(text), GL_UNSIGNED_BYTE, text); //执行strlen(text)个显示列表，具体要执
		                       //行哪个哪个显示列表，要看text里面的字符值（即看其ascii码）.text里
		                       //的数据是0~255，即GL_UNSIGNED_BYTE类型。例如
		                       //glCallLists(4,GL_UNSIGNED_BYTE,"Time") ，其实就是要执行第53、74、
		                       //78、68号显示列表，因为T的ascii为84，84-31=53.而53号显示列表正好就
		                       //是能在屏幕上画出T字的那个显示列表（这是刚才Build()函数创建好的)。
		                       //i、m、e按此类推
	glPopAttrib(); //恢复刚才glPushAttrib()记下的GL_LIST_BASE
	glPopMatrix();
}

void CFont::ClearFont()
{
	glDeleteLists(callList, 96);
}
