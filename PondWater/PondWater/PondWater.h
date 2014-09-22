//=============================================================================
// PondWater.h by Frank Luna (C) 2004 All Rights Reserved.
//
// Note: If you want large bodies of water, then you should break
//       the water mesh into sub-grids like we did with the terrain
//       so that you can frustum cull.
//=============================================================================

#ifndef POND_WATER_H
#define POND_WATER_H

#include "d3dUtil.h"
#include "DrawableTex2D.h"

class PondWater
{
public:
	struct InitInfo
	{
		DirLight dirLight;
		Mtrl     mtrl;
		int      vertRows;
		int      vertCols;
		float    dx;
		float    dz;
		tstring waveMapFilename0;
		tstring waveMapFilename1;
		D3DXVECTOR2 waveMapVelocity0;
		D3DXVECTOR2 waveMapVelocity1;
		float texScale;
		float refractBias;
		float refractPower;
		D3DXVECTOR2 rippleScale;
		D3DXMATRIX toWorld;
	};

public:
	PondWater(InitInfo& initInfo);

	~PondWater();

	DWORD getNumTriangles();
	DWORD getNumVertices();

	void onLostDevice();
	void onResetDevice();

	void update(float dt);
	void draw();

private:
	void buildFX();

public:
	// We make public to make it easier to render scene into.
	DrawableTex2D* mReflectMap;
	DrawableTex2D* mRefractMap;

private:
	ID3DXMesh* mMesh;
	ID3DXEffect* mFX;

	// The two normal maps to scroll.
	IDirect3DTexture9* mWaveMap0;
	IDirect3DTexture9* mWaveMap1;

	// Offset of normal maps for scrolling (vary as a function of time)
	D3DXVECTOR2 mWaveMapOffset0;
	D3DXVECTOR2 mWaveMapOffset1;

	InitInfo mInitInfo;
	float mWidth;
	float mDepth;

	D3DXHANDLE mhTech;
	D3DXHANDLE mhWVP;
	D3DXHANDLE mhWorld;
	D3DXHANDLE mhWorldInv;
	D3DXHANDLE mhLight;
	D3DXHANDLE mhMtrl;
	D3DXHANDLE mhEyePosW;
	D3DXHANDLE mhWaveMap0;
	D3DXHANDLE mhWaveMap1;
	D3DXHANDLE mhWaveMapOffset0;
	D3DXHANDLE mhWaveMapOffset1;
	D3DXHANDLE mhRefractBias;
	D3DXHANDLE mhRefractPower;
	D3DXHANDLE mhRippleScale;
	D3DXHANDLE mhReflectMap;
	D3DXHANDLE mhRefractMap;
};

#endif // POND_WATER_H