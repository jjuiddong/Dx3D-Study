
//#include "stdafx.h"
#include "dbgpcinfo.h"

#include <stdio.h>
#include <TCHAR.H>

#include <Lm.h>
#include <Mmsystem.h>

#include <winsock.h>


#pragma comment( lib, "ws2_32.lib" )
#pragma comment( lib, "winmm.lib" )


// These are the bit flags that get set on calling cpuid
// with register eax set to 1
#define _MMX_FEATURE_BIT        0x00800000
#define _SSE_FEATURE_BIT        0x02000000
#define _SSE2_FEATURE_BIT       0x04000000

// This bit is set when cpuid is called with
// register set to 80000001h (only applicable to AMD)
#define _3DNOW_FEATURE_BIT      0x80000000



struct FREQ_INFO
{
    unsigned long in_cycles;                                
    unsigned long ex_ticks;     
    unsigned long raw_freq;
    unsigned long norm_freq;
};

typedef struct __CPUINFO{
	UINT nSpeed;
	char szType[32];
	char szLevel[32];
	char szArchitecture[32];
	char szVenderInfo[32];
	DWORD dwMaxApplicationAddress;
	DWORD dwMinApplicationAddress;
	UINT nActiveProcessMask;
	UINT nProcessorNumber;
	UINT nPageSize;
} ZCPUINFO;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KSystemInfo::KSystemInfo()
{
	ZeroMemory( &meminfo, sizeof( MEMORYINFO ) );
}

KSystemInfo::~KSystemInfo()
{

}

BOOL KSystemInfo::GetIPAddress( LPSTR lpBuf )
{
	TCHAR stTemp[255];
	gethostname( stTemp, 255 );
	hostent *pHostent = gethostbyname( stTemp );
	if( !pHostent ) return FALSE;
	IN_ADDR inAddr;
	
	CopyMemory( &inAddr, pHostent->h_addr, 4 );
	wsprintf(lpBuf, "%d.%d.%d.%d", inAddr.S_un.S_un_b.s_b1, inAddr.S_un.S_un_b.s_b2,
								   inAddr.S_un.S_un_b.s_b3, inAddr.S_un.S_un_b.s_b4 );
	return TRUE;
}


// Operating System 정보
BOOL KSystemInfo::GetOS(LPSTR lpOS)
{
    if( !lpOS ) return FALSE;
    OSVERSIONINFO   osvi;
	
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osvi);

    switch( osvi.dwPlatformId )
    {
    case VER_PLATFORM_WIN32s:                   //  window 3.1
		{
            wsprintf( lpOS, "Windows 3.1" );       
		}
        break;

    case VER_PLATFORM_WIN32_WINDOWS:            //  win95
		{
			if( osvi.dwMinorVersion  == 0 )
			{
                wsprintf( lpOS, "Windows 95" );
			}
			else if( osvi.dwMinorVersion > 0 && osvi.dwMinorVersion < 90)      //  win98
			{
				if( strcmp(osvi.szCSDVersion , " A ") == 0 )            
                    wsprintf( lpOS, "Windows 98 Second Edition" );
				else
                    wsprintf( lpOS, "Windows 98" ); 
			}
			else
                wsprintf( lpOS, "Windows ME" );
		}        
        break;

    case VER_PLATFORM_WIN32_NT:         // Window NT and 2000
		{
            if( osvi.dwMajorVersion < 5 )
                wsprintf( lpOS, "Windows NT %d.%d %s", osvi.dwMajorVersion ,
						osvi.dwMinorVersion, osvi.szCSDVersion);

			else if ( osvi.dwMinorVersion == 0 )
                wsprintf( lpOS, "Windows 2000 %s", osvi.szCSDVersion);
			else
				wsprintf( lpOS, "Windows XP %s", osvi.szCSDVersion);
       
		}
        
        break;
    }   

    if( strlen( lpOS ) > 32 ) return FALSE;
    return TRUE;

}


