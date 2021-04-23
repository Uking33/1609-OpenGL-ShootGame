#include "md2.h"

// CalculateNormal()
// desc: given 3 points, calculates the normal to the points
void CalculateNormal( float *p1, float *p2, float *p3 )
{
   float a[3], b[3], result[3];
   float length;

   a[0] = p1[0] - p2[0];
   a[1] = p1[1] - p2[1];
   a[2] = p1[2] - p2[2];

   b[0] = p1[0] - p3[0];
   b[1] = p1[1] - p3[1];
   b[2] = p1[2] - p3[2];

   result[0] = a[1] * b[2] - b[1] * a[2];
   result[1] = b[0] * a[2] - a[0] * b[2];
   result[2] = a[0] * b[1] - b[0] * a[1];

   // calculate the length of the normal
   length = (float)sqrt(result[0]*result[0] + result[1]*result[1] + result[2]*result[2]);

   // normalize and specify the normal
   glNormal3f(result[0]/length, result[1]/length, result[2]/length);
}

// vector subtraction
vector_t operator-(vector_t a, vector_t b)
{
	vector_t c;

	c.point[0] = a.point[0] - b.point[0];
	c.point[1] = a.point[1] - b.point[1];
	c.point[2] = a.point[2] - b.point[2];

	return c;
}

// scalar-vector multiplication
vector_t operator*(float f, vector_t b)
{
	vector_t c;

	c.point[0] = f * b.point[0];
	c.point[1] = f * b.point[1];
	c.point[2] = f * b.point[2];

	return c;
}

// vector division
vector_t operator/(vector_t a, vector_t b)
{
	vector_t c;

	c.point[0] = a.point[0] / b.point[0];
	c.point[1] = a.point[1] / b.point[1];
	c.point[2] = a.point[2] / b.point[2];

	return c;
}

// vector addition
vector_t operator+(vector_t a, vector_t b)
{
	vector_t c;

	c.point[0] = a.point[0] + b.point[0];
	c.point[1] = a.point[1] + b.point[1];
	c.point[2] = a.point[2] + b.point[2];

	return c;
}

// CMD2Model constructor
CMD2Model::CMD2Model()
{
     numVertices = 0;    // vertices
     numTriangles = 0;   // triangles
     numFrames = 0;      // frames
     numST = 0;          // texture coordinates
     frameSize = 0;      // needed?
     currentFrame = 0;   // current keyframe 
     nextFrame = 1;      // next keyframe
     interpol = 0.0;     // interpolation percent
     triIndex = NULL;    // triangle indices
     st = NULL;          // texture coordinate indices
     vertexList = NULL;  // vertices
     modelTex = new CTexture;    // skin/texture
	modelState = MODEL_IDLE;
}    

// CMD2Model destructor
CMD2Model::~CMD2Model()
{
	Unload();
}

// CMD2Model::SetupSkin()
// access: private
// desc: sets up the model skin/texture for OpenGL
void CMD2Model::SetupSkin(CTexture *thisTexture)
{
     // set the proper parameters for an MD2 texture
     glGenTextures(1, &thisTexture->texID);
     glBindTexture(GL_TEXTURE_2D, thisTexture->texID);
     glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
     glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
     glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_NEAREST);
     
     switch (thisTexture->textureType)
     {
     case BMP:
          gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, thisTexture->width, thisTexture->height, 
               GL_RGB, GL_UNSIGNED_BYTE, thisTexture->data);
          break;
     case PCX:
          gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, thisTexture->width, thisTexture->height,GL_RGBA, GL_UNSIGNED_BYTE, thisTexture->data);
		break;
     case TGA:
		  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, thisTexture->width, thisTexture->height,GL_RGB, GL_UNSIGNED_BYTE, thisTexture->data);
          break;
     default:
          break;
     }
}

