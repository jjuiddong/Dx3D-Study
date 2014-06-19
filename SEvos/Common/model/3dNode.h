//-----------------------------------------------------------------------------//
// 2005-10-04  programer: jaejung ¦®(¢Á¬Õ¢Á)¦°
// 
//-----------------------------------------------------------------------------//

#if !defined(__3DNODE_H__)
#define __3DNODE_H__


//-----------------------------------------------------------------------------//
// C3dNode
//-----------------------------------------------------------------------------//
struct C3dNode
{
public:
	C3dNode( int id );
	virtual ~C3dNode() {}

public:
	int GetId() { return m_nId; }
	virtual BOOL Animate( int nDelta ) { return TRUE; }
	virtual void Render() {}

public:
	int m_nId;
	C3dNode *m_pNext;
	C3dNode *m_pPrev;
	C3dNode *m_pChild;
	C3dNode *m_pParent;

};

void InsertChildTree( C3dNode *pParent, C3dNode *pInst );
C3dNode* FindTree( C3dNode *pCurTree, int nId );
BOOL DelTree( C3dNode *pCurTree, int nId );
void ReleaseTree( C3dNode *pTree );

#endif // __J3DNODE_H__