VOID KSystemInfo::GetWorkGroupName(LPTSTR szGroupName, DWORD *pdwSize)
{
	TCHAR szTempGroupName[129];
	
	szTempGroupName[128] = 0;
	
	if ( pdwSize == 0 ) return;
	if ( szGroupName == 0 ) return;

	memset( szGroupName, 0, *pdwSize + 1 );

	if ( GetVersion() < 0x80000000)                
	{	// WinNT series
		typedef struct __WKSTA_INFO_102 {
			DWORD     wki102_platform_id;
			LPWSTR    wki102_computername;
			LPWSTR    wki102_langroup;
			DWORD     wki102_ver_major;
			DWORD     wki102_ver_minor;
			LPWSTR    wki102_lanroot;
			DWORD     wki102_logged_on_users;
		} __WKSTA_INFO_102, *__PWKSTA_INFO_102;

		typedef NET_API_STATUS (NET_API_FUNCTION *FPNETWKSTAGETINFO) (
			IN  LPWSTR   servername OPTIONAL,
			IN  DWORD   level,
			OUT LPBYTE  *bufptr
			);
		typedef NET_API_STATUS (NET_API_FUNCTION *FPNETAPIBUFFERFREE) (
			IN LPVOID Buffer
			);

		const	DWORD	dwLevel = 102;
		const	TCHAR  *szNetApiModName = "netapi32.dll";
		FPNETAPIBUFFERFREE fpApiBufferFree;
		FPNETWKSTAGETINFO fpGetInfo;
		LPWSTR	pszServerName = NULL;
		HMODULE hMod;
		BOOL	fIsDLLLoaded = FALSE;

		hMod = GetModuleHandle( szNetApiModName  );
		if ( hMod == NULL )
		{
			hMod = LoadLibrary( szNetApiModName );
			if ( !hMod ) return;
			fIsDLLLoaded = TRUE;
		}

		fpGetInfo = (FPNETWKSTAGETINFO)::GetProcAddress( hMod, "NetWkstaGetInfo" );
		fpApiBufferFree = (FPNETAPIBUFFERFREE)::GetProcAddress( hMod, "NetApiBufferFree" );
		if ( !fpGetInfo || !fpApiBufferFree )
		{
			if ( fIsDLLLoaded )	FreeLibrary( hMod );
			return;
		}

		{
			__PWKSTA_INFO_102	pBuf = NULL;
			NET_API_STATUS		nStatus;

			nStatus = fpGetInfo( pszServerName, dwLevel, (LPBYTE*)&pBuf );

			if (nStatus == NERR_Success)
			{
				//유니코드를 스트링으로 변환
				WideCharToMultiByte(CP_ACP, 0, (const unsigned short *)pBuf->wki102_langroup, -1, szTempGroupName, sizeof(szTempGroupName), 0, 0);

				int nLen = min(*pdwSize,_tcslen(szTempGroupName));
				_tcsncpy( szGroupName, szTempGroupName, nLen  );
				szGroupName[nLen] = 0;
				*pdwSize = nLen;
			}

			if (pBuf != NULL) fpApiBufferFree(pBuf);
		}
			
		if ( fIsDLLLoaded )	FreeLibrary( hMod );
	}
	else
	{
		// 9X에서는 레지스트리에서 검색을 해야함..
		LONG result;
		HKEY hKey;      
		DWORD dwType = REG_SZ;  
		DWORD dwSize = 128;

		result = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
							_T("System\\CurrentControlSet\\Services\\VxD\\VNETSUP"),0,
							KEY_READ, &hKey);
		if (result == ERROR_SUCCESS)
		{
			result = RegQueryValueEx(hKey, _T("Workgroup"), 0, &dwType, (LPBYTE)szTempGroupName, &dwSize);
		}

		hKey = NULL;
		RegCloseKey(hKey);

		int nLen = min(*pdwSize,_tcslen(szTempGroupName));
		_tcsncpy( szGroupName, szTempGroupName, nLen  );
		szGroupName[nLen] = 0;
		*pdwSize = nLen;
	}
}

void KSystemInfo::GetHddInfo(char *szInfo)
{	
	NAMEGROUP namegroup;

	if ( GetVersion() < 0x80000000){
		
		// WinNT Series //
		GetHddInfo_WinNT( &namegroup );		
		// WinNT Series //

	}else{
		// Win9X Series //
		GetHddInfo_Win9x( &namegroup );		
		// Win9X Series //
	}

	/* it use for several device */		
	int nCnt = 0;
	char cToken;
	cToken = ',';

	if( !(namegroup.nItemCnt) ){
		strcpy( szInfo, "none" );
	}else{
		strcpy( szInfo, namegroup.szName[0] );
	}

	nCnt = 1;
	for( int i = 2 ; i < namegroup.nItemCnt; i++ )
		wsprintf( szInfo, "%s%c%s", szInfo, cToken, namegroup.szName[i] );
}

void KSystemInfo::GetVgaInfo(char *szInfo)
{
	NAMEGROUP namegroup;

	if ( GetVersion() < 0x80000000){		
		// WinNT Series //
		GetVgaInfo_WinNT( &namegroup );	
		// WinNT Series //
	}else{
		// Win9X Series //
		GetVgaInfo_Win9x( &namegroup );
		// Win9X Series //
	}

	/* it use for several device */		
	int nCnt = 0;
	char cToken;
	cToken = ',';

	if( !(namegroup.nItemCnt) ){
		strcpy( szInfo, "none" );
	}else{
		strcpy( szInfo, namegroup.szName[0] );
	}
	
	nCnt = 1;
	for( int i = 2 ; i < namegroup.nItemCnt; i++ ){		
		wsprintf( szInfo, "%s%c%s", szInfo, cToken, namegroup.szName[i] );
	}
}

