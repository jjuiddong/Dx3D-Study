//-----------------------------------------------------------------------------//
// 2005-10-17  programer: jaejung ┏(⊙д⊙)┛
// 
//-----------------------------------------------------------------------------//

#if !defined(__TREE_H__)
#define __TREE_H__


#include <windows.h>
/*
template< class T >
struct STreeNode
{
	STreeNode() {}
	STreeNode( T p ):t(p) {}

	T t;
	STreeNode *pNext;
	STreeNode *pPrev;
	STreeNode *pChild;
	STreeNode *pParent;
};


//-----------------------------------------------------------------------------//
// ClassName: CTree
//-----------------------------------------------------------------------------//
template< class T >
class CTree
{
public:
	CTree() : m_pRoot(NULL) { m_pRoot = new SNode; }
	virtual ~CTree();
	typedef STreeNode<T> SNode;

protected:
	SNode *m_pRoot;

public:
	STreeNode* GetRootNode() { return m_pRoot->pChild; }
	void InsertChild( T t, SNode *pParent );
	void InsertChild( CTree<T> *pTree, SNode *pParent );
	void InsertChild( CTree<T> *pTree ) { InsertChild(pTree, m_pRoot); }
	void InsertChild( T t ) { InsertChild(t, m_pRoot); }
	SNode* Find( T t, SNode *pCurNode );
	SNode* Find( T t ) { return Find(t, m_pRoot->pChild); }

	BOOL Del( SNode *pCompNode, SNode *pCurTree );
	BOOL Del( SNode *pCompNode ) { Del(pCompNode, m_pRoot); }
	void Release( SNode *pNode );	

};


template< class T >
CTree<T>::~CTree() 
{
	Release( m_pRoot );
}

// Node의 가장끝에 추가된다.
template< class T >
void CTree<T>::InsertChild( T t, SNode *pParent )
{
	if( !pParent ) return;

	SNode *pInst = new SNode( t );

	if( !pParent->pChild )
	{
		pParent->pChild = pInst;
		pInst->pParent = pParent;
	}
	else
	{
		SNode *pNext = pParent->pChild;
		SNode *pPrev = pParent->pChild;
		while( pNext )
		{
			pPrev = pNext;
			pNext = pNext->pNext;
		}
		pPrev->pNext = pInst;
		pInst->pPrev = pPrev;
		pInst->pParent = pParent;
	}
}


// 
template< class T >
void CTree<T>::InsertChild( CTree<T> *pTree, SNode *pParent )
{

}


// 
template< class T >
CTree<T>::SNode* CTree<T>::Find( T t, SNode *pCurNode ) // pCurNode = NULL
{
	if( !pCurNode ) return NULL;
	if( t == pCurNode->t ) return pCurNode;

	SNode *pNode = NULL;
	pNode = Find( t, pCurNode->pNext );
	if( pNode ) return pNode;
	pNode = Find( t, pCurNode->pChild );
	if( pNode ) return pNode;

	return NULL;
}

// 
template< class T >
BOOL CTree<T>::Del( SNode *pCompNode, SNode *pCurNode )
{
	SNode *pNode = Find( pCompNode, pCurTree );
	if( !pNode ) return FALSE;

	SNode *pPrevNode = pNode->pPrev;
	SNode *pNextNode = pNode->pNext;
	SNode *pParentNode = pNode->pParent;

	if( pPrevNode )
		pPrevNode->pNext = pNextNode;
	if( pNextNode )
		pNextNode->pPrev = pPrevNode;

	if( !pPrevNode ) // root 일때
	{
		if( pParentNode )
		{
			pParentNode->pChild = pNextNode;
			if( pNextNode )
				pNextNode->pPrev = NULL;
		}
	}

	return TRUE;
}

// 
template< class T >
void CTree<T>::Release( SNode *pNode )
{
	if( !pNode ) return;

	Release( pNode->pChild );
	Release( pNode->pNext );
	delete pNode;
}

/**/
#endif // __TREE_H__