// CMD2Model::Load()
// access: public
// desc: loads model and skin(加载模型和纹理)
int CMD2Model::Load(char *modelFile, char *skinFile)
{
     FILE *filePtr;                          // file pointer（文件指针）
     int fileLen;                            // length of model file（模型文件长度）
     char *buffer;                           // file buffer（文件缓冲区）
											 
     modelHeader_t *modelHeader;             // model header（文件头）
											 
     stIndex_t *stPtr;                       // texture data（纹理数据）
     frame_t *frame;                         // frame data（帧数据）
     vector_t *vertexListPtr;                // 每帧中顶点列表数据开始的位置
     mesh_t *bufIndexPtr;                    // 文件缓冲区中面列表数据开始的位置
     int i, j;                               // 循环变量
     
     // open the model file（打开模型文件）
     filePtr = fopen(modelFile, "rb");  //rb:以二进制读取方式打开
     if (filePtr == NULL)
          return FALSE;
     
     // find length of file（取得文件长度）
     fseek(filePtr, 0, SEEK_END);
     fileLen = ftell(filePtr);
     fseek(filePtr, 0, SEEK_SET);
     
     // read entire file into buffer（把整个文件全部数据读进文件缓冲区）
     buffer = new char [fileLen+1];
     fread(buffer, sizeof(char), fileLen, filePtr);
     
     // extract model file header from buffer（从缓冲区取出文件头信息）
     modelHeader = (modelHeader_t*)buffer;
     
	 // 根据模型本来有多少个顶点和帧数来新建顶点列表储存空间
     vertexList = new vector_t [modelHeader->numXYZ * modelHeader->numFrames];
     
	 // 从文件头中读出顶点数量(一帧)、帧数、一帧的内存大小
     numVertices = modelHeader->numXYZ;
     numFrames = modelHeader->numFrames;
     frameSize = modelHeader->framesize;
     
	 // 所有帧的顶点数据都读出存到顶点数组vertexList[]中
     for (j = 0; j < numFrames; j++)
     {
          frame = (frame_t*)&buffer[modelHeader->offsetFrames + frameSize * j];//取得顶点列表数组vertexList中的一个位置，每帧的顶点列表应从该位置开始
          
          vertexListPtr = (vector_t*)&vertexList[numVertices * j];
          for (i = 0; i < numVertices; i++)
          {
               vertexListPtr[i].point[0] = frame->scale[0] * frame->fp[i].v[0] + frame->translate[0];
               vertexListPtr[i].point[1] = frame->scale[1] * frame->fp[i].v[1] + frame->translate[1];
               vertexListPtr[i].point[2] = frame->scale[2] * frame->fp[i].v[2] + frame->translate[2];
          }
     }
     
	 //加载纹理
     modelTex->LoadTexture(skinFile);// = LoadTexture(skinFile);
     if (modelTex != NULL)
          SetupSkin(modelTex);  //绑定和设置纹理
     else
          return FALSE;
     
     numST = modelHeader->numST; //从文件头中读出有多少个纹理坐标数据
     st = new texCoord_t [numST];//创建纹理坐标数组
     
     stPtr = (stIndex_t*)&buffer[modelHeader->offsetST];//在文件缓冲区中取得储存纹理坐标数据的位置
     for (i = 0; i < numST; i++ )//读入所有纹理坐标数据
     {
          st[i].s = (float)stPtr[i].s / (float)modelTex->width;
          st[i].t = (float)stPtr[i].t / (float)modelTex->height;
     }
     
	 //顶点数组和纹理坐标数组最终怎样有序地组建成漂亮模型呢?这全靠面列表.以下读出面列表

     numTriangles = modelHeader->numTris;    //从文件头读出面列表中有多少个三角形
     triIndex = new mesh_t [numTriangles];   //创建面列表数组
     
     // point to triangle indexes in buffer(在文件缓冲区中取得面列表的位置)
     bufIndexPtr = (mesh_t*)&buffer[modelHeader->offsetTris];
     
     // create a mesh (triangle) list(读出面列表中所有三角形的顶点索引信息)
     for (j = 0; j < numFrames; j++)       //外循环要不要都无所谓的，看下面内循环的代码中也没有j的。实际上不管是哪一帧，三角形构成都是一样的    
     {
          // for all triangles in each frame
          for(i = 0; i < numTriangles; i++)
          {
			   //顶点索引
               triIndex[i].meshIndex[0] = bufIndexPtr[i].meshIndex[0];
               triIndex[i].meshIndex[1] = bufIndexPtr[i].meshIndex[1];
               triIndex[i].meshIndex[2] = bufIndexPtr[i].meshIndex[2];
			   //纹理索引
               triIndex[i].stIndex[0] = bufIndexPtr[i].stIndex[0];
               triIndex[i].stIndex[1] = bufIndexPtr[i].stIndex[1];
               triIndex[i].stIndex[2] = bufIndexPtr[i].stIndex[2];
          }
     }
     
     // close file and free memory(关闭文件,释放内存)
     fclose(filePtr);
	 delete buffer;
     
	 //初始化动画播放的信息
     currentFrame = 0;
     nextFrame = 1;
     interpol = 0.0;//动画插值百分比
     
     return TRUE;
}

