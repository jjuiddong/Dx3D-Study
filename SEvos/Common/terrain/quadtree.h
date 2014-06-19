//-----------------------------------------------------------------------------//
// 2009-08-23  programer: jaejung ┏(⊙д⊙)┛
// 
//-----------------------------------------------------------------------------//

#if !defined(__QUADTREE_H__)
#define __QUADTREE_H__

#include "terrain.h"

class CFrustum;
class CQuadTree
{
public:
	CQuadTree( int cx, int cy );
	CQuadTree( CQuadTree *parent );
	virtual ~CQuadTree();

	enum CONER_TYPE { CORNER_TL, CORNER_TR, CORNER_BL, CORNER_BR };
	enum { EDGE_UP, EDGE_DN, EDGE_LT, EDGE_RT }; // 이웃노드 처리용 상수값
	enum QuadLocation { FRUSTUM_OUT = 0,			/// 프러스텀에서 완전벗어남
					   FRUSTUM_PARTIALLY_IN = 1,	/// 프러스텀에 부분포함
					   FRUSTUM_COMPLETELY_IN = 2,	/// 프러스텀에 완전포함
					   FRUSTUM_UNKNOWN = -1 };		/// 모르겠음(^^;)

protected:
	CQuadTree*	m_pChild[ 4];		/// QuadTree의 4개의 자식노드
	CQuadTree*	m_pParent;			/// Triangle Crack(Popping)을 막기위해서 사용한다.
	CQuadTree*	m_pNeighbor[4];		/// Triangle Crack(Popping)을 막기위해서 사용한다.
	int			m_nCenter;			/// QuadTree에 보관할 첫번째 값
	int			m_nCorner[ 4];		/// QuadTree에 보관할 두번째 값
									///    TopLeft(TL)      TopRight(TR)
									///              0------1
									///              |      |
									///              |      |
									///              2------3
									/// BottomLeft(BL)      BottomRight(BR)
	BOOL		m_bCulled;			/// 프러스텀에서 컬링된 노드인가?
	float		m_fRadius;			/// 노드를 감싸는 경계구(bounding sphere)의 반지름

public:
	BOOL Build( SVtxNormTex *pHeightMap );
	int GenerateIndex( LPVOID pIB, SVtxNormTex* pHeightMap, CFrustum* pFrustum, float fLODRatio );

protected:
	CQuadTree* AddChild( int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR );	
	BOOL SetCorners( int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR );
	BOOL SubDivide();
	BOOL IsVisible( SVtxNormTex* pHeightMap, float fLODRatio );
	int	GenTriIndex( int nTriangles, LPVOID pIndex, SVtxNormTex* pHeightMap, float fLODRatio );
	void Destroy();
	QuadLocation IsInFrustum( SVtxNormTex *pHeightMap, CFrustum* pFrustum );
	void FrustumCull( SVtxNormTex *pHeightMap, CFrustum* pFrustum );
	int	GetLODLevel( SVtxNormTex* pHeightMap, float fLODRatio );
	void GetCorner( int& _0, int& _1, int& _2, int& _3 ) // 코너 인덱스 값을 얻어온다.
		{ _0 = m_nCorner[0]; _1 = m_nCorner[1]; _2 = m_nCorner[2]; _3 = m_nCorner[3]; }

	BOOL BuildQuadTree( SVtxNormTex* pHeightMap );
	void BuildNeighborNode( CQuadTree* pRoot, SVtxNormTex* pHeightMap, int cx );
	CQuadTree* FindNode( SVtxNormTex* pHeightMap, int _0, int _1, int _2, int _3 );
	int GetNodeIndex( int ed, int cx, int& _0, int& _1, int& _2, int& _3 );

};

#endif // __QUADTREE_H__
