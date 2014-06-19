
#ifndef __SHADER_H__
#define __SHADER_H__


class CShader
{
public:
	CShader();
	virtual ~CShader();

protected:
	LPD3DXEFFECT m_pEffect;
	D3DXHANDLE m_hTechnique;
	LPDIRECT3DVERTEXDECLARATION9 m_pDecl;

public:
	BOOL Init(char *szFileName, char *szTechnique);
	void Begin();
	void BeginPass(int pass);
	void EndPass();
	void End();
	void Clear();

	void SetMatrix(char *pKey, const Matrix44 &mat);
	void SetTexture(char *pKey, IDirect3DTexture9 *pTexture);
	void SetTexture(char *pKey, LPDIRECT3DCUBETEXTURE9 pTexture);
	void SetFloat(char *pKey, float val);
	void SetVector(char *pKey, const Vector3 &vec );
	void SetVector(char *pKey, const D3DXVECTOR4 &vec );
	void CommitChanges();
	LPD3DXEFFECT GetEffect() { return m_pEffect; }

};

#endif
