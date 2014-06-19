//-----------------------------------------------------------------------------//
// 2005-10-05  programer: jaejung ¦®(¢Á¬Õ¢Á)¦°
// 
//-----------------------------------------------------------------------------//

#if !defined(__KEYANIMESH_H__)
#define __KEYANIMESH_H__


#include "mesh.h"

//-----------------------------------------------------------------------------//
// CKeyAniMesh
//-----------------------------------------------------------------------------//
class CTrack;
class CKeyAniMesh : public CMesh
{
public:
	CKeyAniMesh();
	CKeyAniMesh( SMeshLoader *pLoader, BOOL bCollision );
	CKeyAniMesh( SMeshLoader *pLoader, SKeyLoader *pKey, BOOL bCollision );
	virtual ~CKeyAniMesh();

protected:
	int m_AniStart;
	int m_AniEnd;
	int m_AniFrame;
	CTrack *m_pTrack;

public:
	virtual BOOL LoadAni( SKeyLoader *pKey );
	virtual void Animate( int nDelta );
	void Clear();

};

#endif // __KEYANIMESH_H_