// CMD2Model::LoadModel()
// access: public
// desc: loads model from file
int CMD2Model::LoadModel(char *modelFile)
{
     FILE *filePtr;                          // file pointer
     int fileLen;                            // length of model file
     char *buffer;                           // file buffer
     
     modelHeader_t *modelHeader;             // model header
     
     stIndex_t *stPtr;                       // texture data
     frame_t *frame;                              // frame data
     vector_t *vertexListPtr;                // index variable
     mesh_t *triIndex, *bufIndexPtr;         // index variables
     int i, j;                               // index variables
     
     // open the model file
     filePtr = fopen(modelFile, "rb");
     if (filePtr == NULL)
          return FALSE;
     
     // find length of file
     fseek(filePtr, 0, SEEK_END);
     fileLen = ftell(filePtr);
     fseek(filePtr, 0, SEEK_SET);
     
     // read entire file into buffer
     buffer = new char [fileLen+1];
     fread(buffer, sizeof(char), fileLen, filePtr);
     
     // extract model file header from buffer
     modelHeader = (modelHeader_t*)buffer;
     
     // allocate vertex list
     vertexList = new vector_t [modelHeader->numXYZ * modelHeader->numFrames];
     
     numVertices = modelHeader->numXYZ;
     numFrames = modelHeader->numFrames;
     frameSize = modelHeader->framesize;
     
     for (j = 0; j < numFrames; j++)
     {
          frame = (frame_t*)&buffer[modelHeader->offsetFrames + frameSize * j];
          
          vertexListPtr = (vector_t*)&vertexList[numVertices * j];
          for (i = 0; i < numVertices; i++)
          {
               vertexListPtr[i].point[0] = frame->scale[0] * frame->fp[i].v[0] + frame->translate[0];
               vertexListPtr[i].point[1] = frame->scale[1] * frame->fp[i].v[1] + frame->translate[1];
               vertexListPtr[i].point[2] = frame->scale[2] * frame->fp[i].v[2] + frame->translate[2];
          }
     }
     
     numST = modelHeader->numST;
     
     st = new texCoord_t [numST];
     
     stPtr = (stIndex_t*)&buffer[modelHeader->offsetST];
     for (i = 0; i < numST; i++)
     {
          st[i].s = 0.0;
          st[i].t = 0.0;
     }
     
     numTriangles = modelHeader->numTris;
     triIndex = new mesh_t [numTriangles];
     
     // point to triangle indexes in buffer
     bufIndexPtr = (mesh_t*)&buffer[modelHeader->offsetTris];
     
     // create a mesh (triangle) list
     for (j = 0; j < numFrames; j++)         
     {
          // for all triangles in each frame
          for(i = 0; i < numTriangles; i++)
          {
               triIndex[i].meshIndex[0] = bufIndexPtr[i].meshIndex[0];
               triIndex[i].meshIndex[1] = bufIndexPtr[i].meshIndex[1];
               triIndex[i].meshIndex[2] = bufIndexPtr[i].meshIndex[2];
               triIndex[i].stIndex[0] = bufIndexPtr[i].stIndex[0];
               triIndex[i].stIndex[1] = bufIndexPtr[i].stIndex[1];
               triIndex[i].stIndex[2] = bufIndexPtr[i].stIndex[2];
          }
     }
     
     // close file and free memory
     fclose(filePtr);
	delete buffer;
     
     modelTex = NULL;
     currentFrame = 0;
     nextFrame = 1;
     interpol = 0.0;
     
     return 0;
}

