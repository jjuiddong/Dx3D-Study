
#include "stdafx.h"
#include "quadtree.h"
#include "../common/utility.h"

#define _USE_INDEX16


//-----------------------------------------------------------------------------//
// 높이,너비 분할 갯수 설정
//-----------------------------------------------------------------------------//
CQuadTree::CQuadTree( int cx, int cy )
{
	int		i;
	m_pParent = NULL;
	m_nCenter = 0;
	for( i = 0 ; i < 4 ; i++ )
	{
		m_pChild[i] = NULL;
		m_pNeighbor[i] = NULL;
	}
	m_nCorner[ CORNER_TL] = 0;
	m_nCorner[ CORNER_TR] = cx - 1;
	m_nCorner[ CORNER_BL] = cx * ( cy - 1 );
	m_nCorner[ CORNER_BR] = cx * cy - 1;

	m_nCenter			  = ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] + 
						  	  m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 4;
}

CQuadTree::CQuadTree( CQuadTree* pParent )
{
	int		i;
	m_pParent = pParent;
	m_nCenter = 0;
	for( i = 0 ; i < 4 ; i++ )
	{
		m_pChild[i] = NULL;
		m_pNeighbor[i] = NULL;
		m_nCorner[i] = 0;
	}
}

CQuadTree::~CQuadTree()
{
	Destroy();
}


//-----------------------------------------------------------------------------//
/// 메모리에서 쿼드트리를 삭제한다.
//-----------------------------------------------------------------------------//
void CQuadTree::Destroy()
{
	for( int i = 0 ; i < 4 ; i++ ) 
		SAFE_DELETE( m_pChild[i] );
}


//-----------------------------------------------------------------------------//
/// 4개의 코너값을 셋팅한다.
//-----------------------------------------------------------------------------//
BOOL CQuadTree::SetCorners( int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR )
{
	m_nCorner[CORNER_TL] = nCornerTL;
	m_nCorner[CORNER_TR] = nCornerTR;
	m_nCorner[CORNER_BL] = nCornerBL;
	m_nCorner[CORNER_BR] = nCornerBR;
	m_nCenter			 = ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] + 
							 m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 4;
	return TRUE;
}


//-----------------------------------------------------------------------------//
/// 자식 노드를 추가한다.
//-----------------------------------------------------------------------------//
CQuadTree* CQuadTree::AddChild( int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR )
{
	CQuadTree*	pChild;

	pChild = new CQuadTree( this );
	pChild->SetCorners( nCornerTL, nCornerTR, nCornerBL, nCornerBR );

	return pChild;
}


//-----------------------------------------------------------------------------//
/// Quadtree를 4개의 자식 트리로 부분분할(subdivide)한다.
//-----------------------------------------------------------------------------//
BOOL CQuadTree::SubDivide()
{
	int		nTopEdgeCenter;
	int		nBottomEdgeCenter;
	int		nLeftEdgeCenter;
	int		nRightEdgeCenter;
	int		nCentralPoint;

	// 상단변 가운데
	nTopEdgeCenter		= ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] ) / 2;
	// 하단변 가운데 
	nBottomEdgeCenter	= ( m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 2;
	// 좌측변 가운데
	nLeftEdgeCenter		= ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_BL] ) / 2;
	// 우측변 가운데
	nRightEdgeCenter	= ( m_nCorner[CORNER_TR] + m_nCorner[CORNER_BR] ) / 2;
	// 한가운데
	nCentralPoint		= ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] + 
							m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 4;

	// 더이상 분할이 불가능한가? 그렇다면 SubDivide() 종료
	if( abs(m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL]) <= 1 )
	{
		return FALSE;
	}

	// 4개의 자식노드 추가
	m_pChild[CORNER_TL] = AddChild( m_nCorner[CORNER_TL], nTopEdgeCenter, nLeftEdgeCenter, nCentralPoint );
	m_pChild[CORNER_TR] = AddChild( nTopEdgeCenter, m_nCorner[CORNER_TR], nCentralPoint, nRightEdgeCenter );
	m_pChild[CORNER_BL] = AddChild( nLeftEdgeCenter, nCentralPoint, m_nCorner[CORNER_BL], nBottomEdgeCenter );
	m_pChild[CORNER_BR] = AddChild( nCentralPoint, nRightEdgeCenter, nBottomEdgeCenter, m_nCorner[CORNER_BR] );

	return TRUE;
}


