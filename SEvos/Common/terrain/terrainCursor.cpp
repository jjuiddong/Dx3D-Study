
#include "stdafx.h"
#include "Terrain.h"
#include "TerrainEditor.h"
#include "terrainCursor.h"


const int MAX_VERTEX_COUNT = 64;

CTerrainCursor::CTerrainCursor()
{
	SetTerrain(NULL);
	m_Load = FALSE;

}

CTerrainCursor::CTerrainCursor(CTerrain *pterrain)
{
	SetTerrain(pterrain);

	m_Load = FALSE;
	m_IsTerrainUp = TRUE;
	m_Flag = 0;
	m_OutterRadius = 30.f;
	m_InnerRadius = 10.f;
	m_OutterAlpha = 0.f;
	m_InnerAlpha = 1.0f;
	m_Offset = 20.f;
	m_TextureName[ 0] = NULL;

}


CTerrainCursor::~CTerrainCursor()
{

}


BOOL CTerrainCursor::Init()
{
	m_InnerCursor.Create(0, MAX_VERTEX_COUNT, sizeof(SVtxDiff), SVtxDiff::FVF, MAX_VERTEX_COUNT);
	m_InnerCursor.setVtxBuffCount(0);
	m_InnerCursor.setIdxBuffCount(0);

	m_OutterCursor.Create(0, MAX_VERTEX_COUNT, sizeof(SVtxDiff), SVtxDiff::FVF, MAX_VERTEX_COUNT);
	m_OutterCursor.setVtxBuffCount(0);
	m_OutterCursor.setIdxBuffCount(0);

	return TRUE;
}


//------------------------------------------------------------------------
// 
// [2011/2/14 jjuiddong]
//------------------------------------------------------------------------
void CTerrainCursor::Render()
{
	m_InnerCursor.RenderLineStrip();
	m_OutterCursor.RenderLineStrip();
}


//------------------------------------------------------------------------
// 
// [2011/2/14 jjuiddong]
//------------------------------------------------------------------------
void CTerrainCursor::Update( int nElapseTick )
{

}


//------------------------------------------------------------------------
// mousePos : Screen 상의 마우스 위치
// [2011/2/14 jjuiddong]
//------------------------------------------------------------------------
void CTerrainCursor::UpdateCursor(POINT mousePos)
{
	if (!m_pTerrain) return;

	m_ScreenPos = mousePos;
	Vector3 pos;
	if (m_pTerrain->Pick(&Vector2((float)mousePos.x, (float)mousePos.y), &pos))
	{
		m_CursorPos = pos;
		_UpdateCursor(m_CursorPos);
	}
}


//------------------------------------------------------------------------
// 저장된 커서의 위치를 기준으로 커서 모양을 업데이트 한다.
// [2011/2/14 jjuiddong]
//------------------------------------------------------------------------
void CTerrainCursor::UpdateCursor()
{
	if (!m_pTerrain) return;
	_UpdateCursor(m_CursorPos);
}


