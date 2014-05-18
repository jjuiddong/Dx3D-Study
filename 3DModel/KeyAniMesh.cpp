
#include "global.h"
#include "keyanimesh.h"
#include "track.h"


CKeyAniMesh::CKeyAniMesh() :
m_AniStart(0), m_AniEnd(0), m_AniFrame(0), m_pTrack(NULL) 
{

}

CKeyAniMesh::CKeyAniMesh( SMeshLoader *pLoader, BOOL bCollision ):
CMesh( pLoader, bCollision ),
m_AniStart(0), m_AniEnd(0), m_AniFrame(0), m_pTrack(NULL) 
{
}

CKeyAniMesh::CKeyAniMesh( SMeshLoader *pLoader, SKeyLoader *pKey, BOOL bCollision ):
CMesh( pLoader, bCollision ),
m_AniStart(0), m_AniEnd(0), m_AniFrame(0), m_pTrack(NULL) 
{
	LoadAni( pKey );
}


CKeyAniMesh::~CKeyAniMesh() 
{
	if( m_pTrack )
	{
		delete m_pTrack;
		m_pTrack = NULL;
	}
}


//-----------------------------------------------------------------------------//
// Load Animation
//-----------------------------------------------------------------------------//
BOOL CKeyAniMesh::LoadAni( SKeyLoader *pKey )
{
	if( !pKey ) return FALSE;

	// default: 첫번째 에니메이션을 가르킨다.
	m_AniStart	= (int)pKey->start;
	m_AniEnd	= (int)pKey->end;
	m_AniFrame	= m_AniStart;

	// animation 생성
	m_pTrack = new CTrack;
	m_pTrack->Load( pKey );

	return TRUE;
}


//-----------------------------------------------------------------------------//
// Animation
//-----------------------------------------------------------------------------//
void CKeyAniMesh::Animate( int nDelta )
{
	if( !m_pTrack ) return;

	m_AniFrame += nDelta;
	if( m_AniFrame > m_AniEnd )
	{
		m_AniFrame = m_AniStart;
		m_pTrack->InitAnimate();
	}

	m_matAniTM.SetIdentity();
	m_pTrack->Animate( m_AniFrame, &m_matAniTM );
}

