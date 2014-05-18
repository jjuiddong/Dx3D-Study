//-----------------------------------------------------------------------------//
// 2005-10-04  programer: jaejung ┏(⊙д⊙)┛
// 
// 2007-12-24 Chrismas Eve 에 코딩하다.
//		- Model, Animation 로딩시 자동으로 바이너리파일 저장, 로딩가능하게 구현함
//-----------------------------------------------------------------------------//

#if !defined(__FILELOADER_H__)
#define __FILELOADER_H__

#pragma warning (disable: 4786)
#include <map>
#include <string>
using namespace std;

#include "d3dx9.h"
#include "linearmemloader.h"

//-----------------------------------------------------------------------------//
// CFileLoader
//-----------------------------------------------------------------------------//
class CFileLoader
{
protected:
	CFileLoader() {}
	virtual ~CFileLoader() {}

protected:
	enum MEMTYPE { MT_TREE, MT_LINEAR };
	typedef struct _tagSMapItem
	{
		_tagSMapItem() {}
		_tagSMapItem( MEMTYPE t, BYTE *p ) : eType(t), pItem(p) {}
		MEMTYPE eType;
		BYTE *pItem;
	} SMapItem;

	typedef struct _tagModelData
	{
		SBMMLoader *pBmm;
		IDirect3DVertexBuffer9 *pVtxBuff;
		IDirect3DIndexBuffer9 *pIdxBuff;
	} ModelData;

	typedef map< string, SMapItem > ModelMap;
	typedef map< string, SMapItem > KeyMap;
	typedef map< string, IDirect3DTexture9* > TextureMap;
	typedef TextureMap::iterator TextureItor;
	typedef ModelMap::iterator ModelItor;
	typedef KeyMap::iterator KeyItor;

	static IDirect3DDevice9 *s_pDevice;
	static ModelMap s_ModelMap;
	static KeyMap s_KeyMap;
	static TextureMap s_TextureMap;
	static CLinearMemLoader s_MemLoader;
	static int m_ObjId;						// unique한 오브젝트 아이디를 생성한다.

public:
	static void Init( IDirect3DDevice9* pDevice );
	static SBMMLoader* LoadModel( char *szFileName );
	static SAniLoader* LoadAnimation( char *szFileName );
	static IDirect3DTexture9* LoadTexture( char *szFileName );
	static int LoadCombo( char *pFileName, SCombo *pReval[] );
	static SScene* LoadUI( char *pFileName );
	static BOOL WriteScripttoBinary_Model( char *szSrcFileName, char *szDstFileName );
	static BOOL WriteScripttoBinary_Ani( char *szSrcFileName, char *szDstFileName );
	static BOOL WriteScripttoBinary_Model( SBMMLoader *pModelLoader, char *szDstFileName );
	static BOOL WriteScripttoBinary_Ani( SAniLoader *pAniLoader, char *szDstFileName );
	static void Release();

protected:
	static SBMMLoader* LoadBMM_GPJ( char *szFileName );
	static SBMMLoader* LoadBMM_Bin( char *szFileName );
	static SAniLoader* LoadAni_GPJ( char *szFileName );
	static SKeyGroupLoader* LoadAni_Bin( char *szFileName );
	static BOOL ConvertAniListToAniform( char *szSrcFileName, char *szDstFileName );
	static void ModifyTextureFilename( SMaterialGroupLoader *pLoader );
	static BOOL CreateIndexBuffer( SBMMLoader *pLoader );
	static BOOL CreateVertexBuffer( SBMMLoader *pLoader );

};

#endif // __FILELOADER_H__
