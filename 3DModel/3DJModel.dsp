# Microsoft Developer Studio Project File - Name="3DJModel" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=3DJModel - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "3DJModel.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "3DJModel.mak" CFG="3DJModel - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "3DJModel - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "3DJModel - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "3DJModel - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3dx9.lib d3d9.lib dxguid.lib winmm.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "3DJModel - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3dx9.lib d3d9.lib dxguid.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "3DJModel - Win32 Release"
# Name "3DJModel - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\3DJModel.cpp
# End Source File
# Begin Source File

SOURCE=.\3dNode.cpp
# End Source File
# Begin Source File

SOURCE=.\ai_object.cpp
# End Source File
# Begin Source File

SOURCE=.\AniMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Bone.cpp
# End Source File
# Begin Source File

SOURCE=.\Camera.cpp
# End Source File
# Begin Source File

SOURCE=.\character.cpp
# End Source File
# Begin Source File

SOURCE=.\collision.cpp
# End Source File
# Begin Source File

SOURCE=.\combo.cpp
# End Source File
# Begin Source File

SOURCE=.\control.cpp
# End Source File
# Begin Source File

SOURCE=.\d3dUtility.cpp
# End Source File
# Begin Source File

SOURCE=.\dbg.cpp
# End Source File
# Begin Source File

SOURCE=.\dbgpcinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\dbgsymengine.cpp
# End Source File
# Begin Source File

SOURCE=.\drawtext.cpp
# End Source File
# Begin Source File

SOURCE=.\FileLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\Global.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyAniMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\LifeBar.cpp
# End Source File
# Begin Source File

SOURCE=.\LinearMemLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\Matrix.cpp
# End Source File
# Begin Source File

SOURCE=.\Mesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Model.cpp
# End Source File
# Begin Source File

SOURCE=.\Quaternion.cpp
# End Source File
# Begin Source File

SOURCE=.\scene.cpp
# End Source File
# Begin Source File

SOURCE=.\sceneGame.cpp
# End Source File
# Begin Source File

SOURCE=.\sceneMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\script.cpp
# End Source File
# Begin Source File

SOURCE=.\script_analyze.cpp
# End Source File
# Begin Source File

SOURCE=.\script_codegen.cpp
# End Source File
# Begin Source File

SOURCE=.\script_global.cpp
# End Source File
# Begin Source File

SOURCE=.\script_machine.cpp
# End Source File
# Begin Source File

SOURCE=.\script_parser.cpp
# End Source File
# Begin Source File

SOURCE=.\sound.cpp
# End Source File
# Begin Source File

SOURCE=.\sound_lib.cpp
# End Source File
# Begin Source File

SOURCE=.\sound_track.cpp
# End Source File
# Begin Source File

SOURCE=.\Terrain.cpp
# End Source File
# Begin Source File

SOURCE=.\Track.cpp
# End Source File
# Begin Source File

SOURCE=.\Tree.cpp
# End Source File
# Begin Source File

SOURCE=.\uicontrol.cpp
# End Source File
# Begin Source File

SOURCE=.\Vector2.cpp
# End Source File
# Begin Source File

SOURCE=.\Vector3.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\3dNode.h
# End Source File
# Begin Source File

SOURCE=.\ai_object.h
# End Source File
# Begin Source File

SOURCE=.\AniMesh.h
# End Source File
# Begin Source File

SOURCE=.\Bone.h
# End Source File
# Begin Source File

SOURCE=.\Camera.h
# End Source File
# Begin Source File

SOURCE=.\character.h
# End Source File
# Begin Source File

SOURCE=.\collision.h
# End Source File
# Begin Source File

SOURCE=.\combo.h
# End Source File
# Begin Source File

SOURCE=.\control.h
# End Source File
# Begin Source File

SOURCE=.\d3dUtility.h
# End Source File
# Begin Source File

SOURCE=.\dbg.h
# End Source File
# Begin Source File

SOURCE=.\dbgpcinfo.h
# End Source File
# Begin Source File

SOURCE=.\dbgstream.h
# End Source File
# Begin Source File

SOURCE=.\dbgsymengine.h
# End Source File
# Begin Source File

SOURCE=.\debug_stream.h
# End Source File
# Begin Source File

SOURCE=.\drawtext.h
# End Source File
# Begin Source File

SOURCE=.\FileLoader.h
# End Source File
# Begin Source File

SOURCE=.\Global.h
# End Source File
# Begin Source File

SOURCE=.\KeyAniMesh.h
# End Source File
# Begin Source File

SOURCE=.\LifeBar.h
# End Source File
# Begin Source File

SOURCE=.\LinearMemLoader.h
# End Source File
# Begin Source File

SOURCE=.\Math.h
# End Source File
# Begin Source File

SOURCE=.\Matrix.h
# End Source File
# Begin Source File

SOURCE=.\Mesh.h
# End Source File
# Begin Source File

SOURCE=.\Model.h
# End Source File
# Begin Source File

SOURCE=.\parser.h
# End Source File
# Begin Source File

SOURCE=.\parserdef.h
# End Source File
# Begin Source File

SOURCE=.\Quaternion.h
# End Source File
# Begin Source File

SOURCE=.\scene.h
# End Source File
# Begin Source File

SOURCE=.\sceneGame.h
# End Source File
# Begin Source File

SOURCE=.\sceneMenu.h
# End Source File
# Begin Source File

SOURCE=.\script.h
# End Source File
# Begin Source File

SOURCE=.\script_analyze.h
# End Source File
# Begin Source File

SOURCE=.\script_codegen.h
# End Source File
# Begin Source File

SOURCE=.\script_global.h
# End Source File
# Begin Source File

SOURCE=.\script_machine.h
# End Source File
# Begin Source File

SOURCE=.\script_parser.h
# End Source File
# Begin Source File

SOURCE=.\sound.h
# End Source File
# Begin Source File

SOURCE=.\sound_lib.h
# End Source File
# Begin Source File

SOURCE=.\sound_track.h
# End Source File
# Begin Source File

SOURCE=.\structdef.h
# End Source File
# Begin Source File

SOURCE=.\Terrain.h
# End Source File
# Begin Source File

SOURCE=.\Track.h
# End Source File
# Begin Source File

SOURCE=.\Tree.h
# End Source File
# Begin Source File

SOURCE=.\uicontrol.h
# End Source File
# Begin Source File

SOURCE=.\Vector2.h
# End Source File
# Begin Source File

SOURCE=.\Vector3.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