//------------------------------------------------------------------------
// cursorPos : Terrain 상에서의 위치
// [2011/2/14 jjuiddong]
//------------------------------------------------------------------------
void CTerrainCursor::_UpdateCursor(Vector3 cursorPos)
{
	IDirect3DVertexBuffer9 *pOutterVtxBuf = m_OutterCursor.getVtxBuffer();
	IDirect3DVertexBuffer9 *pInnerVtxBuf = m_InnerCursor.getVtxBuffer();

	int index = 0;
	float angle = 0.f;
//	const float radius = m_Radius;// * m_Cell_Size * m_ColumnCellCountPerChunk;
	const float hoffset = 2.f;

	SVtxDiff *poutterV;
	SVtxDiff *pinnerV;
	pOutterVtxBuf->Lock(0, 0, (void**)&poutterV, 0);
	pInnerVtxBuf->Lock(0, 0, (void**)&pinnerV, 0);
	while (angle < MATH_PI * 2.f)
	{
		Vector3 outter(m_OutterRadius*cos(angle), 0.f, m_OutterRadius*sin(angle));
		outter += cursorPos;
		poutterV[ index].v = outter;
		poutterV[ index].v.y = m_pTerrain->GetHeight(outter.x, outter.z) + 0.2f;
		poutterV[ index].c = D3DXCOLOR( 0, 0, 1.f, 0 );

		Vector3 inner(m_InnerRadius*cos(angle), 0.f, m_InnerRadius*sin(angle));
		inner += cursorPos;
		pinnerV[ index].v = inner;
		pinnerV[ index].v.y = m_pTerrain->GetHeight(inner.x, inner.z) + 0.2f;
		pinnerV[ index].c = D3DXCOLOR( 1.f, 0, 0.f, 0 );

		index++;
		angle += 0.2f;
	}
	poutterV[ index] = poutterV[ 0];
	pinnerV[ index] = pinnerV[ 0];
	++index;

	pOutterVtxBuf->Unlock();
	pInnerVtxBuf->Unlock();

	m_OutterCursor.setVtxBuffCount(index-1);
	m_InnerCursor.setVtxBuffCount(index-1);
}


//------------------------------------------------------------------------
// 
// [2011/2/14 jjuiddong]
//------------------------------------------------------------------------
void CTerrainCursor::SetEraseMode(BOOL isErase)
{
	m_Flag = (isErase)? m_Flag | ERASE : (m_Flag & (DWORD)~ERASE);
}


//------------------------------------------------------------------------
// 
// [2011/2/14 jjuiddong]
//------------------------------------------------------------------------
void CTerrainCursor::SetMatchMode(BOOL isMatch)
{
	m_Flag = (isMatch)? m_Flag | MATCH : (m_Flag & (DWORD)~MATCH);
}


//------------------------------------------------------------------------
// 
// [2011/2/14 jjuiddong]
//------------------------------------------------------------------------
void CTerrainCursor::SetOutterRadius( float outterradius )
{
	m_OutterRadius = outterradius;
	if (outterradius < m_InnerRadius)
		m_InnerRadius = outterradius;
}


//------------------------------------------------------------------------
// 
// [2011/2/14 jjuiddong]
//------------------------------------------------------------------------
void CTerrainCursor::SetInnerRadius( float innerradius )
{
	m_InnerRadius = innerradius;
	if (innerradius > m_OutterRadius)
		m_OutterRadius = innerradius;
}


//------------------------------------------------------------------------
// 
// [2011/2/14 jjuiddong]
//------------------------------------------------------------------------
void CTerrainCursor::SetOutterAlpha( float outteralpha)
{
	m_OutterAlpha = outteralpha;
}


//------------------------------------------------------------------------
// 
// [2011/2/14 jjuiddong]
//------------------------------------------------------------------------
void CTerrainCursor::SetInnerAlpha( float innerralpha)
{
	m_InnerAlpha = innerralpha;
}


//------------------------------------------------------------------------
// 
// [2011/2/14 jjuiddong]
//------------------------------------------------------------------------
void CTerrainCursor::SetHeightIncrementOffset( float offset )
{
	m_Offset = offset;

}


//------------------------------------------------------------------------
// 
// [2011/2/14 jjuiddong]
//------------------------------------------------------------------------
void CTerrainCursor::SetTextureName( char *texturename )
{
	m_Load = TRUE;
	strcpy_s(m_TextureName, sizeof(m_TextureName), texturename);

}


//------------------------------------------------------------------------
// 
// [2011/2/14 jjuiddong]
//------------------------------------------------------------------------
void CTerrainCursor::Clear()
{
	m_Load = FALSE;
	m_Flag = 0;
	m_OutterRadius = 30.f;
	m_InnerRadius = 10.f;
	m_OutterAlpha = 0.f;
	m_InnerAlpha = 1.0f;
	m_Offset = 10.f;
	m_TextureName[ 0] = NULL;

}
