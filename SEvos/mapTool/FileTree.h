#if !defined(AFX_FILETREE_H__608EB76A_B4B1_48C7_BEE9_3AFD6714444B__INCLUDED_)
#define AFX_FILETREE_H__608EB76A_B4B1_48C7_BEE9_3AFD6714444B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "global.h"
#pragma warning(disable: 4786)
#include <string>


// CFileTree window
class CFileTree : public CTreeCtrl
{
public:
	CFileTree();
	typedef struct _tagSObjFile
	{
		_tagSObjFile() {}
		_tagSObjFile( std::string path, HTREEITEM h ): strPath(path), hPItem(h) {}
		std::string strPath;
		HTREEITEM hPItem;
	} SObjFile;
	enum ITEM_TYPE { IT_FOLDER, IT_FILE };

protected:
	CImageList m_TreeImg;

public:
	BOOL Init();
	BOOL OpenModelTable( char *szFileName ); 
	BOOL OpenDirectory( char *szDirectory, OBJ_TYPE eOType );

protected:
	void GetFileName( HTREEITEM hItem, char *szFileName, int stringSize );
	void GetLastFolderName( char *szPath, char *szRcvFolderName, int stringSize );
	ITEM_TYPE GetItemType( HTREEITEM hItem );
	OBJ_TYPE GetObjType( HTREEITEM hItem );
	HTREEITEM InsertItem( LPCTSTR lpszItem, ITEM_TYPE eIType, OBJ_TYPE eOType, HTREEITEM hParent=TVI_ROOT, HTREEITEM hInsertAfter=TVI_LAST );
	LPARAM MakeLParam( ITEM_TYPE eIType, OBJ_TYPE eOType ) { return (MAKELONG(eIType,eOType)); }
	ITEM_TYPE GetItemType( LPARAM lParam ) { return (ITEM_TYPE)LOWORD(lParam); }
	OBJ_TYPE GetObjType( LPARAM lParam )  { return (OBJ_TYPE)HIWORD(lParam); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileTree)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFileTree();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFileTree)
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILETREE_H__608EB76A_B4B1_48C7_BEE9_3AFD6714444B__INCLUDED_)