//-----------------------------------------------------------------------------//
/// 출력할 폴리곤의 인덱스를 생성한다.
//-----------------------------------------------------------------------------//
int	CQuadTree::GenTriIndex( int nTris, LPVOID pIndex, SVtxNormTex* pHeightMap, float fLODRatio )
{
	// 컬링된 노드라면 그냥 리턴
	if( m_bCulled )
	{
		m_bCulled = FALSE;
		return nTris;
	}

#ifdef _USE_INDEX16
	LPWORD p = ((LPWORD)pIndex) + nTris * 3;
#else
	LPDWORD p = ((LPDWORD)pIndex) + nTris * 3;
#endif

	if( IsVisible(pHeightMap, fLODRatio) )
	{
		// 만약 최하위 노드라면 부분분할(subdivide)이 불가능하므로 그냥 출력하고 리턴한다.
		if( abs(m_nCorner[CORNER_TR]-m_nCorner[CORNER_TL]) <= 1 )
		{
			// 좌측상단 삼각형
			*p++ = m_nCorner[0];
			*p++ = m_nCorner[1];
			*p++ = m_nCorner[2];
			nTris++;
			// 우측하단 삼각형
			*p++ = m_nCorner[2];
			*p++ = m_nCorner[1];
			*p++ = m_nCorner[3];
			nTris++;
			return nTris;
		}

		BOOL b[4] = {TRUE, TRUE, TRUE, TRUE };
		if( m_pNeighbor[EDGE_UP] ) b[EDGE_UP] = m_pNeighbor[EDGE_UP]->IsVisible( pHeightMap, fLODRatio );
		if( m_pNeighbor[EDGE_DN] ) b[EDGE_DN] = m_pNeighbor[EDGE_DN]->IsVisible( pHeightMap, fLODRatio );
		if( m_pNeighbor[EDGE_LT] ) b[EDGE_LT] = m_pNeighbor[EDGE_LT]->IsVisible( pHeightMap, fLODRatio );
		if( m_pNeighbor[EDGE_RT] ) b[EDGE_RT] = m_pNeighbor[EDGE_RT]->IsVisible( pHeightMap, fLODRatio );

		// 이웃노드들이 모두다 출력가능하다면 현재노드와 이웃노드들이 같은 LOD이므로 
		// 부분분할이 필요없다.
		if( b[EDGE_UP] && b[EDGE_DN] && b[EDGE_LT] && b[EDGE_RT] )
		{
			*p++ = m_nCorner[0];
			*p++ = m_nCorner[1];
			*p++ = m_nCorner[2];
			nTris++;
			*p++ = m_nCorner[2];
			*p++ = m_nCorner[1];
			*p++ = m_nCorner[3];
			nTris++;
			return nTris;
		}
/**/
		int n;
		if( !b[EDGE_UP] ) // 상단 부분분할이 필요한가?
		{
			n = ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] ) / 2;
			*p++ = m_nCenter; *p++ = m_nCorner[CORNER_TL]; *p++ = n; nTris++;
			*p++ = m_nCenter; *p++ = n; *p++ = m_nCorner[CORNER_TR]; nTris++;
		}
		else	// 상단 부분분할이 필요없을 경우
		{ *p++ = m_nCenter; *p++ = m_nCorner[CORNER_TL]; *p++ = m_nCorner[CORNER_TR]; nTris++; }

		if( !b[EDGE_DN] ) // 하단 부분분할이 필요한가?
		{
			n = ( m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 2;
			*p++ = m_nCenter; *p++ = m_nCorner[CORNER_BR]; *p++ = n; nTris++;
			*p++ = m_nCenter; *p++ = n; *p++ = m_nCorner[CORNER_BL]; nTris++;
		}
		else	// 하단 부분분할이 필요없을 경우
		{ *p++ = m_nCenter; *p++ = m_nCorner[CORNER_BR]; *p++ = m_nCorner[CORNER_BL]; nTris++; }

		if( !b[EDGE_LT] ) // 좌측 부분분할이 필요한가?
		{
			n = ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_BL] ) / 2;
			*p++ = m_nCenter; *p++ = m_nCorner[CORNER_BL]; *p++ = n; nTris++;
			*p++ = m_nCenter; *p++ = n; *p++ = m_nCorner[CORNER_TL]; nTris++;
		}
		else	// 좌측 부분분할이 필요없을 경우
		{ *p++ = m_nCenter; *p++ = m_nCorner[CORNER_BL]; *p++ = m_nCorner[CORNER_TL]; nTris++; }

		if( !b[EDGE_RT] ) // 우측 부분분할이 필요한가?
		{
			n = ( m_nCorner[CORNER_TR] + m_nCorner[CORNER_BR] ) / 2;
			*p++ = m_nCenter; *p++ = m_nCorner[CORNER_TR]; *p++ = n; nTris++;
			*p++ = m_nCenter; *p++ = n; *p++ = m_nCorner[CORNER_BR]; nTris++;
		}
		else	// 우측 부분분할이 필요없을 경우
		{ *p++ = m_nCenter; *p++ = m_nCorner[CORNER_TR]; *p++ = m_nCorner[CORNER_BR]; nTris++; }