void KSystemInfo::GetHddInfo_WinNT( NAMEGROUP *namegroup )
{
	BOOL		bVerify;
	DWORD		dwKeyIndex, dwSubKeyIndex, dwVerifyLoop;
	DWORD		dwSize, dwType;
	char		szService[1024], szDesc[1024];
	char		szEnumName[1024];
	long		lResult, lResult1;
	HKEY		hMainKey, hSubKey_1, hSubKey_2;
	LPSTR		lpNTSubKey = "SYSTEM\\CURRENTCONTROLSET\\ENUM\\IDE";
	LPSTR		lpNTKeyword = "SERVICE";
	LPSTR		lpNTService = "DISK";
	LPSTR		lpNTVerifyName = "CONTROL";
	LPSTR		lpNTDesc = "FRIENDLYNAME";

		// GROBAL INIT
	namegroup->nItemCnt = 0;

	// ONLY ONE LOOP FOR ANYTIME BREAK :P //
	do{
		// INIT VARIABLE
		dwKeyIndex = 0;
		lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpNTSubKey, 0 , KEY_ENUMERATE_SUB_KEYS|KEY_EXECUTE|KEY_QUERY_VALUE, &hMainKey);		
		if (lResult != ERROR_SUCCESS)break;

		// REAL LOOP FOR FINDING HDD NAME
		while( 1 )
		{
			lResult = RegEnumKey( hMainKey , dwKeyIndex, szEnumName, MAX_PATH + 1);						
			lResult1 = RegOpenKeyEx(hMainKey, szEnumName, 0, KEY_READ, &hSubKey_1);
			if (lResult == ERROR_NO_MORE_ITEMS || lResult1 != ERROR_SUCCESS )break;						

			dwSubKeyIndex = 0;
			while( 1 )
			{
				lResult = RegEnumKey( hSubKey_1, dwSubKeyIndex, szEnumName, MAX_PATH + 1);						
				lResult1 = RegOpenKeyEx(hSubKey_1, szEnumName, 0, KEY_READ, &hSubKey_2);
				if (lResult == ERROR_NO_MORE_ITEMS || lResult1 != ERROR_SUCCESS )break;						

				// CHECK WHETHER SERVICE IS !!DISK!!
				dwType = REG_SZ;dwSize = sizeof(szService);memset( szService, 0, sizeof(szService) );
				lResult = RegQueryValueEx(hSubKey_2, lpNTKeyword, NULL, &dwType, (LPBYTE)szService, &dwSize);	
				if( lResult != ERROR_SUCCESS ){
					dwSubKeyIndex++;
					continue;
				}

				if( _strnicmp( szService, lpNTService, strlen(szService) ) ){
					dwSubKeyIndex++;
					continue;
				}

				// INIT SECOND LOOP
				dwVerifyLoop = 0;
				bVerify = TRUE;
				// SECOND LOOP FOR VERIFYING IT IS USING DEVICE
				while( 1 )
				{
					lResult = RegEnumKey( hSubKey_2, dwVerifyLoop, szEnumName, MAX_PATH + 1);										
					if (lResult == ERROR_NO_MORE_ITEMS  ){
						bVerify = FALSE;
						break;						
					}
					
					if( !_strnicmp( szEnumName, lpNTVerifyName, strlen(szEnumName) ) ){
						bVerify = TRUE;
						break;
					}

					dwVerifyLoop++;
				}

				if( bVerify ){
					// GET HDD`NANE
					dwType = REG_SZ;dwSize = sizeof(szDesc);memset( szDesc, 0, sizeof(szDesc) );
					lResult = RegQueryValueEx(hSubKey_2, lpNTDesc, NULL, &dwType, (LPBYTE)szDesc, &dwSize);	
					if( lResult != ERROR_SUCCESS )break;
					
					namegroup->nItemCnt++;
					strcpy( namegroup->szName[namegroup->nItemCnt-1], szDesc );
				}

				dwSubKeyIndex++;
			}

			dwKeyIndex++;
		}
	}while(0);

	// TERMINATE REGISTRY KEY
	RegCloseKey( hMainKey );
	RegCloseKey( hSubKey_1 );
	RegCloseKey( hSubKey_2 );	
}

void KSystemInfo::GetHddInfo_Win9x( NAMEGROUP *namegroup )
{
	HKEY		hMainKey, hSubKey_1, hSubKey_2;
	long		lResult, lResult1;
	DWORD		dwKeyIndex, dwSubKeyIndex ;
	DWORD		dwSize, dwType;
	LPSTR		lpWin9xKeyword	= "CLASS";
	LPSTR		lpWin9xService	= "DISKDRIVE";	
	LPSTR		lpWin9xSubKey	= "ENUM\\ESDI";
	LPSTR		lpWin9xDesc		= "DEVICEDESC";
	char		szRegPath[1024], szSubRegPath[1024], szEnumName[1024], szClass[1024], szDesc[1024];

	// GROBAL INIT
	namegroup->nItemCnt = 0;

	// ONLY ONE LOOP FOR ANYTIME BREAK :P //
	do{
		// INIT VARIABLE
		dwKeyIndex = 0;
		lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpWin9xSubKey, 0 , KEY_ENUMERATE_SUB_KEYS|KEY_EXECUTE|KEY_QUERY_VALUE, &hMainKey);		
		if (lResult != ERROR_SUCCESS)break;

		// REAL LOOP FOR FINDING DISKDRIVE NAME
		while( 1 )
		{
			// INIT PATH BUFF			
			strcpy( szRegPath, "ESDI" );

			lResult = RegEnumKey( hMainKey , dwKeyIndex, szEnumName, MAX_PATH + 1);						
			lResult1 = RegOpenKeyEx(hMainKey, szEnumName, 0, KEY_READ, &hSubKey_1);
			if (lResult == ERROR_NO_MORE_ITEMS || lResult1 != ERROR_SUCCESS )break;						

			// ADD PATH
			wsprintf( szRegPath, "%s\\%s", szRegPath, szEnumName );

			dwSubKeyIndex = 0;
			while(1)
			{
				lResult = RegEnumKey( hSubKey_1, dwSubKeyIndex, szEnumName, MAX_PATH + 1);						
				lResult1 = RegOpenKeyEx(hSubKey_1, szEnumName, 0, KEY_READ, &hSubKey_2);
				if (lResult == ERROR_NO_MORE_ITEMS || lResult1 != ERROR_SUCCESS )break;						

				// ADD PATH
				wsprintf( szSubRegPath, "%s\\%s", szRegPath, szEnumName );

				// CHECK WHETHER CLASS IS !!DISCDRIVE!!
				dwType = REG_SZ;dwSize = sizeof(szClass);memset( szClass, 0, sizeof(szClass) );
				lResult = RegQueryValueEx(hSubKey_2, lpWin9xKeyword, NULL, &dwType, (LPBYTE)szClass, &dwSize);	
				if( lResult != ERROR_SUCCESS ){
					dwSubKeyIndex++;
					continue;
				}

				if( _strnicmp( szClass, lpWin9xService, strlen(szClass) ) ){
					dwSubKeyIndex++;
					continue;
				}

				if( !IsDynamicDeviceOnWin9x( szSubRegPath ) ){
					dwSubKeyIndex++;
					continue;
				}

				//GET DISPLAY NAME			
				dwType = REG_SZ;dwSize = sizeof(szDesc);memset( szDesc, 0, sizeof(szDesc) );
				lResult = RegQueryValueEx(hSubKey_2, lpWin9xDesc, NULL, &dwType, (LPBYTE)szDesc, &dwSize);	
				if( lResult != ERROR_SUCCESS )break;
							
				strcpy( namegroup->szName[namegroup->nItemCnt], szDesc );						
				namegroup->nItemCnt++;

				dwSubKeyIndex++;
			}

			dwKeyIndex++;
		}

	}while(0);
	
	// TERMINATE REGISTRY KEY
	RegCloseKey( hMainKey );
	RegCloseKey( hSubKey_1 );
	RegCloseKey( hSubKey_2 );
}

