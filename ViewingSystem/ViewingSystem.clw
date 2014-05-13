; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CViewingSystemView
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ViewingSystem.h"
LastPage=0

ClassCount=5
Class1=CViewingSystemApp
Class2=CViewingSystemDoc
Class3=CViewingSystemView
Class4=CMainFrame

ResourceCount=2
Resource1=IDR_MAINFRAME
Class5=CAboutDlg
Resource2=IDD_ABOUTBOX

[CLS:CViewingSystemApp]
Type=0
HeaderFile=ViewingSystem.h
ImplementationFile=ViewingSystem.cpp
Filter=N

[CLS:CViewingSystemDoc]
Type=0
HeaderFile=ViewingSystemDoc.h
ImplementationFile=ViewingSystemDoc.cpp
Filter=N

[CLS:CViewingSystemView]
Type=0
HeaderFile=ViewingSystemView.h
ImplementationFile=ViewingSystemView.cpp
Filter=C
BaseClass=CView
VirtualFilter=VWC
LastObject=ID_CARTOON2


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=ID_WIREFRAME




[CLS:CAboutDlg]
Type=0
HeaderFile=ViewingSystem.cpp
ImplementationFile=ViewingSystem.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_APP_EXIT
Command4=ID_PARALLEL
Command5=ID_PERSPECTIVE
Command6=ID_WIREFRAME
Command7=ID_FLAT
Command8=ID_GOURAUD
Command9=ID_PHONG
Command10=ID_CARTOON
Command11=ID_CARTOON2
CommandCount=11

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
CommandCount=8

