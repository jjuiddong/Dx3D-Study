// Terrain.h: interface for the CTerrain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TERRAIN_H__304AF236_0AF3_4AFC_AB5A_F3DF58151BB4__INCLUDED_)
#define AFX_TERRAIN_H__304AF236_0AF3_4AFC_AB5A_F3DF58151BB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Model.h"

class CTerrain  
{
public:
	CTerrain();
	virtual ~CTerrain();

protected:
	
	STileForm* m_pTile;
	LPDIRECT3DTEXTURE9 m_pBaseTex;
	LPDIRECT3DTEXTURE9* m_pAlphaTex;
	LPDIRECT3DTEXTURE9* m_pOverTex;
	LPDIRECT3DVERTEXBUFFER9* m_pVB;
	LPDIRECT3DVERTEXBUFFER9* m_pBlockVB;
	LPDIRECT3DINDEXBUFFER9 m_pIB;
	LPDIRECT3DINDEXBUFFER9 m_pBlockIB;
	std::list<CModel*> m_lstModel;
	std::list<CModel*>::iterator m_iterModel;
	D3DMATERIAL9 m_Mtrl;
//	std::list<char*> m_lstOverTexName;
//	std::list<LPDIRECT3DTEXTURE9> m_lstOverTex;
	int m_iTileNum;

public:
	BOOL LoadTerrain( char *szFileName );
	BOOL SetTerrain();
	BOOL CreateTile();
	BOOL CreateModel();
	BOOL CreateAlphaTexture();
	BOOL CreateOverTexture();
	void Render();
	void Clear();
	FLOAT GetHeight(float x, float z);
	int GetAttribute();
	float IntersectTriangle(Vector3 Origin, Vector3 v0, Vector3 v1, Vector3 v2);
};

#endif // !defined(AFX_TERRAIN_H__304AF236_0AF3_4AFC_AB5A_F3DF58151BB4__INCLUDED_)