/**/
		
		return nTris;	// 이 노드 아래의 자식노드는 탐색할 필요없으므로 리턴!
	}

	if( m_pChild[CORNER_TL] ) nTris = m_pChild[CORNER_TL]->GenTriIndex( nTris, pIndex, pHeightMap, fLODRatio );
	if( m_pChild[CORNER_TR] ) nTris = m_pChild[CORNER_TR]->GenTriIndex( nTris, pIndex, pHeightMap, fLODRatio );
	if( m_pChild[CORNER_BL] ) nTris = m_pChild[CORNER_BL]->GenTriIndex( nTris, pIndex, pHeightMap, fLODRatio );
	if( m_pChild[CORNER_BR] ) nTris = m_pChild[CORNER_BR]->GenTriIndex( nTris, pIndex, pHeightMap, fLODRatio );

	return nTris;
}


//-----------------------------------------------------------------------------//
// QuadTree를 구축한다.
//-----------------------------------------------------------------------------//
BOOL CQuadTree::Build( SVtxNormTex* pHeightMap )
{
	BuildQuadTree( pHeightMap );
	BuildNeighborNode( this, pHeightMap, abs(m_nCorner[CORNER_TR]-m_nCorner[CORNER_TL])+1 );

	return TRUE;
}


//-----------------------------------------------------------------------------//
//	삼각형의 인덱스를 만들고, 출력할 삼각형의 개수를 반환한다.
//-----------------------------------------------------------------------------//
int	CQuadTree::GenerateIndex( LPVOID pIndex, SVtxNormTex* pHeightMap, CFrustum* pFrustum, float fLODRatio )
{
	FrustumCull( pHeightMap, pFrustum );
	return GenTriIndex( 0, pIndex, pHeightMap, fLODRatio );
}


//-----------------------------------------------------------------------------//
// pHeightMap이 pFrustum안에 포함되는지 검사한다.
//-----------------------------------------------------------------------------//
CQuadTree::QuadLocation CQuadTree::IsInFrustum( SVtxNormTex *pHeightMap, CFrustum* pFrustum )
{
	BOOL	b[4];
	BOOL	bInSphere;

	// 경계구안에 있는가?
//	if( m_fRadius == 0.0f ) g_pLog->Log( "Index:[%d], Radius:[%f]",m_nCenter, m_fRadius );
	bInSphere = pFrustum->IsInSphere( &pHeightMap[ m_nCenter].v, m_fRadius );
	if( !bInSphere ) return FRUSTUM_OUT;	// 경계구 안에 없으면 점단위의 프러스텀 테스트 생략

	// 쿼드트리의 4군데 경계 프러스텀 테스트
	b[0] = pFrustum->IsIn( &pHeightMap[ m_nCorner[0]].v );
	b[1] = pFrustum->IsIn( &pHeightMap[ m_nCorner[1]].v );
	b[2] = pFrustum->IsIn( &pHeightMap[ m_nCorner[2]].v );
	b[3] = pFrustum->IsIn( &pHeightMap[ m_nCorner[3]].v );

	// 4개모두 프러스텀 안에 있음
	if( (b[0] + b[1] + b[2] + b[3]) == 4 ) return FRUSTUM_COMPLETELY_IN;

	// 일부분이 프러스텀에 있는 경우
	return FRUSTUM_PARTIALLY_IN;
}


