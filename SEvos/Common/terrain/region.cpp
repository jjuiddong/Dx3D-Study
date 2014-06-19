
#include "StdAfx.h"
#include "terrainCursor.h"
#include "../common/utility.h"

#include "region.h"


CRegion::CRegion() :
m_pvtxBuff(NULL)
, m_pidxBuff(NULL)
, m_pvtxBuff_Edge(NULL)

{
	ZeroMemory( &m_mtrl, sizeof( D3DMATERIAL9 ) );
	m_mtrl.Diffuse = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	m_mtrl.Ambient = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	m_mtrl.Specular = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );

}


CRegion::~CRegion()
{
	Clear();

}


//------------------------------------------------------------------------
// 
// [2011/2/9 jjuiddong]
//------------------------------------------------------------------------
void CRegion::Init(int posX, int posY, int width, int height, float cellsize)
{
	Clear();

	m_Visible = TRUE;
	m_Flag = VISIBLE;
	m_posX = posX;
	m_posY = posY;
	m_pvtxBuff = NULL;
	m_pidxBuff = NULL;

	// CreateVertex, CreatIndex 가 호출되기 전에 초기화 되어야 한다.
	m_columnCellCount = width / (int)cellsize;
	m_rowCellCount = height / (int)cellsize;
	m_columnVtxCount = m_columnCellCount + 1;
	m_rowVtxCount = m_rowCellCount + 1;
	m_cellSize = cellsize;

	CreateVertexBuffer(posX, posY, width, height, cellsize);
	CreateIndexBuffer();
	CreateEdge();
}


//------------------------------------------------------------------------
// 
// [2011/2/10 jjuiddong]
//------------------------------------------------------------------------
void CRegion::CreateVertexBuffer(int posX, int posY, int width, int height, float cellSize)
{
	const int vtxsize = (m_columnVtxCount) * (m_rowVtxCount);
	const float startx = (float)posX;
	const float starty = (float)posY;
	const float endx = (float)startx + (float)width;
	const float endy = (float)starty - (float)height;

	const float uCoordIncrementSize = 1.0f / (float)m_columnCellCount;
	const float vCoordIncrementSize = 1.0f / (float)m_rowCellCount;

	// create vertex buffer
	g_pDevice->CreateVertexBuffer( vtxsize*sizeof(SVtxNormTex), 0, SVtxNormTex::FVF, 
		D3DPOOL_MANAGED, &m_pvtxBuff, NULL );
	SVtxNormTex *pv = NULL;
	m_pvtxBuff->Lock( 0, 0, (void**)&pv, 0 );
	int i=0;
	for( float y=starty; y >= endy; y -= cellSize, ++i )
	{
		int k=0;
		for( float x=startx; x <= endx; x += cellSize, ++k )
		{
			int index = (i * m_columnVtxCount) + k;
			pv[ index] = SVtxNormTex( Vector3(x, 0.1f, y), Vector3(0,1,0),
				((float)k*uCoordIncrementSize), (float)i*vCoordIncrementSize );
		}
	}

	m_pvtxBuff->Unlock();
}


//------------------------------------------------------------------------
// 
// [2011/2/10 jjuiddong]
//------------------------------------------------------------------------
void CRegion::CreateIndexBuffer()
{
	const int trisize = (m_columnCellCount * m_rowCellCount) * 2;

	// create index buffer
	g_pDevice->CreateIndexBuffer( trisize*3*sizeof(WORD), D3DUSAGE_WRITEONLY, 
		D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pidxBuff, NULL );
	WORD *pi = NULL;
	m_pidxBuff->Lock( 0, 0, (void**)&pi, 0 );

	int baseIndex = 0;
	for( int i=0; i < m_rowCellCount; ++i )
	{
		for( int k=0; k < m_columnCellCount; ++k )
		{
			pi[ baseIndex]     = (  i   * m_columnVtxCount) + k;
			pi[ baseIndex + 1] = (  i   * m_columnVtxCount) + k + 1;
			pi[ baseIndex + 2] = ((i+1) * m_columnVtxCount) + k;

			pi[ baseIndex + 3] = ((i+1) * m_columnVtxCount) + k;
			pi[ baseIndex + 4] = (  i   * m_columnVtxCount) + k + 1;
			pi[ baseIndex + 5] = ((i+1) * m_columnVtxCount) + k + 1;

			// next quad
			baseIndex += 6;
		}
	}
	m_pidxBuff->Unlock();
}


