
#ifndef __WATER_H__
#define __WATER_H__


class CShader;
class CTerrain;
class CWater : public CDispObject
{
public:
	CWater();
	virtual ~CWater();

protected:
	int m_ObjId;
	Matrix44 m_matWorld;
	IDirect3DTexture9 *m_pTex;
	IDirect3DVertexBuffer9 *m_pVtxBuff;
	IDirect3DIndexBuffer9 *m_pIdxBuff;
	LPDIRECT3DTEXTURE9 m_pWaterBump;		// π∞
	LPDIRECT3DCUBETEXTURE9 m_pReflectionMap;// «œ¥√¿« π›ªÁ ∏ 

	// shader
	CShader *m_pShader;
	LPDIRECT3DVERTEXDECLARATION9 m_pDecl;
	float m_IncTime;

public:
	void Init(CTerrain *pTerrain);
	virtual int GetId() { return m_ObjId; }
	virtual void Render();
	virtual void RenderDepth();
	virtual BOOL Update( int elapseTime );
	virtual Matrix44* GetWorldTM();
	virtual void Clear();

};

#endif