//-----------------------------------------------------------------------------//
// IsInFrustum()함수의 결과에 따라 프러스텀 컬링 수행
//-----------------------------------------------------------------------------//
void CQuadTree::FrustumCull( SVtxNormTex *pHeightMap, CFrustum* pFrustum )
{
	QuadLocation ret;

	ret = IsInFrustum( pHeightMap, pFrustum );
	switch( ret )
	{
		case FRUSTUM_COMPLETELY_IN :	// 프러스텀에 완전포함, 하위노드 검색 필요없음
			m_bCulled = FALSE;
			return;
		case FRUSTUM_PARTIALLY_IN :		// 프러스텀에 일부포함, 하위노드 검색 필요함
			m_bCulled = FALSE;
			break;
		case FRUSTUM_OUT :				// 프러스텀에서 완전벗어남, 하위노드 검색 필요없음
			m_bCulled = TRUE;
			return;
	}
	if( m_pChild[0] ) m_pChild[0]->FrustumCull( pHeightMap, pFrustum );
	if( m_pChild[1] ) m_pChild[1]->FrustumCull( pHeightMap, pFrustum );
	if( m_pChild[2] ) m_pChild[2]->FrustumCull( pHeightMap, pFrustum );
	if( m_pChild[3] ) m_pChild[3]->FrustumCull( pHeightMap, pFrustum );
}


BOOL CQuadTree::IsVisible( SVtxNormTex* pHeightMap, float fLODRatio ) 
{ 
	return ( abs(m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL]) <= GetLODLevel(pHeightMap, fLODRatio) );
}


//-----------------------------------------------------------------------------//
// 카메라와 현재 노드와의 거리값을 기준으로 LOD값을 구한다.
//-----------------------------------------------------------------------------//
int	CQuadTree::GetLODLevel( SVtxNormTex* pHeightMap, float fLODRatio )
{
	Vector3 v = pHeightMap[ m_nCenter].v - *(Vector3*)&g_Camera.GetEye();
	float d = v.Length();
	return max( (int)(d*fLODRatio), 1 );
}


//-----------------------------------------------------------------------------//
// 이웃노드를 만든다.(삼각형 찢어짐 방지용)
//-----------------------------------------------------------------------------//
void CQuadTree::BuildNeighborNode( CQuadTree* pRoot, SVtxNormTex* pHeightMap, int cx )
{
	for( int i=0 ; i<4 ; i++ )
	{
		int conertl = m_nCorner[0];
		int conertr = m_nCorner[1];
		int conerbl = m_nCorner[2];
		int conerbr = m_nCorner[3];
		// 이웃노드의 4개 코너값을 얻는다.
		int n = GetNodeIndex( i, cx, conertl, conertr, conerbl, conerbr );
		// 코너값으로 이웃노드의 포인터를 얻어온다.
		if( n >= 0 ) m_pNeighbor[i] = pRoot->FindNode( pHeightMap, conertl, conertr, conerbl, conerbr );
	}

	// 자식노드로 재귀호출
	if( m_pChild[0] )
	{
		m_pChild[0]->BuildNeighborNode( pRoot, pHeightMap, cx );
		m_pChild[1]->BuildNeighborNode( pRoot, pHeightMap, cx );
		m_pChild[2]->BuildNeighborNode( pRoot, pHeightMap, cx );
		m_pChild[3]->BuildNeighborNode( pRoot, pHeightMap, cx );
	}
}


//-----------------------------------------------------------------------------//
// 쿼드트리를 만든다.(Build()함수에서 불린다)
//-----------------------------------------------------------------------------//
BOOL CQuadTree::BuildQuadTree( SVtxNormTex* pHeightMap )
{
	if( SubDivide() )
	{
		// 좌측상단과, 우측 하단의 거리를 구한다.
		Vector3 v = pHeightMap[ m_nCorner[CORNER_TL]].v - 
					pHeightMap[ m_nCorner[CORNER_BR]].v;
		// v의 거리값이 이 노드를 감싸는 경계구의 지름이므로, 
		// 2로 나누어 반지름을 구한다.
		m_fRadius	  = v.Length() / 2.0f;

		m_pChild[CORNER_TL]->BuildQuadTree( pHeightMap );
		m_pChild[CORNER_TR]->BuildQuadTree( pHeightMap );
		m_pChild[CORNER_BL]->BuildQuadTree( pHeightMap );
		m_pChild[CORNER_BR]->BuildQuadTree( pHeightMap );
	}
	return TRUE;
}


