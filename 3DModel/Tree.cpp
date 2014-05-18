
#include "tree.h"

/*
//-----------------------------------------------------------------------------//
// Node의 가장끝에 추가된다.
//-----------------------------------------------------------------------------//
void CJTree::InsertChildTree( CJTree *pParent, CJTree *pInst )
{
	if( !pParent ) return;

	if( !pParent->m_pChild )
	{
		pParent->m_pChild = pInst;
		pInst->m_pParent = pParent;
	}
	else
	{
		CJTree *pNext = pParent->m_pChild;
		CJTree *pPrev = pParent->m_pChild;
		while( pNext )
		{
			pPrev = pNext;
			pNext = pNext->m_pNext;
		}
		pPrev->m_pNext = pInst;
		pInst->m_pPrev = pPrev;
		pInst->m_pParent = pParent;
	}
}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
CJTree* CJTree::FindTree( CJTree *pCurNode, int nId )
{
	if( !pCurNode ) return NULL;
	if( pCurNode->GetId() == nId ) return pCurNode;

	CJTree *pNode = NULL;
	pNode = FindTree( pCurNode->m_pNext, nId );
	if( pNode ) return pNode;
	pNode = FindTree( pCurNode->m_pChild, nId );
	if( pNode ) return pNode;

	return NULL;
}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
BOOL CJTree::DelTree( CJTree *pCurTree, int nId )
{
	CJTree *pNode = FindTree( pCurTree, nId );
	if( !pNode ) return FALSE;

	CJTree *pPrevNode = pNode->m_pPrev;
	CJTree *pNextNode = pNode->m_pNext;
	CJTree *pParentNode = pNode->m_pParent;

	if( pPrevNode )
		pPrevNode->m_pNext = pNextNode;
	if( pNextNode )
		pNextNode->m_pPrev = pPrevNode;

	if( !pPrevNode ) // root 일때
	{
		if( pParentNode )
		{
			pParentNode->m_pChild = pNextNode;
			if( pNextNode )
				pNextNode->m_pPrev = NULL;
		}
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
void CJTree::ReleaseTree( CJTree *pTree )
{
	if( !pTree ) return;
	
	ReleaseTree( pTree->m_pChild );
	ReleaseTree( pTree->m_pNext );
	delete pTree;
}

/**/