void KSystemInfo::GetVgaInfo_Win9x( NAMEGROUP *namegroup )
{
	HKEY		hMainKey, hSubKey_1, hSubKey_2;
	long		lResult, lResult1;
	DWORD		dwKeyIndex, dwSubKeyIndex ;
	DWORD		dwSize, dwType;
	LPSTR		lpWin9xKeyword	= "CLASS";
	LPSTR		lpWin9xService	= "DISPLAY";	
	LPSTR		lpWin9xSubKey	= "ENUM\\PCI";
	LPSTR		lpWin9xDesc		= "DEVICEDESC";
	char		szRegPath[1024], szSubRegPath[1024], szEnumName[1024], szClass[1024], szDesc[1024];

	memset( szRegPath, 0, sizeof(szRegPath) );
	memset( szSubRegPath, 0, sizeof(szRegPath) );
	memset( szEnumName, 0, sizeof(szRegPath) );
	memset( szClass, 0, sizeof(szRegPath) );
	memset( szDesc, 0, sizeof(szRegPath) );

	// GROBAL INIT
	namegroup->nItemCnt = 0;
	// clear namegroup
	
	// ONLY ONE LOOP FOR ANYTIME BREAK :P //
	do{
		// INIT VARIABLE
		dwKeyIndex = 0;
		lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpWin9xSubKey, 0 , KEY_ENUMERATE_SUB_KEYS|KEY_EXECUTE|KEY_QUERY_VALUE, &hMainKey);		
		if (lResult != ERROR_SUCCESS)break;

		// REAL LOOP FOR FINDING DISPLAY NAME
		while( 1 )
		{
			// INIT PATH BUFF			
			strcpy( szRegPath, "PCI" );

			lResult = RegEnumKey( hMainKey , dwKeyIndex, szEnumName, MAX_PATH + 1);						
			lResult1 = RegOpenKeyEx(hMainKey, szEnumName, 0, KEY_READ, &hSubKey_1);
			if (lResult == ERROR_NO_MORE_ITEMS || lResult1 != ERROR_SUCCESS )break;						

			// ADD PATH
			wsprintf( szRegPath, "%s\\%s", szRegPath, szEnumName );

			memset( szSubRegPath, 0, sizeof(szSubRegPath) );
			dwSubKeyIndex = 0;
			while(1)
			{
				lResult = RegEnumKey( hSubKey_1, dwSubKeyIndex, szEnumName, MAX_PATH + 1);						
				lResult1 = RegOpenKeyEx(hSubKey_1, szEnumName, 0, KEY_READ, &hSubKey_2);
				if (lResult == ERROR_NO_MORE_ITEMS || lResult1 != ERROR_SUCCESS )break;						

				// ADD PATH
				wsprintf( szSubRegPath, "%s\\%s", szRegPath, szEnumName );

				// CHECK WHETHER CLASS IS !!DISPLAY!!
				dwType = REG_SZ;dwSize = sizeof(szClass);memset( szClass, 0, sizeof(szClass) );
				lResult = RegQueryValueEx(hSubKey_2, lpWin9xKeyword, NULL, &dwType, (LPBYTE)szClass, &dwSize);	
				if( lResult != ERROR_SUCCESS ){
					dwSubKeyIndex++;
					continue;
				}

				if( _strnicmp( szClass, lpWin9xService, strlen(szClass) ) )break;				

				if( !IsDynamicDeviceOnWin9x( szSubRegPath ) ){
					dwSubKeyIndex++;
					continue;
				}

				//GET DISPLAY NAME			
				dwType = REG_SZ;dwSize = sizeof(szDesc);memset( szDesc, 0, sizeof(szDesc) );
				lResult = RegQueryValueEx(hSubKey_2, lpWin9xDesc, NULL, &dwType, (LPBYTE)szDesc, &dwSize);	
				if( lResult != ERROR_SUCCESS )break;
								
				strcpy( namegroup->szName[namegroup->nItemCnt], szDesc );				
				namegroup->nItemCnt++;				

				dwSubKeyIndex++;
			}

			dwKeyIndex++;
		}

	}while(0);
	
	// TERMINATE REGISTRY KEY
	RegCloseKey( hMainKey );
	RegCloseKey( hSubKey_1 );
	RegCloseKey( hSubKey_2 );
}