// CMD2Model::LoadSkin()
// access: public
// desc: loads a skin for the model
int CMD2Model::LoadSkin(char *skinFile)
{
     int i;
     
     modelTex->LoadTexture(skinFile);
     
     if (modelTex != NULL)
          SetupSkin(modelTex);
     else
          return -1;
     
     for (i = 0; i < numST; i++)
     {
          st[i].s /= (float)modelTex->width;
          st[i].t /= (float)modelTex->height;
     }
     
     return 0;
}

// CMD2Model::SetTexture()
// access: public
// desc: sets a new texture object 
int CMD2Model::SetTexture(CTexture *texture)
{
     int i;
     
     if (texture != NULL)
	{
		free(modelTex);
          modelTex = texture;
	}
     else
          return -1;
     
     SetupSkin(modelTex);
     
     for (i = 0; i < numST; i++)
     {
          st[i].s /= (float)modelTex->width;
          st[i].t /= (float)modelTex->height;
     }
     
     return 0;
}

// CMD2Model::Animate()
// access: public
// desc: animates the model between the keyframes startFrame and endFrame
int CMD2Model::AnimateModel(int startFrame, int endFrame, float percent)
{
     vector_t *vList;             // current frame vertices(顶点列表中当前帧位置）
     vector_t *nextVList;         // next frame vertices(顶点列表中下一帧位置）
     int i;                       // index counter（循环变量）
     float x1, y1, z1;            // current frame point values（顶点在当前帧的坐标）
     float x2, y2, z2;            // next frame point values（顶点在下一帧的坐标）
								  
     vector_t vertex[3];		  // 三角形的三个顶点（在插值帧中）
     
     if ((startFrame > currentFrame))
          currentFrame = startFrame;
     
     if ((startFrame < 0) || (endFrame < 0))
          return -1;
     
     if ((startFrame >= numFrames) || (endFrame >= numFrames))
          return -1;
     
     if (interpol >= 1.0)
     {
          interpol = 0.0f;
          currentFrame++;
          if (currentFrame >= endFrame)
               currentFrame = startFrame;
          
          nextFrame = currentFrame + 1;
          
          if (nextFrame >= endFrame)
               nextFrame = startFrame;
          
     }
     
     vList = &vertexList[numVertices*currentFrame];
     nextVList = &vertexList[numVertices*nextFrame];
     
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
     glBindTexture(GL_TEXTURE_2D, modelTex->texID);
     glBegin(GL_TRIANGLES);
     for (i = 0; i < numTriangles; i++)    //画出面列表的所有面（三角形）
     {
          // get first points of each frame
          x1 = vList[triIndex[i].meshIndex[0]].point[0];
          y1 = vList[triIndex[i].meshIndex[0]].point[1];
          z1 = vList[triIndex[i].meshIndex[0]].point[2];
          x2 = nextVList[triIndex[i].meshIndex[0]].point[0];
          y2 = nextVList[triIndex[i].meshIndex[0]].point[1];
          z2 = nextVList[triIndex[i].meshIndex[0]].point[2];
          
          // store first interpolated vertex of triangle(插值计算出三角形第一点）
          vertex[0].point[0] = x1 + interpol * (x2 - x1);
          vertex[0].point[1] = y1 + interpol * (y2 - y1);
          vertex[0].point[2] = z1 + interpol * (z2 - z1);
          
          // get second points of each frame
          x1 = vList[triIndex[i].meshIndex[2]].point[0];
          y1 = vList[triIndex[i].meshIndex[2]].point[1];
          z1 = vList[triIndex[i].meshIndex[2]].point[2];
          x2 = nextVList[triIndex[i].meshIndex[2]].point[0];
          y2 = nextVList[triIndex[i].meshIndex[2]].point[1];
          z2 = nextVList[triIndex[i].meshIndex[2]].point[2];
          
          // store second interpolated vertex of triangle(插值计算出三角形第二点）
          vertex[2].point[0] = x1 + interpol * (x2 - x1);
          vertex[2].point[1] = y1 + interpol * (y2 - y1);
          vertex[2].point[2] = z1 + interpol * (z2 - z1);   
          
          // get third points of each frame
          x1 = vList[triIndex[i].meshIndex[1]].point[0];
          y1 = vList[triIndex[i].meshIndex[1]].point[1];
          z1 = vList[triIndex[i].meshIndex[1]].point[2];
          x2 = nextVList[triIndex[i].meshIndex[1]].point[0];
          y2 = nextVList[triIndex[i].meshIndex[1]].point[1];
          z2 = nextVList[triIndex[i].meshIndex[1]].point[2];
          
          // store third interpolated vertex of triangle(插值计算出三角形第三点）
          vertex[1].point[0] = x1 + interpol * (x2 - x1);
          vertex[1].point[1] = y1 + interpol * (y2 - y1);
          vertex[1].point[2] = z1 + interpol * (z2 - z1);
          
          // calculate the normal of the triangle（计算三角形法线）
          CalculateNormal(vertex[0].point, vertex[2].point, vertex[1].point);
          
          // render properly textured triangle
          glTexCoord2f(st[triIndex[i].stIndex[0]].s,   
               st[triIndex[i].stIndex[0]].t);       //不同帧的对应点的纹理坐标都是一样的。动画播放过程中，顶点坐标不断变化，纹理坐标却是不变的
          glVertex3fv(vertex[0].point);
          
          glTexCoord2f(st[triIndex[i].stIndex[2]].s ,
               st[triIndex[i].stIndex[2]].t);
          glVertex3fv(vertex[2].point);
          
          glTexCoord2f(st[triIndex[i].stIndex[1]].s,
               st[triIndex[i].stIndex[1]].t);
          glVertex3fv(vertex[1].point);
     }
     glEnd();
     glDisable(GL_TEXTURE_2D);

     interpol += percent;  // increase percentage of interpolation between frames
     
     return 0;
}