//------------------------------------------------------------------------
// 외곽선 버텍스를 생성하고 초기화 한다.
// [2011/2/15 jjuiddong]
//------------------------------------------------------------------------
void CRegion::CreateEdge()
{
	const int edgeVtxCount = GetEdgeVertexCount();
	g_pDevice->CreateVertexBuffer( edgeVtxCount*sizeof(SVtxDiff), 0, SVtxDiff::FVF, D3DPOOL_MANAGED, &m_pvtxBuff_Edge, NULL );

	UpdateEdge();
}


//-----------------------------------------------------------------------------//
// 청크의 외곽선을 청크 정점값에 맞게 다시 업데이트 한다.
// pchunk: 업데이트할 청크
//-----------------------------------------------------------------------------//
void CRegion::UpdateEdge()
{
	if (!m_pvtxBuff_Edge) return;

	const int vtxSize = GetVertexCount();

	SVtxDiff *pvEdge = NULL;
	m_pvtxBuff_Edge->Lock( 0, 0, (void**)&pvEdge, 0 );

	SVtxNormTex *pv = NULL;
	m_pvtxBuff->Lock( 0, 0, (void**)&pv, 0 );

	// ------1-------
	// |			|
	// |			|
	// 4			2
	// |			|
	// |			|
	// ------3-------

	int index = 0;
	int srcIndex = 0;
	// 1
	for (int i=0; i < m_rowVtxCount; ++i)
		pvEdge[ index++].v = pv[ srcIndex++].v;

	// 2
	--srcIndex;
	for (int i=0; i < m_columnVtxCount-1; ++i)
	{
		srcIndex += m_rowVtxCount;
		pvEdge[ index++].v = pv[ srcIndex].v;
	}

	// 3
	for (int i=0; i < m_rowVtxCount-1; ++i)
		pvEdge[ index++].v = pv[ --srcIndex].v;

	// 4
	for (int i=1; i < m_columnVtxCount-1; ++i)
	{
		srcIndex -= m_rowVtxCount;
		pvEdge[ index++].v = pv[ srcIndex].v;
	}

	srcIndex -= m_rowVtxCount;
	pvEdge[ index++].v = pv[ srcIndex].v;

	const int edgeVtxCount = GetEdgeVertexCount();
	for( int q=0; q < edgeVtxCount; ++q )
		pvEdge[ q].v.y += 0.1f;

	m_pvtxBuff->Unlock();
	m_pvtxBuff_Edge->Unlock();
}


//------------------------------------------------------------------------
// 외곽선 버텍스 갯수를 리턴한다.
// [2011/2/15 jjuiddong]
//------------------------------------------------------------------------
int CRegion::GetEdgeVertexCount()
{
	return ((m_rowVtxCount * 2) + (m_columnVtxCount-2)*2) + 1;
}



//------------------------------------------------------------------------
// 
// [2011/2/9 jjuiddong]
//------------------------------------------------------------------------
void CRegion::Render()
{
	// 아직 아무일 없음
}


//------------------------------------------------------------------------
// 
// [2011/2/9 jjuiddong]
//------------------------------------------------------------------------
void CRegion::RenderEdge()
{
	// 청크 외곽선 출력
	if( !m_Visible ) return;
	if (!m_pvtxBuff_Edge) return;

	Matrix44 matIdentity;
	matIdentity.SetIdentity();
	g_pDevice->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)&matIdentity );

	g_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	const int edgeVtxCount = GetEdgeVertexCount();

	DWORD edge_color = D3DXCOLOR( 1.f, 0, 1.f, 0 );
	DWORD draw_color = D3DXCOLOR( 0, 0, 1.f, 0 );

	SVtxDiff *pvd = NULL;
	m_pvtxBuff_Edge->Lock( 0, 0, (void**)&pvd, 0 );
	for( int i=0; i < edgeVtxCount; ++i )
		pvd[ i].c = edge_color;
	m_pvtxBuff_Edge->Unlock();

	g_pDevice->SetTexture( 0, NULL );
	g_pDevice->SetTexture( 1, NULL );

	g_pDevice->SetStreamSource( 0, m_pvtxBuff_Edge, 0, sizeof(SVtxDiff) );
	g_pDevice->SetFVF( SVtxDiff::FVF );
	g_pDevice->DrawPrimitive( D3DPT_LINESTRIP, 0, edgeVtxCount-1 );

	if( (FOCUS1 & m_Flag) )
	{
		SVtxDiff *pvd = NULL;
		m_pvtxBuff_Edge->Lock( 0, 0, (void**)&pvd, 0 );
		for( int i=0; i < edgeVtxCount; ++i )
			pvd[ i].c = draw_color;
		m_pvtxBuff_Edge->Unlock();

		g_pDevice->SetTexture( 0, NULL );
		g_pDevice->SetTexture( 1, NULL );

		g_pDevice->SetStreamSource( 0, m_pvtxBuff_Edge, 0, sizeof(SVtxDiff) );
		g_pDevice->SetFVF( SVtxDiff::FVF );
		g_pDevice->DrawPrimitive( D3DPT_LINESTRIP, 0, edgeVtxCount-1 );
	}

	g_pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
}