void KSystemInfo::GetVgaInfo_WinNT( NAMEGROUP *namegroup )
{
	BOOL		bVerify;
	DWORD		dwKeyIndex, dwSubKeyIndex, dwVerifyLoop;
	DWORD		dwSize, dwType;
	char		szClass[1024], szDesc[1024];
	char		szEnumName[1024];
	long		lResult, lResult1;
	HKEY		hMainKey, hSubKey_1, hSubKey_2;
	LPSTR		lpNTSubKey = "SYSTEM\\CURRENTCONTROLSET\\ENUM\\PCI";
	LPSTR		lpNTKeyword = "CLASS";
	LPSTR		lpNTService = "DISPLAY";
	LPSTR		lpNTVerifyName = "CONTROL";
	LPSTR		lpNTDesc = "DEVICEDESC";

	// GROBAL INIT
	namegroup->nItemCnt = 0;

	// ONLY ONE LOOP FOR ANYTIME BREAK :P //
	do{
		// INIT VARIABLE
		dwKeyIndex = 0;
		lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpNTSubKey, 0 , KEY_ENUMERATE_SUB_KEYS|KEY_EXECUTE|KEY_QUERY_VALUE, &hMainKey);		
		if (lResult != ERROR_SUCCESS)break;

		// REAL LOOP FOR FINDING DISPLAY NAME
		while( 1 )
		{
			lResult = RegEnumKey( hMainKey , dwKeyIndex, szEnumName, MAX_PATH + 1);						
			lResult1 = RegOpenKeyEx(hMainKey, szEnumName, 0, KEY_READ, &hSubKey_1);
			if (lResult == ERROR_NO_MORE_ITEMS || lResult1 != ERROR_SUCCESS )break;						
			
			dwSubKeyIndex = 0;
			while(1)
			{
				lResult = RegEnumKey( hSubKey_1, dwSubKeyIndex, szEnumName, MAX_PATH + 1);						
				lResult1 = RegOpenKeyEx(hSubKey_1, szEnumName, 0, KEY_READ, &hSubKey_2);
				if (lResult == ERROR_NO_MORE_ITEMS || lResult1 != ERROR_SUCCESS )break;						
				
				// CHECK WHETHER CLASS IS !!DISPLAY!!
				dwType = REG_SZ;dwSize = sizeof(szClass);memset( szClass, 0, sizeof(szClass) );
				lResult = RegQueryValueEx(hSubKey_2, lpNTKeyword, NULL, &dwType, (LPBYTE)szClass, &dwSize);				

				if( lResult != ERROR_SUCCESS ){
					dwSubKeyIndex++;
					continue;
				}

				if( _strnicmp( szClass, lpNTService, strlen(szClass) ) ){
					dwSubKeyIndex++;
					continue;
				}

				// INIT SECOND LOOP
				dwVerifyLoop = 0;
				bVerify = TRUE;
				// SECOND LOOP FOR VERIFYING IT IS USING DEVICE
				while( 1 )
				{
					lResult = RegEnumKey( hSubKey_2, dwVerifyLoop, szEnumName, MAX_PATH + 1);										
					if (lResult == ERROR_NO_MORE_ITEMS  ){
						bVerify = FALSE;
						break;						
					}
					
					if( !_strnicmp( szEnumName, lpNTVerifyName, strlen(szEnumName) ) ){
						bVerify = TRUE;
						break;
					}

					dwVerifyLoop++;
				}

				if( bVerify ){
					// GET DISPLAY`NANE
					dwType = REG_SZ;dwSize = sizeof(szDesc);memset( szDesc, 0, sizeof(szDesc) );
					lResult = RegQueryValueEx(hSubKey_2, lpNTDesc, NULL, &dwType, (LPBYTE)szDesc, &dwSize);	
					if( lResult != ERROR_SUCCESS )break;
					
					namegroup->nItemCnt++;
					strcpy( namegroup->szName[namegroup->nItemCnt-1], szDesc );				
				}

				dwSubKeyIndex++;
			}

			dwKeyIndex++;
		}		
	}while( 0 );

	// TERMINATE REGISTRY KEY
	RegCloseKey( hMainKey );
	RegCloseKey( hSubKey_1 );
	RegCloseKey( hSubKey_2 );
}

BOOL KSystemInfo::IsDynamicDeviceOnWin9x(char *szRegPath)
{	
	DWORD		dwKeyIndex;
	DWORD		dwSize, dwType;
	char		szHardwareKey[1024];
	char		szEnumName[1024];
	long		lResult, lResult1;
	HKEY		hMainKey, hSubKey;
	LPSTR		lpWin9xSubKey = "CONFIG MANAGER\\ENUM";
	LPSTR		lpWin9xKeyword = "HARDWAREKEY";
	
	// ONLY ONE LOOP FOR ANYTIME BREAK :P //
	do{
		// INIT VARIABLE
		dwKeyIndex = 0;
		lResult = RegOpenKeyEx(HKEY_DYN_DATA, lpWin9xSubKey, 0 , KEY_ENUMERATE_SUB_KEYS|KEY_EXECUTE|KEY_QUERY_VALUE, &hMainKey);		
		if (lResult != ERROR_SUCCESS)break;

		// REAL LOOP FOR FINDING DISPLAY NAME
		while( 1 )
		{
			lResult = RegEnumKey( hMainKey , dwKeyIndex, szEnumName, MAX_PATH + 1);						
			lResult1 = RegOpenKeyEx(hMainKey, szEnumName, 0, KEY_READ, &hSubKey );
			if (lResult == ERROR_NO_MORE_ITEMS || lResult1 != ERROR_SUCCESS )break;						
			
			// CHECK WHETHER HARDWARE KEY IS !!szRegPath!!
			dwType = REG_SZ;dwSize = sizeof(szHardwareKey);memset( szHardwareKey, 0, sizeof(szHardwareKey) );
			lResult = RegQueryValueEx(hSubKey, lpWin9xKeyword, NULL, &dwType, (LPBYTE)szHardwareKey, &dwSize);	
			if( lResult != ERROR_SUCCESS ){
				dwKeyIndex++;
				continue;
			}

			if( _strnicmp( szRegPath, szHardwareKey, strlen(szRegPath) ) ){
				dwKeyIndex++;
				continue;
			}

			// SAME NAME RETURN TRUE
			// TERMINATE REGISTRY KEY
			RegCloseKey( hMainKey );
			RegCloseKey( hSubKey );				
			return TRUE;
		}
	}while( 0 );

	// TERMINATE REGISTRY KEY
	RegCloseKey( hMainKey );
	RegCloseKey( hSubKey );		
	return FALSE;
}

