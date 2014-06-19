
#include "stdafx.h"
#include "aniFileListBox.h"


CAniFileListBox::CAniFileListBox()
{
}


CAniFileListBox::~CAniFileListBox()
{
	int a = 0;
}



BOOL CAniFileListBox::OnInitDialog()
{
	CFileListBox::OnInitDialog();

	list<string> extList;
	extList.push_back("ani");
	SetExtendNameList(&extList);
	SetFilePath("data/model");
	SetFileListName("Animation List");

	Refresh();

	return TRUE;
}
