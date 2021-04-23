#ifndef __TERRAIN_H
#define __TERRAIN_H

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN


#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include "camera.h"
#include "object.h"
#include "texture.h"
#include "vector.h"

class CTerrain : public CObject
{
private:
	int width;		// terrain is of size width X width
					// preferrably with 2^n = width
	float terrainMul;
	float heightMul;
	float scanDepth;
	float textureMul;

	float RangedRandom(float v1,float v2);
	void NormalizeTerrain(float field[],int size);
	void FilterHeightBand(float *band,int stride,int count,float filter);
	void FilterHeightField(float field[],int size,float filter);
	void MakeTerrainPlasma(float field[],int size,float rough);

protected:
	// terrain doesn't move, so no physics animations
	void OnAnimate(scalar_t deltaTime) {}

	void OnDraw(CCamera *camera);
	void OnCollision(CObject *collisionObject);

public:
	float *heightMap;		// dynamic heightmap
	CTexture terrainTex[5];	// for multiple textures on the terrain
	float fogColor[4];		// color of the fog/sky

	CTerrain();
	CTerrain(int width, float rFactor);
	~CTerrain() { delete [] heightMap; }

	void Load() {}
	void Unload() {}

	void BuildTerrain(int width, float rFactor);

	float GetWidth() { return width; }
	float GetMul() { return terrainMul; }
	float GetScanDepth() { return scanDepth; }

	float GetHeight(double x, double z)
	{	
		float projCameraX, projCameraZ;

		// divide by the grid-spacing if it is not 1
		projCameraX = x / terrainMul;
		projCameraZ = z / terrainMul;

		// compute the height field coordinates (Col0, Row0)
		// and (Col1, Row1) that identify the height field cell 
		// directly below the camera.
		int col0 = int(projCameraX);
		int row0 = int(projCameraZ);
		int col1 = col0 + 1;
		int row1 = row0 + 1;
		
		// make sure that the cell coordinates don't fall
		// outside the height field.
		if (col0>width || col0<0)   //bug
			col0 = 0;
		if (row0>width || row0<0)
			row0 = 0;
		if (col0 < 0)
			col0 = 0;
		if (row1 < 0)
			row1 = 0;               //end bug
		if (col1 > width)
			col1 = 0;
		if (row1 > width)
			row1 = 0;

		// get the four corner heights of the cell from the height field
		float h00 = heightMul * (float)heightMap[col0 + row0*width];
		float h01 = heightMul * (float)heightMap[col1 + row0*width];
		float h11 = heightMul * (float)heightMap[col1 + row1*width];
		float h10 = heightMul * (float)heightMap[col0 + row1*width];

		// calculate the position of the camera relative to the cell.
		// note, that 0 <= tx, ty <= 1.
		float tx = projCameraX - float(col0);
		float ty = projCameraZ - float(row0);

		// the next step is to perform a bilinear interpolation
		// to compute the height of the terrain directly below
		// the object.
		float txty = tx * ty;

		float final_height = h00 * (1.0f - ty - tx + txty)
						+ h01 * (tx - txty)
						+ h11 * txty
						+ h10 * (ty - txty);

		return final_height;
	}
};

#endif