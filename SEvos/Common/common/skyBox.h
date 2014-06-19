
#ifndef __SKYBOX_H__
#define __SKYBOX_H__

//------------------------------------------------------------------------
// SkyBox
//
// [2011/3/1 jjuiddong]
//------------------------------------------------------------------------


class CSkyBox
{
public:
	CSkyBox();
	virtual ~CSkyBox();

protected:
	enum {FRONT, BACK, LEFT, RIGHT, TOP, BOTTOM, MAX_FACE};

	IDirect3DVertexBuffer9 *m_pVtxBuffer;
	IDirect3DTexture9 *m_pTexture[ MAX_FACE];

public:
	BOOL Init(char *szTextureFilePath);
	void Update(int elapseTime);
	void Render();
	void Clear();

protected:
	BOOL CreateVertexBuffer();	
	void SetRenderState();

};

#endif