MEMORYINFO KSystemInfo::GetMemoryInfo()
{
	MEMORYSTATUS memstatus;

	// memset (&memstatus, sizeof (MEMORYSTATUS), 0);
	memstatus.dwLength = sizeof(MEMORYSTATUS);
	
	GlobalMemoryStatus(&memstatus);

	meminfo.nUsingPercent = memstatus.dwMemoryLoad/1024;
	meminfo.dwPhysicalFree = memstatus.dwAvailPhys/1024;
	meminfo.dwPhysicalTotal = memstatus.dwTotalPhys/1024;
	meminfo.dwVirtualFree = memstatus.dwAvailVirtual/1024;
	meminfo.dwVirtualTotal = memstatus.dwTotalVirtual/1024;

	return meminfo;
}

BOOL KSystemInfo::GetComputerName( LPSTR lpBuf )
{	
	DWORD dwSize = MAX_COMPUTERNAME_LENGTH + 1;
	return ::GetComputerName( lpBuf, &dwSize );
}




#define _MEGA_BYTE		1048576				//	1MB
#define _4GIGA_BYTE		4294967296			//	4GB
#define _1GIGA_BYTE		1073741824			//	1GB
/*
BOOL KSystemInfo::GetDiskSize( LPSTR lpBuf )
{
	KBuffer		buf;

	if ( !GetDriveString( &buf ) ) return FALSE;

	int	nCount = buf.GetTotalCount();

	DWORD	dwTotalSize	= 0;

	for ( int nIndex = 1 ; nIndex <= nCount ; nIndex++ )
	{
		LPSTR	lpDrv = (LPSTR)buf.GetBuffer( nIndex );

		if ( !lpDrv ) continue;

		ULARGE_INTEGER	ulAvailable, ulTotal, ulFree;

		if ( !GetDiskFreeSpaceEx( lpDrv, &ulAvailable, &ulTotal, &ulFree) ) continue;

		DWORD	dwSize = 0;

		if ( !ulTotal.HighPart )
			dwSize = ( ulTotal.LowPart / _MEGA_BYTE  );

		else {
			DWORD	dwHigh	= ( ( _4GIGA_BYTE / _MEGA_BYTE ) * ulTotal.HighPart );
			DWORD	dwLow	= ( ulTotal.LowPart / _MEGA_BYTE );
			dwSize	= dwHigh + dwLow;
		}

		TCHAR	szSize[128];
		wsprintf( szSize, "%s : %dMB", lpDrv, dwSize );

		dwTotalSize += dwSize;
	}

	TCHAR	szView[128];
	wsprintf( szView, "%d", dwTotalSize );

	int	nLen = strlen( szView );

	TCHAR	szBuf[32], szRem[32];
	ZeroMemory( szBuf, sizeof( szBuf ) );
	ZeroMemory( szRem, sizeof( szRem ) );

	switch( nLen )
	{
	case 1:		//	MB
	case 2:
	case 3:
		memcpy( szBuf, szView, nLen );
		wsprintf( lpBuf, "%sMB", szBuf );
		break;
	case 4:		//	GB
	case 5:
	case 6:
		memcpy( szBuf, szView, nLen - 3 );
		memcpy( szRem, &szView[nLen -3], 2 );
		wsprintf( lpBuf, "%s.%sGB", szBuf, szRem );
		break;
	case 7:		//	TB
	case 8:
	case 9:
		memcpy( szBuf, szView, nLen - 6 );
		memcpy( szRem, &szView[nLen -6], 2 );
		wsprintf( lpBuf, "%s.%sTB", szBuf, szRem );
		break;

	default:
		strcpy( lpBuf, "UnKnown DiskSize" );
	}

	return TRUE;
}
/**/

/*
DWORD KSystemInfo::GetDriveString( KBuffer *pBuf )
{
	TCHAR	szBuf[1024];
	ZeroMemory( szBuf, sizeof( szBuf ) );

	if ( !GetLogicalDriveStrings( sizeof( szBuf ), szBuf ) )
		return 0;

	for ( int nIndex = 0 ; nIndex < 1024 ; nIndex += 4 )
	{
		TCHAR	szDrv[16];
		ZeroMemory( szDrv, sizeof( szDrv ) );

		if ( !szBuf[nIndex] )
			break;

		strcpy( szDrv, &szBuf[nIndex] );

		UINT	uType = GetDriveType( szDrv );

		//	하드디스크가 아니라면 필요없다.
		if ( uType != DRIVE_FIXED )
			continue;

		pBuf->Add( szDrv, ( strlen( szDrv ) +1 ) );
	}

	return pBuf->GetTotalCount();
}
/**/