// RenderFrame()
// desc: renders a single key frame
int CMD2Model::RenderFrame(int keyFrame)
{
     vector_t *vList;
     int i;
     
     // create a pointer to the frame we want to show
     vList = &vertexList[numVertices * keyFrame];
     
	if (modelTex != NULL)
		// set the texture
		glBindTexture(GL_TEXTURE_2D, modelTex->texID);
     
     // display the textured model with proper lighting normals
     glBegin(GL_TRIANGLES);
     for(i = 0; i < numTriangles; i++)
     {
		  //计算法线
          CalculateNormal(vList[triIndex[i].meshIndex[0]].point,
               vList[triIndex[i].meshIndex[2]].point,
               vList[triIndex[i].meshIndex[1]].point);
		  //第一点
		  if (modelTex != NULL)
			  glTexCoord2f(st[triIndex[i].stIndex[0]].s,
				           st[triIndex[i].stIndex[0]].t);
          glVertex3fv(vList[triIndex[i].meshIndex[0]].point);
          //第二点
		  if (modelTex != NULL)
			  glTexCoord2f(st[triIndex[i].stIndex[2]].s ,
				           st[triIndex[i].stIndex[2]].t);
          glVertex3fv(vList[triIndex[i].meshIndex[2]].point);
          //第三点
		  if (modelTex != NULL)
			  glTexCoord2f(st[triIndex[i].stIndex[1]].s,
				           st[triIndex[i].stIndex[1]].t);
          glVertex3fv(vList[triIndex[i].meshIndex[1]].point);
     }
     glEnd();
     
     return 0;
}

// Unload()
// desc: unloads model data from memory
void CMD2Model::Unload()
{
     if (triIndex != NULL)
          delete [] triIndex;
     if (vertexList != NULL)
          delete [] vertexList;
     if (st != NULL)
          delete [] st; 

	if (modelTex != NULL)
		delete modelTex;
}

// SetState()
// desc: set the model state
void CMD2Model::SetState(modelState_t state)
{
	modelState = state;
}

// GetState()
// desc: retrieve the model state
modelState_t CMD2Model::GetState()
{
	return modelState;
}
