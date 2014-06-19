
#include "stdafx.h"
#include "common/FileLoader.h"
#include "modelFileListBox.h"

using namespace std;


CModelFielListBox::CModelFielListBox()
{
}


CModelFielListBox::~CModelFielListBox()
{
}


//------------------------------------------------------------------------
// 
// [2011/2/22 jjuiddong]
//------------------------------------------------------------------------
BOOL CModelFielListBox::OnInitDialog()
{
	CFileListBox::OnInitDialog();

	list<string> extList;
	extList.push_back("msh");
	SetExtendNameList(&extList);
	SetFilePath((char*)g_szModelPath);
	SetFileListName("Model List");

	Refresh();

	return TRUE;
}