BOOL KSystemInfo::GetCPUInfo( LPSTR lpBuf )
{

	DWORD	dwSpd = calculateCpuSpeed();

//	TCHAR	szSpd[32];
//	wsprintf( szSpd, "%d", dwSpd );
//	AfgMessageBox( szSpd );

#define CPU_KEY "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"
#define CPU_VAL "ProcessorNameString"
#define CPU_VAL2 "Identifier"

	HKEY	hKey;
	if ( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, CPU_KEY, 0, KEY_READ, &hKey ) )
		return FALSE;

	TCHAR	szBuf[512];
	ZeroMemory( szBuf, sizeof( szBuf ) );

	DWORD	dwType	= REG_SZ,
			dwSize	= sizeof( szBuf );

	if ( ERROR_SUCCESS != RegQueryValueEx( hKey, CPU_VAL, NULL, &dwType, (LPBYTE)szBuf, &dwSize ) )
	{
		if ( ERROR_SUCCESS != RegQueryValueEx( hKey, CPU_VAL2, NULL, &dwType, (LPBYTE)szBuf, &dwSize ) )
		{
            RegCloseKey( hKey );
			return FALSE;
		}
	} 
	RegCloseKey( hKey );

	strcpy( lpBuf, szBuf );

	return TRUE;
}


DWORD KSystemInfo::calculateCpuSpeed()
{
   int   nTimeStart = 0;
   int   nTimeStop = 0;
   DWORD dwStartTicks = 0;
   DWORD dwEndTicks = 0;
   DWORD dwTotalTicks = 0;
   DWORD dwCpuSpeed = 0;

   nTimeStart = timeGetTime();

   for(;;)
   {
      nTimeStop = timeGetTime();

      if ((nTimeStop - nTimeStart) > 1)
      {
         dwStartTicks = determineTimeStamp();
         break;
      }
   }

   nTimeStart = nTimeStop;

   for(;;)
   {
      nTimeStop = timeGetTime();
      if ((nTimeStop - nTimeStart) > 500)    // one-half second
      {
         dwEndTicks = determineTimeStamp();
         break;
      }
   }

   dwTotalTicks = dwEndTicks - dwStartTicks;
   dwCpuSpeed = dwTotalTicks / 500000;

   return (dwCpuSpeed);
}

DWORD KSystemInfo::determineTimeStamp()
{
   DWORD dwTickVal;

   __asm
   {
      _emit 0Fh
      _emit 31h
      mov   dwTickVal, eax
   }

   return (dwTickVal);
}

/*
BOOL KSystemInfo::GetCPUEx( LPSTR lpCPU )
{
	DWORD	dwSpd = calculateCpuSpeed();

	TCHAR	szSpd[128];
	if ( dwSpd >= 1024 )
	{
		TCHAR	szRem[8];
		div_t	dvSpd = div( (int)dwSpd, 1024 );
		wsprintf( szRem, "%d", dvSpd.rem );

		if ( ( (int)strlen( szRem ) ) >= 2 )
			wsprintf( szSpd, "%d.%c%cGhz", dvSpd.quot, szRem[0], szRem[1] );
		else
			wsprintf( szSpd, "%d.%cGhz", dvSpd.quot, szRem[0] );
	}
	else
		wsprintf( szSpd, "%dMhz", dwSpd );

    _p_info info;
	ZeroMemory( &info, sizeof( _p_info ) );

	_cpuid ( &info );

	if ( info.v_name[0] )
		wsprintf( lpCPU, "(%s) %s %s", info.v_name, info.model_name, szSpd );
	else
		wsprintf( lpCPU, "%s %s", info.model, szSpd );

    return TRUE;
}
/**/

