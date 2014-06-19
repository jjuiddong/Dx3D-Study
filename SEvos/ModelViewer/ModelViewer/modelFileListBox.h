
#ifndef __MODELFILELISTBOX_H__
#define __MODELFILELISTBOX_H__

#include "ModelViewer.h"
#include "FileListBox.h"


class CModelFielListBox : public CFileListBox
{
public:
	CModelFielListBox();
	virtual ~CModelFielListBox();

protected:

public:
	virtual BOOL OnInitDialog();

};

#endif