//------------------------------------------------------------------------
// 
// [2011/2/15 jjuiddong]
//------------------------------------------------------------------------
void CRegion::RenderNormal()
{
	g_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	SVtxNormTex *pv;
	m_pvtxBuff->Lock(0, 0, (void**)&pv, 0);
	const int vtxsize = GetVertexCount();
	for (int i=0; i < vtxsize; ++i)
	{
		const int cross_size = 2;
		SVtxDiff crossline[ cross_size];
		// normal
		crossline[ 0].v = pv[ i].v;
		crossline[ 1].v = pv[ i].v + pv[ i].n * 5.f;
		crossline[ 0].c = D3DXCOLOR( 1, 1, 1, 0 );
		crossline[ 1].c = D3DXCOLOR( 1, 1, 1, 0 );
		g_pDevice->SetFVF( SVtxDiff::FVF );
		g_pDevice->DrawPrimitiveUP( D3DPT_LINELIST, cross_size/2, crossline, sizeof(SVtxDiff) );
	}

	m_pvtxBuff->Unlock();

	g_pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
}


//------------------------------------------------------------------------
// 
// [2011/2/9 jjuiddong]
//------------------------------------------------------------------------
void CRegion::Clear()
{
	m_Visible = FALSE;
	SAFE_RELEASE( m_pidxBuff );
	SAFE_RELEASE( m_pvtxBuff );
	SAFE_RELEASE( m_pvtxBuff_Edge );
}



//-----------------------------------------------------------------------------//
// map height 배열의 row, col 위치의값을 리턴한다.
//-----------------------------------------------------------------------------//
float CRegion::GetHeightMapEntry( int nRow, int nCol )
{
	const int chunk_vtxsize = (m_columnVtxCount) * (m_rowVtxCount);

	if( 0 > nRow || 0 > nCol )
		return 0.f;
	if( chunk_vtxsize <= (nRow * m_rowVtxCount + nCol) ) 
		return 0.f;

	SVtxNormTex *pv = NULL;
	m_pvtxBuff->Lock( 0, 0, (void**)&pv, 0 );
	float h = pv[ nRow * m_rowVtxCount + nCol].v.y;
	m_pvtxBuff->Unlock();
	return h;
}


