
/*
	2003-11-09	programer: jaejung ^^;;
	PC정보 얻는 라이브러리
	인터넷에서 긁어왔다. --;;
*/


#if !defined(__DBGPCINFO_H__)
#define __DBGPCINFO_H__

#include <windows.h>

#pragma once

typedef struct tagNAMEGROUP{
	int nItemCnt;
	char szName[5][1024];	
}NAMEGROUP, *LPNAMEGROUP;

typedef struct __MEMORYINFO{
	UINT  nUsingPercent;
	DWORD dwPhysicalTotal;
	DWORD dwPhysicalFree;
	DWORD dwVirtualTotal;
	DWORD dwVirtualFree;
}MEMORYINFO;

typedef struct __CPUIDENTIFY
{
   DWORD dwSignature;
   DWORD dwFeatureEbx;
   DWORD dwFeatureEcx;
   DWORD dwFeatures;
}CPUIDENTIFY, FAR* LPCPUINDENTIFY;


#define _CPU_FEATURE_MMX    0x0001
#define _CPU_FEATURE_SSE    0x0002
#define _CPU_FEATURE_SSE2   0x0004
#define _CPU_FEATURE_3DNOW  0x0008

#define _MAX_VNAME_LEN  13
#define _MAX_MNAME_LEN  30

typedef struct _processor_info {
    char v_name[_MAX_VNAME_LEN];        // vendor name
    char model_name[_MAX_MNAME_LEN];    // name of model
                                        // e.g. Intel Pentium-Pro
    int family;                         // family of the processor
                                        // e.g. 6 = Pentium-Pro architecture
    int model;                          // model of processor
                                        // e.g. 1 = Pentium-Pro for family = 6
    int stepping;                       // processor revision number
    int feature;                        // processor feature
                                        // (same as return value from _cpuid)
    int os_support;                     // does OS Support the feature?
    int checks;                         // mask of checked bits in feature
                                        // and os_support fields
} _p_info;


class KSystemInfo  
{
public:
	KSystemInfo();
	virtual ~KSystemInfo();

public:
	BOOL GetOS(LPSTR lpOS, int stringSize);
	BOOL GetIPAddress( LPSTR lpBuf, int stringSize );
	VOID GetWorkGroupName( LPTSTR szGroupName, int stringSize, DWORD *pdwSize );
	void GetVgaInfo( char *szInfo, int stringSize );
	void GetHddInfo( char *szInfo, int stringSize );	
	void GetHddInfo_Win9x( NAMEGROUP *namegroup );
	void GetHddInfo_WinNT( NAMEGROUP *namegroup );
	void GetVgaInfo_WinNT( NAMEGROUP *namegroup );
	void GetVgaInfo_Win9x( NAMEGROUP *namegroup );	
	BOOL IsDynamicDeviceOnWin9x( char *szSubRegPath );
	MEMORYINFO GetMemoryInfo();
	BOOL GetComputerName( LPSTR lpBuf, int stringSize );
	BOOL GetDiskSize( LPSTR lpBuf, int stringSize );
	BOOL GetCPUInfo( LPSTR lpBuf, int stringSize );
//	BOOL GetCPUEx( LPSTR lpCPU );

	int IsCPUID();
//	int _os_support(int feature);
	void map_mname(int family, int model, const char *v_name, char *m_name, int nameSize);
//	int _cpuid (_p_info *pinfo);

	MEMORYINFO	meminfo;

protected:
//	DWORD GetDriveString( KBuffer *pBuf );
	DWORD calculateCpuSpeed();
	DWORD determineTimeStamp();

};

#endif // __DBGPCINFO_H__
