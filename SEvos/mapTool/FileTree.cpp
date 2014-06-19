// FileTree.cpp : implementation file
//

#include "stdafx.h"
#include "MapTool2.h"
#include "FileTree.h"
#include "global.h"

#include <list>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileTree

CFileTree::CFileTree()
{
}

CFileTree::~CFileTree()
{
}


BEGIN_MESSAGE_MAP(CFileTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CFileTree)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileTree message handlers


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
BOOL CFileTree::Init()
{
	m_TreeImg.Create( IDB_BITMAP_FILETREE, 16, 4, RGB(255,0,255) );
	SetImageList( &m_TreeImg, TVSIL_NORMAL );
	return TRUE;
}


//-----------------------------------------------------------------------------//
// szDirectory 경로의 파일을 모두 Tree에 추가한다.
// 각 TreeItem의 lParam 값에 eOType값을 할당한다.
//-----------------------------------------------------------------------------//
BOOL CFileTree::OpenDirectory( char *szDirectory, OBJ_TYPE eOType )
{
	char szCurDir[ MAX_PATH];
	GetCurrentDirectory( MAX_PATH, szCurDir );
	SetCurrentDirectory( szDirectory );

	char szFolderName[ MAX_PATH];
	GetLastFolderName( szDirectory, szFolderName, sizeof(szFolderName) );
	HTREEITEM hRootItem = InsertItem( szFolderName, IT_FOLDER, eOType );

	list<SObjFile> PathList;
	PathList.push_back( SObjFile(".", hRootItem) );

	while( !PathList.empty() )
	{
		SObjFile info = PathList.back();
					    PathList.pop_back();

		string strSearch = info.strPath;
		strSearch += "/*.*";

		WIN32_FIND_DATA fd;
		HANDLE hDir = ::FindFirstFile( strSearch.c_str(), &fd );
		if( hDir == INVALID_HANDLE_VALUE )
			continue;

		while( ::FindNextFile( hDir, &fd ) )
		{
			if( !strcmp(".",fd.cFileName) || !strcmp("..",fd.cFileName) )
				continue;

			if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				HTREEITEM hItem = InsertItem( fd.cFileName, IT_FOLDER, eOType, info.hPItem, TVI_FIRST );
				PathList.push_back( SObjFile(fd.cFileName,hItem) );
			}
			else
			{
				InsertItem( fd.cFileName, IT_FILE, eOType, info.hPItem );
			}
		}

		FindClose( hDir );
	}
	SetCurrentDirectory( szCurDir );

	return TRUE;
}


//-----------------------------------------------------------------------------//
// ModelTable Script를 읽는다.
//-----------------------------------------------------------------------------//
BOOL CFileTree::OpenModelTable( char *szFileName )
{
/*
	CLinearMemLoader loader;
	loader.LoadTokenFile( "data//script//token_model.txt" );
	SD_SModelTable *pModelTable = (SD_SModelTable*)loader.ReadScript( szFileName, "MODELTABLE" );

	HTREEITEM hItem = InsertItem( "model", IT_FOLDER, OT_MODEL );
	for( int i=0; i < pModelTable->nModelSize; ++i )
		InsertItem( pModelTable->pModel[ i].szName, IT_FILE, OT_MODEL, hItem );

	HTREEITEM hRigidItem = InsertItem( "rigid", IT_FOLDER, OT_RIGID );
	for( i=0; i < pModelTable->nRigidSize; ++i )
		InsertItem( pModelTable->pRigid[ i].szName, IT_FILE, OT_RIGID, hRigidItem );

	HTREEITEM hMapItem = InsertItem( "map", IT_FOLDER, OT_TILE );
	for( i=0; i < pModelTable->nTileSize; ++i )
		InsertItem( pModelTable->pTile[ i].szName, IT_FILE, OT_TILE, hMapItem );

	delete[] (BYTE*)pModelTable;
/**/

	return TRUE;
}


//-----------------------------------------------------------------------------//
// TreeItem 추가
//-----------------------------------------------------------------------------//
HTREEITEM CFileTree::InsertItem( LPCTSTR lpszItem, CFileTree::ITEM_TYPE eIType, OBJ_TYPE eOType, HTREEITEM hParent, HTREEITEM hInsertAfter )
// hParent=TVI_ROOT, hInsertAfter=TVI_LAST
{
	int nImg = (IT_FOLDER==eIType)? 1 : 0;
	LPARAM lParam = MakeLParam( eIType, eOType );
	return CTreeCtrl::InsertItem( TVIF_PARAM |TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE, lpszItem, nImg, nImg, 0, 0, lParam, hParent, hInsertAfter );
}


//-----------------------------------------------------------------------------//
// 디렉토리 경로 스트링에서 마지막번째 폴더이름을 얻는다.
//-----------------------------------------------------------------------------//
void CFileTree::GetLastFolderName( char *szPath, char *szRcvFolderName, int stringSize )
{
	strcpy_s( szRcvFolderName, stringSize, szPath );
	char *s=NULL;
	while( (s = strchr(szRcvFolderName, '/')) )
	{
		memmove( szRcvFolderName, s+1, strlen(s+1) );
		szRcvFolderName[ strlen(s+1)] = NULL;
	}
}


//-----------------------------------------------------------------------------//
// FileTree에서 hItem의 파일명을 얻어온다.
// 부모노드를 따로올라가며 file명을 생성한다.
//-----------------------------------------------------------------------------//
void CFileTree::GetFileName( HTREEITEM hItem, char *szFileName, int stringSize )
{
	if( !hItem ) return;

	CString strText = GetItemText( hItem );
	if( IT_FOLDER == GetItemType(hItem) )
		strText += "/";
	memmove( szFileName + strText.GetLength(), szFileName, strlen(szFileName) );
	strncpy_s( szFileName, stringSize, strText, strText.GetLength() );

	GetFileName( GetParentItem(hItem), szFileName, stringSize );
}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
void CFileTree::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	if( IT_FILE == GetItemType(GetSelectedItem()) )
	{
// 지우지말것
//		char szFileName[ MAX_PATH] = {0,};
//		GetFileName( GetSelectedItem(), szFileName );
//		g_pMapView->SelectObj( szFileName );

		CString strText = GetItemText( GetSelectedItem() );
//		g_pMapView->SelectObj( GetObjType(GetSelectedItem()), (char*)(LPCTSTR)strText );
	}

	*pResult = 0;
}

CFileTree::ITEM_TYPE CFileTree::GetItemType( HTREEITEM hItem )
{
	return GetItemType( GetItemData(hItem) );
}

OBJ_TYPE CFileTree::GetObjType( HTREEITEM hItem )
{
	return GetObjType( GetItemData(hItem) );
}

void CFileTree::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint HitPos;
	GetCursorPos( &HitPos );
	ScreenToClient( &HitPos );
	HTREEITEM hItem = HitTest( HitPos );
	if( hItem )
	{
		if( IT_FILE == GetItemType(hItem) )
		{
// 지우지말것
//			char szFileName[ MAX_PATH] = {0,};
//			GetFileName( hItem, szFileName );
//			g_pMapView->SelectObj( szFileName );

			CString strText = GetItemText( hItem );
//			g_pMapView->SelectObj( GetObjType(hItem), (char*)(LPCTSTR)strText );
			SelectItem(hItem);
		}
	}
	
	*pResult = 0;
}