//------------------------------------------------------------------------
// row, col위치에서 법선벡터를 구하기 위해서 주변 삼각형 6개의 법선 벡터를
// 구하고 그 평균을 리턴한다.
// [2011/2/15 jjuiddong]
//------------------------------------------------------------------------
Vector3 CRegion::GetNormalVector(int vtxRow, int vtxCol )
{

	// x, y
	//        1     2
	//        |    /
	//        |  /
	//        |/
	//0 ---------------3
	//       /|
	//     /  |
	//   /    |
	//  5     4
	//
	int table[][2] = { {-1,-1}, {0,-1}, {1,1}, {1,0}, {0,1}, {-1,1} };
	int indexBuff[18] = 
	{
		(vtxCol) + (vtxRow) * m_rowVtxCount,
		(vtxCol+table[0][0]) + (vtxRow+table[0][1]) * m_rowVtxCount,
		(vtxCol+table[1][0]) + (vtxRow+table[1][1]) * m_rowVtxCount,

		(vtxCol) + (vtxRow) * m_rowVtxCount,
		(vtxCol+table[1][0]) + (vtxRow+table[1][1]) * m_rowVtxCount,
		(vtxCol+table[2][0]) + (vtxRow+table[2][1]) * m_rowVtxCount,

		(vtxCol) + (vtxRow) * m_rowVtxCount,
		(vtxCol+table[2][0]) + (vtxRow+table[2][1]) * m_rowVtxCount,
		(vtxCol+table[3][0]) + (vtxRow+table[3][1]) * m_rowVtxCount,

		(vtxCol) + (vtxRow) * m_rowVtxCount,
		(vtxCol+table[3][0]) + (vtxRow+table[3][1]) * m_rowVtxCount,
		(vtxCol+table[4][0]) + (vtxRow+table[4][1]) * m_rowVtxCount,

		(vtxCol) + (vtxRow) * m_rowVtxCount,
		(vtxCol+table[4][0]) + (vtxRow+table[4][1]) * m_rowVtxCount,
		(vtxCol+table[5][0]) + (vtxRow+table[5][1]) * m_rowVtxCount,

		(vtxCol) + (vtxRow) * m_rowVtxCount,
		(vtxCol+table[5][0]) + (vtxRow+table[5][1]) * m_rowVtxCount,
		(vtxCol+table[0][0]) + (vtxRow+table[0][1]) * m_rowVtxCount,
	};

	Vector3 normalTable[6];
	for (int k=0; k < 6; ++k)
		normalTable[ k] = Vector3(0,1,0);

	Vector3 n(0,0,0);
	for (int i=0; i < 18; i+=3)
	{
		n += _GetNormalVector( indexBuff[ i], indexBuff[ i+1], indexBuff[ i+2] );
	}

	n /= 6;
	n.Normalize();
	return n;
}


//------------------------------------------------------------------------
// col : x축 vertex index
// row : y축 vertex index
// pv : 해당 col,row 의 버텍스 정보가 채워져서 리턴된다.
// col, row 축에 버텍스가 없다면 FALSE를 리턴한다.
// [2011/2/15 jjuiddong]
//------------------------------------------------------------------------
BOOL CRegion::GetVertex(int row, int col, Vector3 *poutV)
{
	if (0 > row || 0 > col) return FALSE;
	if (GetVertexCount() <= (row*m_rowVtxCount + col)) return FALSE;

	SVtxNormTex *pv = NULL;
	m_pvtxBuff->Lock( 0, 0, (void**)&pv, 0 );
	*poutV = pv[ row*m_rowVtxCount + col].v;
	m_pvtxBuff->Unlock();

	return TRUE;
}


//------------------------------------------------------------------------
// vertextBuffer에서 vtxIndex 를 리턴한다.
// [2011/2/18 jjuiddong]
//------------------------------------------------------------------------
BOOL CRegion::GetVertex(int vtxIndex, Vector3 *poutV)
{
	if (0 > vtxIndex) return FALSE;
	if (GetVertexCount() <= vtxIndex) return FALSE;

	SVtxNormTex *pv = NULL;
	m_pvtxBuff->Lock( 0, 0, (void**)&pv, 0 );
	*poutV = pv[ vtxIndex].v;
	m_pvtxBuff->Unlock();

	return TRUE;
}


//------------------------------------------------------------------------
// 
// [2011/2/9 jjuiddong]
//------------------------------------------------------------------------
void CRegion::SetFocus(BOOL bFocus)
{
	if (bFocus)
	{
		m_Flag |= FOCUS1;
	}
	else
	{
		m_Flag = m_Flag & ~(DWORD)(FOCUS1 | FOCUS2);
	}
}

//------------------------------------------------------------------------
// 
// [2011/2/11 jjuiddong]
//------------------------------------------------------------------------
BOOL CRegion::IsFocus()
{
	return FOCUS1 & m_Flag;
}


//------------------------------------------------------------------------
// 브러쉬가 Region안에 들어왔다면 TRUE를 리턴한다.
// [2011/2/10 jjuiddong]
//------------------------------------------------------------------------
BOOL CRegion::IsInRegion( CTerrainCursor *pcursor, Vector3 pos )
{
	const int x = (int)((float)m_columnCellCount * m_cellSize / 2.f);
	const int y = (int)((float)m_rowCellCount * m_cellSize / 2.f);
	const float chunk_radius = (float)sqrt((float)(x*x + y*y));

	const int width = (int)(m_columnCellCount * m_cellSize);
	const int height = (int)(m_rowCellCount * m_cellSize);
	Vector2 centerPos((float)(m_posX + width/2), (float)(m_posY - height/2));
	Vector2 brushPos(pos.x, pos.z);

	const float dist = brushPos.Distance(centerPos);
	const float brushRadius = pcursor->GetOutterRadius(); // pbrush->radius * width;

	return dist < brushRadius + chunk_radius;
}


