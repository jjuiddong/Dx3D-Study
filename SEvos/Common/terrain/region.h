
#ifndef __REGION_H__
#define __REGION_H__

/*
좌표 체계

<VertexBuffer 에 저장된 순서>

0 1	2 3	4 5 6 7	| 8 9 10 ~~~ nVtxPerCol-1
nVtxPerCol..	|
				|
				|
				|Pos(0,0)
------------------------------------->x
				|
				|
				|
				|
				| ~~~~~~~~~~~~~~~~(nVtxPerCol * nVtxPerRow)-1



<Cell>
셀은 버텍스가 네개 모여 만들어진 정사각형이다. 삼각형 두개가 모여서 구성된다.
셀 크기는 m_CellSize에 저장된다.
실제로 셀 크기는 맵의 크기 Width, Height 와 nVtxPerCol, nVtxPerRow 에 의해서
결정된다.

Row 의 셀 갯수는 Row에 들어가는 버텍스 갯수에서 -1 한 것과 같다.
마찬가지로 Col 의 셀 갯수는 Col에 들어가는 버텍스 갯수에서 -1 한 것과 같다.

0 1	2 3	4 5 6 7	| 8 9 10 ~~~ nVtxPerRow-1
nVtxPerRow...	|
				|
				|
				|Pos(0,0)
-------------------------------------> x
				|
				|
				|
				|
				| ~~~~~~~~~~~~~~~~(nVtxPerCol * nVtxPerRow)-1

/**/

class CTerrainCursor;
class CRegion
{
public:
	CRegion();
	virtual ~CRegion();
	enum { VISIBLE=0x01, EDIT=0x02, FOCUS1=0x04, FOCUS2=0x08 };

protected:
	BOOL m_Visible;
	DWORD m_Flag;				// visible=0x01, focus=0x40, 
	int m_posX;					// 청크 위치 (x,y 청크단위축)
	int m_posY;
	int m_columnCellCount;		// 지역을 구성하는 셀의 가로 갯수
	int m_rowCellCount;			// 지역를 구성하는 셀의 세로 갯수
	int m_columnVtxCount;
	int m_rowVtxCount;
	float m_cellSize;			// 셀의 한변의 크기 (버텍스 사이 길이)
	// 전체 버텍스 갯수 = (cell_width_cnt+1) * (cell_height_cnt+1)
	// 전체 셀 갯수 = cell_width_cnt * cell_height_cnt 
	// 폴리곤 갯수 =  전체 셀 갯수 / 2
	// 지역 한변의 크기 = cell_width_cnt * cellsize (정사각형 일때)

	D3DMATERIAL9 m_mtrl;
	IDirect3DVertexBuffer9 *m_pvtxBuff;
	IDirect3DIndexBuffer9 *m_pidxBuff;
	IDirect3DVertexBuffer9 *m_pvtxBuff_Edge; // 외곽선 버텍스 정보

public:
	virtual void Init(int posX, int posY, int width, int height, float cellsize);
	virtual void Render();
	virtual void RenderEdge();
	virtual void RenderNormal();
	virtual void UpdateEdge();
	virtual void Clear();

	void SetFocus(BOOL bFocus);
	BOOL IsInRegion( CTerrainCursor *pcursor, Vector3 pos );
	BOOL Pick( Vector2 *pPos, Vector3 *pvTarget );
	void UpdateNormals();

	BOOL IsFocus();
	float GetRegionWidth() { return (float)m_columnCellCount * m_cellSize; }
	float GetRegionHeight() { return (float)m_rowCellCount * m_cellSize; }
	float GetHeight( float x, float y );
	float GetHeightMapEntry( int row, int col );
	Vector3 GetNormalVector(int vtxRow, int vtxCol );
	BOOL GetVertex(int row, int col, Vector3 *poutV);
	BOOL GetVertex(int vtxIndex, Vector3 *poutV);
	Vector2 GetPos() { return Vector2((float)m_posX, (float)m_posY); }
	int GetVertexCount() { return m_columnVtxCount * m_rowVtxCount; }
	int GetTriangleCount() { return m_columnCellCount * m_rowCellCount * 2; }
	int GetEdgeVertexCount();
	float GetCellSize() { return m_cellSize; }
	int GetColumnVertexCount() { return m_columnVtxCount; }
	int GetColumnCellCount() { return m_columnCellCount; }
	int GetRowVertexCount() { return m_rowVtxCount; }
	int GetRowCellCount() { return m_rowCellCount; }

	IDirect3DVertexBuffer9* GetVertexBuffer() { return m_pvtxBuff; }
	IDirect3DIndexBuffer9* GetIndexBuffer() { return m_pidxBuff; }
	D3DMATERIAL9* GetMaterial() { return &m_mtrl; }

protected:
	virtual void CreateEdge();
	virtual void CreateVertexBuffer(int posX, int posY, int width, int height, float cellSize);
	virtual void CreateIndexBuffer();
	BOOL isInDrawBrush(float u, float v, CTerrainCursor *pcursor);
	Vector3 _GetNormalVector(int vtxIdx1, int vtxIdx2, int vtxIdx3);

};

#endif