int KSystemInfo::IsCPUID()
{
    __try {
        _asm {
            xor eax, eax
            cpuid
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return 0;
    }
    return 1;
}


/***
* int _os_support(int feature)
*   - Checks if OS Supports the capablity or not
*
* Entry:
*   feature: the feature we want to check if OS supports it.
*
* Exit:
*   Returns 1 if OS support exist and 0 when OS doesn't support it.
*
****************************************************************/
/*
int KSystemInfo::_os_support(int feature)
{
    __try {
        switch (feature) {
        case _CPU_FEATURE_SSE:
            __asm {
                xorps xmm0, xmm0        // executing SSE instruction
            }
            break;
        case _CPU_FEATURE_SSE2:
            __asm {
                xorpd xmm0, xmm0        // executing SSE2 instruction
            }
            break;
        case _CPU_FEATURE_3DNOW:
            __asm {
                pfrcp mm0, mm0          // executing 3DNow! instruction
                emms
            }
            break;
        case _CPU_FEATURE_MMX:
            __asm {
                pxor mm0, mm0           // executing MMX instruction
                emms
            }
            break;
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        if (_exception_code() == STATUS_ILLEGAL_INSTRUCTION) {
            return 0;
        }
        return 0;
    }
    return 1;
}
/**/



/***
*
* void map_mname(int, int, const char *, char *)
*   - Maps family and model to processor name
*
****************************************************/


void KSystemInfo::map_mname(int family, int model, const char *v_name, char *m_name)
{
    // Default to name not known
    m_name[0] = '\0';

    if (!strncmp("AuthenticAMD", v_name, 12)) {
        switch (family) { // extract family code
        case 4: // Am486/AM5x86
            strcpy (m_name, "AMD Am486");
            break;

        case 5: // K6
            switch (model) { // extract model code
            case 0:
            case 1:
            case 2:
            case 3:
                strcpy (m_name, "AMD K5");
                break;
            case 6:
            case 7:
                strcpy (m_name, "AMD K6");
                break;
            case 8:
                strcpy (m_name, "AMD K6-2");
                break;
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                strcpy (m_name, "AMD K6-3");
                break;
            }
            break;

        case 6: // Athlon
            // No model numbers are currently defined
            strcpy (m_name, "AMD ATHLON");
            break;
        }
    }
    else if (!strncmp("GenuineIntel", v_name, 12)) {
        switch (family) { // extract family code
        case 4:
            switch (model) { // extract model code
            case 0:
            case 1:
                strcpy (m_name, "INTEL 486DX");
                break;
            case 2:
                strcpy (m_name, "INTEL 486SX");
                break;
            case 3:
                strcpy (m_name, "INTEL 486DX2");
                break;
            case 4:
                strcpy (m_name, "INTEL 486SL");
                break;
            case 5:
                strcpy (m_name, "INTEL 486SX2");
                break;
            case 7:
                strcpy (m_name, "INTEL 486DX2E");
                break;
            case 8:
                strcpy (m_name, "INTEL 486DX4");
                break;
            }
            break;

        case 5:
            switch (model) { // extract model code
            case 1:
            case 2:
            case 3:
                strcpy (m_name, "INTEL Pentium");
                break;
            case 4:
                strcpy (m_name, "INTEL Pentium-MMX");
                break;
            }
            break;

        case 6:
            switch (model) { // extract model code
            case 1:
                strcpy (m_name, "INTEL Pentium-Pro");
                break;
            case 3:
            case 5:
                strcpy (m_name, "INTEL Pentium-II");
                break;  // actual differentiation depends on cache settings
            case 6:
                strcpy (m_name, "INTEL Celeron");
                break;
            case 7:
            case 8:
            case 10:
                strcpy (m_name, "INTEL Pentium-III");
                break;  // actual differentiation depends on cache settings
            }
            break;

        case 15 | (0x00 << 4): // family 15, extended family 0x00
			strcpy (m_name, "INTEL Pentium-IV");
            break;
        }
    }
    else if (!strncmp("CyrixInstead", v_name, 12)) {
        strcpy (m_name, "Cyrix");
    }
    else if (!strncmp("CentaurHauls", v_name, 12)) {
        strcpy (m_name, "Centaur");
    }

    if (!m_name[0]) {
        strcpy (m_name, "Unknown");
    }
}


/***
*
* int _cpuid (_p_info *pinfo)
*
* Entry:
*
*   pinfo: pointer to _p_info.
*
* Exit:
*
*   Returns int with capablity bit set even if pinfo = NULL
*
****************************************************/
/*

int KSystemInfo::_cpuid (_p_info *pinfo)
{
    DWORD dwStandard = 0;
    DWORD dwFeature = 0;
    DWORD dwMax = 0;
    DWORD dwExt = 0;
    int feature = 0;
    int os_support = 0;
    union {
        char cBuf[12+1];
        struct {
            DWORD dw0;
            DWORD dw1;
            DWORD dw2;
        } s;
    } Ident;

    if (!IsCPUID()) {
        return 0;
    }

    _asm {
        push ebx
        push ecx
        push edx

        // get the vendor string
        xor eax, eax
        cpuid
        mov dwMax, eax
        mov Ident.s.dw0, ebx
        mov Ident.s.dw1, edx
        mov Ident.s.dw2, ecx

        // get the Standard bits
        mov eax, 1
        cpuid
        mov dwStandard, eax
        mov dwFeature, edx

        // get AMD-specials
        mov eax, 80000000h
        cpuid
        cmp eax, 80000000h
        jc notamd
        mov eax, 80000001h
        cpuid
        mov dwExt, edx

notamd:
        pop ecx
        pop ebx
        pop edx
    }

    if (dwFeature & _MMX_FEATURE_BIT) {
        feature |= _CPU_FEATURE_MMX;
        if (_os_support(_CPU_FEATURE_MMX))
            os_support |= _CPU_FEATURE_MMX;
    }
    if (dwExt & _3DNOW_FEATURE_BIT) {
        feature |= _CPU_FEATURE_3DNOW;
        if (_os_support(_CPU_FEATURE_3DNOW))
            os_support |= _CPU_FEATURE_3DNOW;
    }
    if (dwFeature & _SSE_FEATURE_BIT) {
        feature |= _CPU_FEATURE_SSE;
        if (_os_support(_CPU_FEATURE_SSE))
            os_support |= _CPU_FEATURE_SSE;
    }
    if (dwFeature & _SSE2_FEATURE_BIT) {
        feature |= _CPU_FEATURE_SSE2;
        if (_os_support(_CPU_FEATURE_SSE2))
            os_support |= _CPU_FEATURE_SSE2;
    }

    if (pinfo) {
        memset(pinfo, 0, sizeof(_p_info));

        pinfo->os_support = os_support;
        pinfo->feature = feature;
        pinfo->family = (dwStandard >> 8) & 0xF; // retrieve family
        if (pinfo->family == 15) {               // retrieve extended family
            pinfo->family |= (dwStandard >> 16) & 0xFF0;
        }
        pinfo->model = (dwStandard >> 4) & 0xF;  // retrieve model
        if (pinfo->model == 15) {                // retrieve extended model
            pinfo->model |= (dwStandard >> 12) & 0xF;
        }
        pinfo->stepping = (dwStandard) & 0xF;    // retrieve stepping

        Ident.cBuf[12] = 0;
        strcpy(pinfo->v_name, Ident.cBuf);

        map_mname(pinfo->family, 
                  pinfo->model,
                  pinfo->v_name,
                  pinfo->model_name);

        pinfo->checks = _CPU_FEATURE_MMX |
                        _CPU_FEATURE_SSE |
                        _CPU_FEATURE_SSE2 |
                        _CPU_FEATURE_3DNOW;
    }

    return feature;
}
/**/