//-----------------------------------------------------------------------------//
// pChunk를 기준으로 u,v 좌표에 브러쉬가 닿는지를 알려주는 함수다. 
// 브러쉬가 닿는다면 true를 리턴한다.
// pChunck : 선택된 청크
// u/v : 청크를 기준으로한 u/v 좌표
// pbrush : 브러쉬 정보
//-----------------------------------------------------------------------------//
BOOL CRegion::isInDrawBrush(float u, float v, CTerrainCursor *pcursor)
{
	const int LEFT_TOP = 0;
	const int RIGHT_TOP = 1;
	const int RIGHT_BOTTOM = 2;
	const int LEFT_BOTTOM = 3;
	const int LEFT = 0;
	const int TOP = 1;
	const int RIGHT = 2;
	const int BOTTOM = 3;

	int type = 0; // 0=leftTop, 1=rightTop, 2=rightBottom, 3=leftBottom
	if (u > 0.f && v > 0.f)
		type = RIGHT_BOTTOM; // rightBottom
	else if (u > 0.f && v < 0.f)
		type = RIGHT_TOP; // rightTop
	else if (u < 0.f && v > 0.f)
		type = LEFT_BOTTOM; // leftBottom
	else if (u < 0.f && v < 0.f)
		type = LEFT_TOP; // leftTop

	// 청크 꼭지점을 향한 방향벡터를 계산한다.
	Vector3 vtxNormal(1,0,1);
	switch(type) {
	case LEFT_TOP:		vtxNormal = Vector3(-1.f, 0, 1.f); break;
	case RIGHT_TOP:		vtxNormal = Vector3(1.f, 0, 1.f); break;
	case RIGHT_BOTTOM:	vtxNormal = Vector3(1.f, 0, -1.f); break;
	case LEFT_BOTTOM:	vtxNormal = Vector3(-1.f, 0, -1.f); break;
	}
	vtxNormal.Normalize();

	Vector3 dir(u, 0, -v); // uv좌표와 실제x,z 좌표에서 v/z 좌표는 반대가 되기때문에 -를 붙였다.
	const float length = dir.Length();
	dir.Normalize();
	Vector3 cross = dir.CrossProduct(vtxNormal);
	const BOOL leftSwing = cross.y < 0.f; // 왼쪽으로 감김 (오른나사의 법칙)

	// 청크의 어느 면과 맞닿는지 계산한다.
	int edgeType = 0; // 0=left, 1=top, 2=right, 3=bottom
	switch(type) {
	case LEFT_TOP:		edgeType = (leftSwing)? TOP : LEFT; break;
	case RIGHT_TOP:		edgeType = (leftSwing)? RIGHT : TOP; break;
	case RIGHT_BOTTOM:	edgeType = (leftSwing)? BOTTOM : RIGHT; break;
	case LEFT_BOTTOM:	edgeType = (leftSwing)? LEFT : BOTTOM; break;
	}

	// 청크면의 법선벡터를 구한다. 방향은 청크의 바깥쪽을 향한다.
	Vector3 normal(0,0,1);
	switch(edgeType) {
	case LEFT:		normal = Vector3(-1.f,0,0); break;
	case TOP:		normal = Vector3(0,0,1.f); break;
	case RIGHT:		normal = Vector3(1.f,0,0); break;
	case BOTTOM:	normal = Vector3(0,0,-1.f); break;
	}

	float cosTheta = fabs(dir.DotProduct(normal));
	if (cosTheta <= 0.f) cosTheta = 0.00001f;
	const float len = 0.5f / cosTheta;

	const BOOL inChunck = ((length-len) < pcursor->GetOutterRadius());
	return inChunck;
}


