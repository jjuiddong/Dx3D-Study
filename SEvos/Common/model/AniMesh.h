//-----------------------------------------------------------------------------//
// 2005-10-04  programer: jaejung ¦®(¢Á¬Õ¢Á)¦°
// 
//-----------------------------------------------------------------------------//

#if !defined(__ANIMESH_H__)
#define __ANIMESH_H__

#include "mesh.h"

//-----------------------------------------------------------------------------//
// CAniMesh
//-----------------------------------------------------------------------------//
class CAniMesh : public CMesh
{
public:
	CAniMesh();
	CAniMesh( SMeshLoader *pLoader, Matrix44 *pPalette, BOOL bCollision );
	virtual ~CAniMesh();

protected:
	SPhysiqueLoader *m_pPhysiq;
	Matrix44 *m_pPalette;

public:
	virtual void Render();
	virtual BOOL LoadMesh( SMeshLoader *pLoader );

protected:
	BOOL ApplyPalette();
	virtual BOOL LoadPhysiq( SMeshLoader *pLoader, Matrix44 *pPalette );

};

#endif // __JANIMESH_H__