//-----------------------------------------------------------------------------//
// 4개 방향(상단,하단,좌측,우측)의 이웃노드 인덱스를 구한다.
// 가운데 인덱스값을 리턴한다.
//-----------------------------------------------------------------------------//
int	CQuadTree::GetNodeIndex( int ed, int cx, int& tl, int& tr, int& bl, int& br )
{
	int	n, gap;
	int topleft = tl;
	int topright = tr;
	int bottomleft = bl;
	int bottomright = br;
	gap	= abs(topright-topleft);	// 현재 노드의 좌우폭값

	switch( ed )
	{
		case EDGE_UP:	// 위쪽 방향 이웃노드의 인덱스
			tl = topleft - cx * gap;
			tr = topright - cx * gap;
			bl = topleft;
			br = topright;
			break;
		case EDGE_DN:	// 아래 방향 이웃노드의 인덱스
			tl = bottomleft;
			tr = bottomright;
			bl = bottomleft + cx * gap;
			br = bottomright + cx * gap;
			break;
		case EDGE_LT:	// 좌측 방향 이웃노드의 인덱스
			tl = topleft + gap;
			tr = topleft;
			bl = bottomleft + gap;
			br = bottomleft;
			break;
		case EDGE_RT:	// 우측 방향 이웃노드의 인덱스
			tl = topright;
			tr = topright - gap;
			bl = bottomright;
			br = bottomright - gap;
			break;
	}

	n = ( tl + tr + bl + br ) / 4;	// 가운데 인덱스
	if( !IS_IN_RANGE( n, 0, cx * cx - 1 ) ) return -1;

	return n;
}	


//-----------------------------------------------------------------------------//
// 쿼드트리를 검색해서 4개 코너값과 일치하는 노드를 찾는다.
// _0,_1,_2,_3: CORNER_TL, CORNER_TR, CORNER_BL, CORNER_BR
//-----------------------------------------------------------------------------//
CQuadTree* CQuadTree::FindNode( SVtxNormTex* pHeightMap, int tl, int tr, int bl, int br )
{
	CQuadTree*	p = NULL;
	// 일치하는 노드라면 노드값을 리턴
	if( (m_nCorner[0] == tl) && (m_nCorner[1] == tr) && (m_nCorner[2] == bl) && (m_nCorner[3] == br) )
		return this;

	// 자식 노드가 있는가?
	if( m_pChild[0] )
	{
		RECT rc;
		POINT pt;
		int n = ( tl + tr + bl + br ) / 4;

		// 현재 맵상에서의 위치
		pt.x = (int)pHeightMap[n].v.x;
		pt.y = (int)pHeightMap[n].v.z;

		// 4개의 코너값을 기준으로 자식노드의 맵 점유범위를 얻는다.
		SetRect( &rc, (int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_TL]].v.x, 
					  (int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_TL]].v.z, 
					  (int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_BR]].v.x, 
					  (int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_BR]].v.z );
		NormalizeRect( &rc );
		// pt값이 점유범위안에 있다면 자식노드로 들어간다.
		if( ::PtInRect( &rc, pt ) )
			return m_pChild[0]->FindNode( pHeightMap, tl, tr, bl, br );

		SetRect( &rc, (int)pHeightMap[m_pChild[1]->m_nCorner[CORNER_TL]].v.x, 
					  (int)pHeightMap[m_pChild[1]->m_nCorner[CORNER_TL]].v.z, 
					  (int)pHeightMap[m_pChild[1]->m_nCorner[CORNER_BR]].v.x, 
					  (int)pHeightMap[m_pChild[1]->m_nCorner[CORNER_BR]].v.z );
		NormalizeRect( &rc );
		if( ::PtInRect( &rc, pt ) )
			return m_pChild[1]->FindNode( pHeightMap, tl, tr, bl, br );

		SetRect( &rc, (int)pHeightMap[m_pChild[2]->m_nCorner[CORNER_TL]].v.x, 
					  (int)pHeightMap[m_pChild[2]->m_nCorner[CORNER_TL]].v.z, 
					  (int)pHeightMap[m_pChild[2]->m_nCorner[CORNER_BR]].v.x, 
					  (int)pHeightMap[m_pChild[2]->m_nCorner[CORNER_BR]].v.z );
		NormalizeRect( &rc );
		if( ::PtInRect( &rc, pt ) )
			return m_pChild[2]->FindNode( pHeightMap, tl, tr, bl, br );

		SetRect( &rc, (int)pHeightMap[m_pChild[3]->m_nCorner[CORNER_TL]].v.x, 
					  (int)pHeightMap[m_pChild[3]->m_nCorner[CORNER_TL]].v.z, 
					  (int)pHeightMap[m_pChild[3]->m_nCorner[CORNER_BR]].v.x, 
					  (int)pHeightMap[m_pChild[3]->m_nCorner[CORNER_BR]].v.z );
		NormalizeRect( &rc );
		if( ::PtInRect( &rc, pt ) )
			return m_pChild[3]->FindNode( pHeightMap, tl, tr, bl, br );
	}

	return NULL;
}