//-----------------------------------------------------------------------------//
// x, y 좌표의 높이를 얻는다.
// x,y : terrain 상에서의 좌표``
//-----------------------------------------------------------------------------//
float CRegion::GetHeight( float x, float y )
{
	float newX = x + (GetRegionWidth() / 2.0f);
	float newY = GetRegionHeight() - (y + (GetRegionHeight() / 2.0f));

	newX /= m_cellSize;
	newY /= m_cellSize;

	float col = ::floorf( newX );
	float row = ::floorf( newY );

	//  A   B
	//  *---*
	//  | / |
	//  *---*
	//  C   D
	float A = GetHeightMapEntry( (int)row, (int)col );
	float B = GetHeightMapEntry( (int)row, (int)col+1 );
	float C = GetHeightMapEntry( (int)row+1, (int)col );
	float D = GetHeightMapEntry( (int)row+1, (int)col+1 );

	float dx = newX - col;
	float dy = newY - row;

	float height = 0.0f;
	if( dy < 1.0f - dx )  // upper triangle ABC
	{
		float uy = B - A; // A->B
		float vy = C - A; // A->C
		height = A + Lerp(0.0f, uy, dx) + Lerp(0.0f, vy, dy);
	}
	else // lower triangle DCB
	{
		float uy = C - D; // D->C
		float vy = B - D; // D->B
		height = D + Lerp(0.0f, uy, 1.0f - dx) + Lerp(0.0f, vy, 1.0f - dy);
	}

	return height;
}


//-----------------------------------------------------------------------------//
// Screen상의 2d좌표 pPos에서 3d Map좌표를 얻어온다.
//-----------------------------------------------------------------------------//
BOOL CRegion::Pick( Vector2 *pPos, Vector3 *pvTarget )
{
	if( !m_pvtxBuff ) return FALSE;

	Vector3 vOrig, vDir;
	d3d::GetPickRay( (int)pPos->x, (int)pPos->y, &vOrig, &vDir );

	*pvTarget = Vector3(0,0,0);
	int nSize = GetTriangleCount() * 3;

	SVtxNormTex *pv = NULL;
	WORD *pi = NULL;
	m_pvtxBuff->Lock( 0, 0, (void**)&pv, 0 );
	m_pidxBuff->Lock( 0, 0, (void**)&pi, 0 );
	for( int i=0; i < nSize; i+=3 )
	{
		Vector3 v1 = pv[ pi[ i+0]].v;
		Vector3 v2 = pv[ pi[ i+1]].v;
		Vector3 v3 = pv[ pi[ i+2]].v;

		Triangle tri( v1, v2, v3 );
		Plane p( v1, v2, v3 );

		float a = vDir.DotProduct( p.N );
		if( a >= 0 ) continue;

		float t;
		if( tri.Intersect(vOrig, vDir, &t ) )
		{
			if( 0 == pvTarget->x && 0 == pvTarget->y && 0 == pvTarget->z )
			{
				*pvTarget = vOrig + vDir * t;
			}
			else
			{
				Vector3 v = vOrig + vDir * t;
				if( vOrig.LengthRoughly(v) < vOrig.LengthRoughly(*pvTarget) )
					*pvTarget = v;
			}
		}
	}

	m_pvtxBuff->Unlock();
	m_pidxBuff->Unlock();

	if( 0 == pvTarget->x && 0 == pvTarget->y && 0 == pvTarget->z )
		return FALSE;

	return TRUE;
}


//------------------------------------------------------------------------
// vtx1, vtx2, vtx3 의 법선벡터를 리턴한다.
// [2011/2/18 jjuiddong]
//------------------------------------------------------------------------
Vector3 CRegion::_GetNormalVector(int vtxIdx1, int vtxIdx2, int vtxIdx3 )
{
	Vector3 v1, v2, v3;
	const BOOL isExist1 = GetVertex( vtxIdx1, &v1 );
	const BOOL isExist2 = GetVertex( vtxIdx2, &v2 );
	const BOOL isExist3 = GetVertex( vtxIdx3, &v3 );

	if (isExist1 && isExist2 && isExist3)
	{
		Vector3 v12 = v2 - v1;
		Vector3 v13 = v3 - v1;
		v12.Normalize();
		v13.Normalize();	
		Vector3 n = v13.CrossProduct(v12);
		return -n;
	}
	else
	{
		return Vector3(0,1,0);
	}
}


//------------------------------------------------------------------------
// 매인지형의 normal 값을 계산해서 업데이트한다.
// [2011/2/18 jjuiddong]
//------------------------------------------------------------------------
void CRegion::UpdateNormals()
{
	const int vtxsize = GetVertexCount();
	SVtxNormTex *pv = NULL;
	m_pvtxBuff->Lock( 0, 0, (void**)&pv, 0 );
	for (int i=0; i < vtxsize; ++i)
	{
		int row = i / m_rowVtxCount;
		int col = i % m_rowVtxCount;
		pv[ i].n = GetNormalVector(row, col);
	}
	m_pvtxBuff->Unlock();
}
