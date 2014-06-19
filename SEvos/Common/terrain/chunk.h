
#ifndef __CHUNK_H__
#define __CHUNK_H__

#include "region.h"

class CTerrain;
class CTerrainCursor;
class CShader;
class CChunk : public CRegion
{
public:
	CChunk();
	virtual ~CChunk();

	enum {MAX_LAYER_CNT = 5,};
	struct SLayer
	{
		// 정적 용량을 줄이기위해 포인터로 선언되었다.
		int layer;						// layer number
		DWORD flag;						// visible=0x01, edit=0x20
		char palphaname[MAX_PATH];		// 알파 텍스쳐 파일이름
		char ptexturename[MAX_PATH];	// 텍스쳐 파일이름
		IDirect3DTexture9 *ptexture;	// 텍스쳐 
		IDirect3DTexture9 *palpha;		// 알파 텍스쳐 (0번 인덱스에 있는 값만 유효하다.)
										// alpha.a = 2번 레이어 알파
										// alpha.r = 3번 레이어 알파
										// alpha.g = 4번 레이어 알파
										// alpha.b = 5번 레이어 알파
	};

protected:
	int m_colIndex;
	int m_rowIndex;
	int m_AlphaTextureSize;
	int m_layerCount;
	SLayer m_layer[ MAX_LAYER_CNT];

public:
	void Init(int colIndex, int rowIndex, int chunkPosX, int chunkPosY, int chunkWidth, int chunkHeight, 
		float cellSize, int alphaTextureSize);
	BOOL LoadLayer(SChunkLoader *pLoader);
	void Render(CShader *pShader, int pass);
//	void RenderFixedPipeline();
	void RenderDepth();
	virtual void Clear();

	BOOL GetChunkLoader(char *szAlphaTextureWriteLocatePath, SChunkLoader *ploader);

	// Terrain Edit
	void ModifyHeight(CTerrainCursor *pcursor, int nElapsTick);
	void UpdateChunkToMainTerrain(CTerrain *pmainTerrain);
	void UpdateMainTerrainToChunk(CTerrain *pmainTerrain);

	// Brush
	void DrawBrush( CTerrainCursor *pcursor );

	// Layer
	SLayer* GetTopEditLayer( CTerrainCursor *pcursor);
	int GetLayerCount() { return m_layerCount; }
	SLayer* GetLayer(int idx);
	BOOL AddLayer();
	BOOL InsertLayer( int insert_layeridx, SLayer *player );
	BOOL DeleteLayer( int layeridx );
	BOOL MergeLayer( int layeridx_from, int layeridx_to );
	BOOL MoveLayer( int layeridx_from, int layeridx_to );
	int GetMaxLayerCount() { return MAX_LAYER_CNT; }
	void ClearLayer();

protected:
	void InitLayer();
	void SetLayerAphaTexture(SLayer *player, BYTE val);
	void Brush( float u, float v, CTerrainCursor *pcursor, int layer, IDirect3DTexture9 *palpha );
	void GetBrushUV(Vector3 pos, float *pu, float *pv );
	BOOL GenerateAlphaTextureFile( char *szAlphaTextureWriteLocatePath, int layerIndex, SLayer *pLayer, char *szAlphaTextFileName);
	BOOL LoadAlphaTexture( char *szAlphaTextureFileName, IDirect3DTexture9 *pAlphaTexture);
	BOOL AddLayer( char *szTextureName, char *szAlphaname=NULL );

	void GetAlphaTextureFileName( int chunkColIndex, int chunkRowIndex, int layerIndex, 
		char *newAlphaTextureFileName, int stringSize);
	DWORD GetAlphaMask(int layerNum);
	int GetAlphaShiftSize(int layerNum);
	void DeleteAlphaTexture(int layerNum);

};

#endif
