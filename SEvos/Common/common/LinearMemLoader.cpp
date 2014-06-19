
#include "stdafx.h"
#include "linearmemloader.h"
#include <sys/stat.h>


typedef struct _tagSBaseType
{
	char name[ 32];
	char size;
} SBaseType;
static SBaseType g_BaseType[] = 
{
	{ "int", sizeof(int) }, 
	{ "float", sizeof(float) },
	{ "char", sizeof(char) },
	{ "short", sizeof(short) },
	{ "size", sizeof(int) },
	{ "dummy", sizeof(int) }
};
const int g_BaseTypeSize = sizeof(g_BaseType) / sizeof(SBaseType);

SBaseType* GetBaseType( char *pTypeName )
{
	for( int i=0; i < g_BaseTypeSize; ++i )
	{
		if( !strcmp(g_BaseType[ i].name, pTypeName) )
			return &g_BaseType[ i];
	}
	return NULL;
}


CLinearMemLoader::CLinearMemLoader()
{
	CreateDefaultToken();
}

CLinearMemLoader::CLinearMemLoader( char *szTokenFileName )
{
	CreateDefaultToken();
	if( szTokenFileName )
		LoadTokenFile( szTokenFileName );
}


CLinearMemLoader::~CLinearMemLoader()
{
	Clear();
}

void CLinearMemLoader::Clear()
{
	TokenItor i = m_STokenMap.begin();
	while( m_STokenMap.end() != i )
	{
		SType type = i++->second;
		if( type.pMember )
			delete type.pMember;
	}
	m_STokenMap.clear();
}


//-----------------------------------------------------------------------------//
// 기본적인건 미리 만들어 놓는다.
//-----------------------------------------------------------------------------//
void CLinearMemLoader::CreateDefaultToken()
{
	for( int i=0; i < g_BaseTypeSize; ++i )
		AddToken( g_BaseType[ i].name, g_BaseType[ i].size );
}


//-----------------------------------------------------------------------------//
// Token file 로드
// opt: 0 중복된 데이타가 발생하면 에러를 발생한다.
//		1 중복된 데이타가 발생하면 덮어 씌운다.
//-----------------------------------------------------------------------------//
BOOL CLinearMemLoader::LoadTokenFile( char *szTokenFileName, int opt )
{
	SDftDataParser parser;
	SDftDataList *pRoot = parser.OpenScriptFile( szTokenFileName );
	if( !pRoot ) return NULL;

	SDftDataList *pNode = pRoot->pChild;
	LoadTokenFileRec( pRoot->pChild, NULL, opt );

	return TRUE;
}


//-----------------------------------------------------------------------------//
// Token을 생성한다.
// opt: 0 중복된 데이타가 발생하면 에러를 발생한다.
//		1 중복된 데이타가 발생하면 덮어 씌운다.
//-----------------------------------------------------------------------------//
void CLinearMemLoader::LoadTokenFileRec( SDftDataList *pNode, MemberList *pTokenList, int opt )
{
	if( !pNode ) return;

	TokenItor itor = m_STokenMap.find( pNode->t->pStr[ 0] );

	if( itor != m_STokenMap.end() )
	{
		// 같은 데이타가 중복되어 선언되어 있다면 opt값에 따라 흐름이 결정된다.
		if( 0 == opt )
		{
			// error
			return;
		}
		else if( 1 == opt )
		{
			SType type = itor->second;
			type.pMember->clear();
			LoadTokenFile_Member( pNode->pChild, type.pMember );
			itor->second.size = GetStructSize( type.pMember );
		}

/*
		SBaseType *p = GetBaseType( pNode->t->pStr[ 0] );
		if( p )
		{
			if( pTokenList )
				pTokenList->push_back( SToken(p->name, p->size, TYPE_DATA) );
		}
		else
		{
			// custome structure 일경우
			SType type = itor->second;
			if( pTokenList )
				pTokenList->push_back( SToken(pNode->t->pStr[ 0], type.size, TYPE_STRUCT) );
		}
/**/
	}
	else
	{
/*
		// 배열
		if( strchr(pNode->t->pStr[ 0], '[') )
		{
			char *p1 = strchr( pNode->t->pStr[ 0], '[' );
			char *p2 = strchr( pNode->t->pStr[ 0], ']' );
			char szNum[ 16];
			ZeroMemory( szNum, sizeof(szNum) );
			strncpy( szNum, p1+1, (p2-p1)-1 );
			int nSize = atoi( szNum );
			char szToken[ 64];
			ZeroMemory( szToken, sizeof(szToken) );
			strncpy( szToken, pNode->t->pStr[ 0], (p1 - pNode->t->pStr[ 0]) );

			TokenItor ArrayItor = m_STokenMap.find( szToken );
			if( ArrayItor == m_STokenMap.end() )
				return;

			nSize *= ArrayItor->second.size; // Array Total size
			if( pTokenList )
				pTokenList->push_back( SToken(szToken, nSize, TYPE_ARRAY) );
		}
		// 포인터
		else if( strchr(pNode->t->pStr[ 0], '*') )
		{
			char *p1 = strchr( pNode->t->pStr[ 0], '*' );
			char szToken[ 64];
			ZeroMemory( szToken, sizeof(szToken) );
			strncpy( szToken, pNode->t->pStr[ 0], (p1 - pNode->t->pStr[ 0]) );

			if( pTokenList )
				pTokenList->push_back( SToken(szToken, sizeof(void*), TYPE_POINTER) );
		}
/**/
		// 새로운 구조체
//		else
		{
			list<SToken> *pNewList = new list<SToken>;
			//LoadTokenFileRec( pNode->pChild, pNewList, opt );
			LoadTokenFile_Member( pNode->pChild, pNewList );
			SType type( pNewList, GetStructSize(pNewList), IsPointer(pNewList) );
			m_STokenMap.insert( TokenMap::value_type(pNode->t->pStr[ 0], type) );
		}
	}

	LoadTokenFileRec( pNode->pNext, pTokenList, opt );

	return;
}


//-----------------------------------------------------------------------------//
// Custome struct의 멤버 토큰을 pTokenList에 저장한다.
//-----------------------------------------------------------------------------//
void CLinearMemLoader::LoadTokenFile_Member( SDftDataList *pNode, MemberList *pTokenList )
{
	SDftDataList *ptok = pNode;

	while( ptok )
	{
		TokenItor itor = m_STokenMap.find( ptok->t->pStr[ 0] );
		if( itor == m_STokenMap.end() )
		{
			// 배열
			if( strchr(ptok->t->pStr[ 0], '[') )
			{
				char *p1 = strchr( ptok->t->pStr[ 0], '[' );
				char *p2 = strchr( ptok->t->pStr[ 0], ']' );
				char szNum[ 16];
				ZeroMemory( szNum, sizeof(szNum) );
				strncpy_s( szNum, sizeof(szNum), p1+1, (p2-p1)-1 );
				int nSize = atoi( szNum );
				char szToken[ 64];
				ZeroMemory( szToken, sizeof(szToken) );
				strncpy_s( szToken, sizeof(szToken), ptok->t->pStr[ 0], (p1 - ptok->t->pStr[ 0]) );

				TokenItor ArrayItor = m_STokenMap.find( szToken );
				if( ArrayItor == m_STokenMap.end() )
					break;	// error

				nSize *= ArrayItor->second.size; // Array Total size
				pTokenList->push_back( SToken(szToken, nSize, TYPE_ARRAY) );
			}
			// 포인터
			else if( strchr(ptok->t->pStr[ 0], '*') )
			{
				char *p1 = strchr( ptok->t->pStr[ 0], '*' );
				char szToken[ 64];
				ZeroMemory( szToken, sizeof(szToken) );
				strncpy_s( szToken, sizeof(szToken), ptok->t->pStr[ 0], (p1 - ptok->t->pStr[ 0]) );
				pTokenList->push_back( SToken(szToken, sizeof(void*), TYPE_POINTER) );
			}
			else
			{
				// error
				break;
			}
		}
		else
		{
			SBaseType *p = GetBaseType( ptok->t->pStr[ 0] );
			if( p )
			{
				pTokenList->push_back( SToken(p->name, p->size, TYPE_DATA) );
			}
			else
			{
				// custome structure 일경우
				SType type = itor->second;
				pTokenList->push_back( SToken(ptok->t->pStr[ 0], type.size, TYPE_STRUCT) );
			}
		}

		ptok = ptok->pNext;
	}
}


//-----------------------------------------------------------------------------//
// 구조체 pStruct 정보를 szTokenName 타입으로 szFileName 파일에 쓴다.
// szFileName : Write할 파일이름
// pStruct: 정보를 저장하고있는 메모리
// szTokenName : 구조체 타입
//-----------------------------------------------------------------------------//
BOOL CLinearMemLoader::WriteBin( char *szFileName, void *pStruct, char *szTokenName )
{
	if( !pStruct ) return FALSE;

	FILE *fp;
	fopen_s( &fp, szFileName, "wb" );
	if( !fp ) return FALSE;

	WriteBin( fp, pStruct, szTokenName );
	fclose( fp );

	return TRUE;
}
BOOL CLinearMemLoader::WriteScript( char *szFileName, void *pStruct, char *szTokenName )
{
	if( !pStruct ) return FALSE;

	FILE *fp;
	fopen_s( &fp, szFileName, "w" );
	if( !fp ) return FALSE;
	fprintf( fp, "GPJ\n" );

	WriteScript( fp, pStruct, szTokenName, 0 );
	fclose( fp );

	return TRUE;
}

//-----------------------------------------------------------------------------//
// 구조체 pStruct 정보를 szTokenName 타입으로 파일포인터 fp에 쓴다.
// fp : Dest파일 포인터
// pStruct: 정보를 저장하고있는 메모리
// szTokenName : 구조체 타입
//-----------------------------------------------------------------------------//
BOOL CLinearMemLoader::WriteBin( FILE *fp, void *pStruct, char *szTokenName )
{
	queue<SWsp> WspQueue;
	WspQueue.push( SWsp(szTokenName, TYPE_DATA, 0, 0, (BYTE*)pStruct) );
	int nFileStartPos = ftell( fp );

	while( !WspQueue.empty() )
	{
		SWsp WspInfo = WspQueue.front(); WspQueue.pop();

		TokenItor titor = m_STokenMap.find( WspInfo.strToken );
		if( m_STokenMap.end() == titor ) break; // error !!
		SType type = titor->second;

		// file에 쓰기
		switch( WspInfo.eType )
		{
		case TYPE_ARRAY:
		case TYPE_DATA:
		case TYPE_STRUCT:
			{
				fwrite( WspInfo.pStruct, type.size, 1, fp );

				// struct에서 pointer 및 filepos 얻음
				CollectPointerRec( ftell(fp) - type.size, (BYTE*)WspInfo.pStruct, type.pMember, &WspQueue );
			}
			break;

		case TYPE_POINTER:
			{
				// 현재 filePos를 저장한다.
				DWORD dwCurPos = ftell( fp );
				DWORD dwPointerOffset = dwCurPos - nFileStartPos;
				fseek( fp, WspInfo.nFilePos, SEEK_SET ); // pointer값이 Update될곳으로 이동
				fwrite( &dwPointerOffset, sizeof(DWORD), 1, fp );
				fseek( fp, dwCurPos, SEEK_SET ); // 다시 돌아옴
				fwrite( WspInfo.pStruct, type.size, WspInfo.nPointerSize, fp );

				// struct에서 pointer 및 filepos 얻음
				dwCurPos = ftell( fp ) - (type.size * WspInfo.nPointerSize);
				for( int i=0; i < WspInfo.nPointerSize; ++i )
					CollectPointerRec( dwCurPos + (type.size*i), WspInfo.pStruct+(type.size*i), type.pMember, &WspQueue );
			}
			break;
		}
	}

	return TRUE;
}
//-----------------------------------------------------------------------------//
// 구조체포인터 pStruct를 szTokenName포맷에 맞게 스크립트로 저장한다.
//-----------------------------------------------------------------------------//
BOOL CLinearMemLoader::WriteScript( FILE *fp, void *pStruct, char *szTokenName, int tab )
{
	TokenItor titor = m_STokenMap.find( szTokenName );
	if( m_STokenMap.end() == titor ) return FALSE; // error !!
	SType type = titor->second;

	WriteTab( fp, tab );
	fprintf( fp, "%s ", szTokenName );

	BYTE *psrc = (BYTE*)pStruct;
	BOOL bchild = FALSE;

	MemberItor it = type.pMember->begin();
	while( type.pMember->end() != it )
	{
		SToken tok = *it++;
		switch( tok.eType )
		{
		case TYPE_NULL:
			break;

		case TYPE_DATA:
		case TYPE_ARRAY:
			{
				WriteTextData( fp, psrc, &tok );
				fprintf( fp, " " );

				psrc += tok.nSize;
			}
			break;

		case TYPE_STRUCT:
			{
				if( !bchild )
				{
					fprintf( fp, "\n" );
					WriteTab( fp, tab );
					fprintf( fp, "{" );
					fprintf( fp, "\n" );
					bchild = TRUE;
				}

				WriteScript( fp, psrc, (char*)tok.strToken.c_str(), tab+1 );

				psrc += tok.nSize;
			}
			break;

		case TYPE_POINTER:
			{
				if( !bchild )
				{
					fprintf( fp, "\n" );
					WriteTab( fp, tab );
					fprintf( fp, "{" );
					fprintf( fp, "\n" );
					bchild = TRUE;
				}

				// ** pointer type이 나오기전에 먼저 size값이 저장되어있다.
				int *psize = (int*)(psrc - 4); // pointersize 를 얻는다.
				BYTE* pPointerAdress = (BYTE*)*(DWORD*)psrc;

				TokenItor titor = m_STokenMap.find( tok.strToken );
				if( m_STokenMap.end() == titor ) break; // error !!
				SType type = titor->second;
				for( int i=0; i < *psize; ++i )				
					WriteScript( fp, (pPointerAdress+(type.size*i)), (char*)tok.strToken.c_str(), tab+1 );

				psrc += tok.nSize;
			}
			break;
		}
	}

	if( bchild )
	{
		WriteTab( fp, tab );
		fprintf( fp, "}" );
	}

	fprintf( fp, "\n" );

	return TRUE;
}


//-----------------------------------------------------------------------------//
// binary file로된 szFileName 파일을 szTokenName 타입으로 읽어드린다.
// 읽어드린 데이타는 BYTE* 리턴된다. (메모리제거는 직접해줘야함)
//-----------------------------------------------------------------------------//
BYTE* CLinearMemLoader::ReadBin( char *szFileName, char *szTokenName )
{
	FILE *fp;
	fopen_s( &fp, szFileName, "rb" );
	if( !fp ) return NULL;

	struct _stat buf;
	_stat( szFileName, &buf );
	int filesize = buf.st_size;	// 64 bit 

	BYTE *pRead = new BYTE[ filesize];
	fread( pRead, 1, filesize, fp );
	fclose( fp );

	if( !ReadBin(pRead, szTokenName) )
	{
		delete[] pRead;
		pRead = NULL;
	}

	return pRead;
}


//-----------------------------------------------------------------------------//
// pReadMem 에 저장된 데이타를 szTokenName 타입으로 읽어드린다.
// pReadMem : Write() 함수로 쓰여진 file 정보
// nOffset : file중간부터 CLinearMemLoader 클래스를 사용할때 필요한값.. 왠만하면 쓰지말자
// szTokenName : pReadMem form
// **읽어드린 byte수를 리턴할려고 했지만 속도최적화를 이유로 뺐다.**
//-----------------------------------------------------------------------------//
int CLinearMemLoader::ReadBin( BYTE *pReadMem, char *szTokenName )
{
	TokenItor itor = m_STokenMap.find( szTokenName );
	if( m_STokenMap.end() == itor ) return 0;

	// pointer offset값 보정
	ReadRec( (DWORD)pReadMem, pReadMem, itor->second.pMember );

	return 1;
}


//-----------------------------------------------------------------------------//
// Member변수 size를 구한다.
//-----------------------------------------------------------------------------//
int CLinearMemLoader::GetStructSize( MemberList *pList )
{
	int nSize = 0;
	MemberItor itor = pList->begin();
	while( itor != pList->end() )
	{
		nSize += itor->nSize;
		++itor;
	}
	return nSize;
}


//-----------------------------------------------------------------------------//
// Member중 pointer가 있는지 검사한다.
//-----------------------------------------------------------------------------//
BOOL CLinearMemLoader::IsPointer( MemberList *pList )
{
	MemberItor itor = pList->begin();
	while( itor != pList->end() )
	{
		if( TYPE_POINTER == itor->eType )
			return TRUE;
		else if( TYPE_STRUCT == itor->eType )
		{
			TokenItor titor = m_STokenMap.find( itor->strToken );
			if( m_STokenMap.end() == titor ) return FALSE; // error !!
			if( titor->second.pointer )
				return TRUE;
		}
		++itor;
	}
	return FALSE;
}


//-----------------------------------------------------------------------------//
// token 추가
// szTokenName: token name
// nSize : token의 byte수
// szParentTokenName : parent token name (이경우 parent의 child로 추가된다.)
//-----------------------------------------------------------------------------//
BOOL CLinearMemLoader::AddToken( char *szTokenName, int nSize, char *szParentTokenName ) // szParentTokenName = NULL
{
	if( szParentTokenName )
	{
		TokenItor titor = m_STokenMap.find( szParentTokenName );
		if( m_STokenMap.end() == titor ) return FALSE; // error!!
		titor->second.pMember->push_back( SToken(szTokenName, nSize, TYPE_DATA) );
	}
	else
	{
		list<SToken> *pNewList = new list<SToken>;
		pNewList->push_back( SToken(szTokenName, nSize, TYPE_DATA) );
		m_STokenMap.insert( TokenMap::value_type(szTokenName, SType(pNewList,nSize,FALSE)) );
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// pointer & filepos 찾기
// dwCurFilePos : pStruct 데이타가 저장된 filepointer
// pStruct : File에 쓰여진 Data Pointer
// pMList : pStruct 의 멤버리스트
//-----------------------------------------------------------------------------//
void CLinearMemLoader::CollectPointerRec( DWORD dwCurFilePos, BYTE *pStruct, MemberList *pMList, queue<SWsp> *pWspList )
{
	DWORD dwFilePos = dwCurFilePos;
	MemberItor mitor = pMList->begin();
	while( mitor != pMList->end() )
	{
		SToken t = *mitor;

		if( TYPE_POINTER == mitor->eType )
		{
			// ** pointer type이 나오기전에 먼저 size값이 저장되어 있어야한다.
			int *pSize = (int*)(pStruct - 4); // pointersize 를 얻는다.
			if( 0 < *pSize )
			{
				DWORD dwAddress = *(DWORD*)pStruct;
				pWspList->push( SWsp(mitor->strToken, TYPE_POINTER, dwFilePos, *pSize, (BYTE*)dwAddress ) );
			}
		}
		else if( TYPE_ARRAY == mitor->eType )
		{
			TokenItor titor = m_STokenMap.find( mitor->strToken );
			if( m_STokenMap.end() == titor ) break; // error!!

			// member변수중 pointer가 있을때만
			SType type = titor->second;
			if( type.pointer )
			{
				int nArraySize = mitor->nSize / type.size;
				for( int i=0; i < nArraySize; ++i )
					CollectPointerRec( dwFilePos + (type.size*i), (pStruct + (type.size*i)), type.pMember, pWspList );
			}
		}
		else if( TYPE_STRUCT == mitor->eType )
		{
			TokenItor titor = m_STokenMap.find( mitor->strToken );
			if( m_STokenMap.end() == titor ) break; // error!!

			// member변수중 pointer가 있을때만
			SType type = titor->second;
			if( type.pointer )
				CollectPointerRec( dwFilePos, pStruct, type.pMember, pWspList );
		}

		dwFilePos += mitor->nSize;
		pStruct += mitor->nSize;
		++mitor;
	}
}


//-----------------------------------------------------------------------------//
// pointer offset값 보정
// member변수를 하나씩검사해서 type이 pointer일경우 dwOffset값만큼 보정한다.
//-----------------------------------------------------------------------------//
void CLinearMemLoader::ReadRec( DWORD dwOffset, BYTE *pStruct, MemberList *pMList )
{
	MemberItor mitor = pMList->begin();
	while( mitor != pMList->end() )
	{
		SToken st = *mitor;

		if( TYPE_POINTER == mitor->eType )
		{
			// size값이 0 보다 클때만 보정함
			int *pSize = (int*)(pStruct - 4); // pointersize 를 얻는다.
 			if( 0 < *pSize )
			{
				*(DWORD*)pStruct += dwOffset; // Poiner값 보정
				BYTE* pPointerAdress = (BYTE*)*(DWORD*)pStruct;

				TokenItor titor = m_STokenMap.find( mitor->strToken );
				if( m_STokenMap.end() == titor ) break; // error !!
				SType type = titor->second;
				if( type.pointer )
				{
					for( int i=0; i < *pSize; ++i )
						ReadRec( dwOffset, (pPointerAdress+(type.size*i)), type.pMember );
				}
			}
		}
		else if( TYPE_ARRAY == mitor->eType )
		{
			TokenItor titor = m_STokenMap.find( mitor->strToken );
			if( m_STokenMap.end() == titor ) break; // error !!
			SType type = titor->second;
			if( type.pointer )
			{
				int nArraySize = mitor->nSize / type.size;
				for( int i=0; i < nArraySize; ++i )
					ReadRec( dwOffset, pStruct+(type.size*i), type.pMember );
			}
		}
		else if( TYPE_STRUCT == mitor->eType )
		{
			TokenItor titor = m_STokenMap.find( mitor->strToken );
			if( m_STokenMap.end() == titor ) break; // error !!
			SType type = titor->second;
			if( type.pointer )
				ReadRec( dwOffset, pStruct, type.pMember );
		}

		pStruct += mitor->nSize;
		++mitor;
	}
}


//-----------------------------------------------------------------------------//
// szFileName 파일을 szTokenName 타입으로 파징해서 메모리에 저장한다.
// szFileName : 스크립트 파일 경로
// szTokenName : 변환될 포맷
// szFileName 스크립트 파일을 열어 토큰트리를 분석하면서 선형 메모리로 변환
// 시킨다.
//-----------------------------------------------------------------------------//
BYTE* CLinearMemLoader::ReadScript( char *szFileName, char *szTokenName )
{
	SDftDataParser parser;
	SDftDataList *pRoot = parser.OpenScriptFile( szFileName );
	if( !pRoot ) return NULL;
	SDftDataList *pToken = CParser<SDftData>::FindToken( pRoot->pChild, SDftData(szTokenName) );
	if( !pToken ) return NULL;

	TokenItor titor = m_STokenMap.find( szTokenName );
	if( m_STokenMap.end() == titor ) return NULL;

	SType type = titor->second;
	BYTE *pStruct = new BYTE[ type.size];
	ZeroMemory( pStruct, type.size );

	ReadScriptRec( pStruct, pToken, type.pMember );

	return pStruct;
}


//-----------------------------------------------------------------------------//
// 코드가 조금 꼬였음.
// Script를 파징할때 Script driven 방식으로 파징한후 MemberList driven방식으로 파징한다. (꽁수)
//-----------------------------------------------------------------------------//
void CLinearMemLoader::ReadScriptRec( BYTE *pStruct, SDftDataList *pNode, MemberList *pMList )
{
	if( !pNode || !pStruct || !pMList ) return;

	// Line 파징
	int idx=1;
	MemberItor mitor = pMList->begin();
	while( pNode->t->pStr[ idx] && (pMList->end() != mitor) )
	{
		SToken t = *mitor;
		switch( t.eType )
		{
		case TYPE_DATA:
		case TYPE_ARRAY: // string
			idx = ParseData( pStruct, &t, pNode->t->pStr[ idx], pNode, idx ); break;
			break;

		case TYPE_POINTER:
			{
				// 데이타 갯수를 얻는다.
				int count = 0;
				while( pNode->t->pStr[ idx + count] )
					++count;

				if( 0 < count )
				{
					TokenItor titor = m_STokenMap.find( t.strToken );
					if( m_STokenMap.end() == titor ) break; // error !!
					SType type = titor->second;

					BYTE *pSubMem = new BYTE[ count * type.size];
					ZeroMemory( pSubMem, count * type.size );

					for( int i=0; i < count; ++i )
						ParseData( pSubMem+(i*type.size), &t, pNode->t->pStr[ idx+i] );

					// memory주소값 설정
					*(DWORD*)pStruct = (DWORD)pSubMem;
					// pointer size 값 설정
					*(int*)(pStruct-4) = count;
				}

				idx += count;
			}
			break;

		default:
			++idx;
			break;
		}

		pStruct += t.nSize;
		++mitor;
	}

	// Child Line 파징
	pNode = pNode->pChild;
	while( pNode && (pMList->end() != mitor) )
	{
		switch( mitor->eType )
		{
		// 꽁수시작...
		// TYPE_DATA일경우 (size, structure) 2가지 경우가 존재한다.
		case TYPE_DATA:
		case TYPE_STRUCT:
			{
				// 다음타입이 TYPE_POINTER일경우 현재 TYPE은 size를 가르키므로 무시하고 넘어간다.
				// 그렇지 않다면 Structure일경우이므로 파징한다.
				SToken t = *mitor;
				MemberItor temp = mitor;
				++temp;

				// size type 일경우
				if( (pMList->end() != temp) && TYPE_POINTER == temp->eType )
				{
					// 아무일 없음
					pNode = pNode->pNext;
				}
				// structure 일경우
				else
				{
					TokenItor titor = m_STokenMap.find( mitor->strToken );
					if( m_STokenMap.end() == titor ) break; // error !!
					SType type = titor->second;
					ReadScriptRec( pStruct, pNode, type.pMember );
					pNode = pNode->pNext;
				}
			}
			break;

		case TYPE_ARRAY:
			{
				SToken t = *mitor;
				TokenItor titor = m_STokenMap.find( mitor->strToken );
				if( m_STokenMap.end() == titor ) break; // error !!
				SType type = titor->second;
				int nArraySize = mitor->nSize / type.size;
				for( int i=0; (i < nArraySize) && pNode; ++i )
				{
					ReadScriptRec( pStruct + (i*type.size), pNode, type.pMember );
					pNode = pNode->pNext;
				}
			}
			break;

		case TYPE_POINTER:
			{
				SToken t = *mitor;
				TokenItor titor = m_STokenMap.find( mitor->strToken );
				if( m_STokenMap.end() == titor ) break; // error !!

				SType type = titor->second;
				int nCount = CParser<SDftData>::GetNodeCount( pNode, SDftData((char*)mitor->strToken.c_str()) );
				if( 0 < nCount )
				{
					BYTE *pSubMem = new BYTE[ nCount * type.size];
					ZeroMemory( pSubMem, nCount * type.size );

					for( int i=0; (i < nCount) && pNode; ++i )
					{
						ReadScriptRec( pSubMem + (i*type.size), pNode, type.pMember );
						pNode = pNode->pNext;
					}

					// memory주소값 설정
					*(DWORD*)pStruct = (DWORD)pSubMem;
					// pointer size 값 설정
					*(int*)(pStruct-4) = nCount;
				}
			}
			break;
		}

		pStruct += mitor->nSize;
		++mitor;
	}
}


//-----------------------------------------------------------------------------//
// token parse
//-----------------------------------------------------------------------------//
int CLinearMemLoader::ParseData( BYTE *pStruct, SToken *pToken, char *szToken, SDftDataList *pNode, int idx )
{
	if( pToken->strToken == "int" )
	{
		if( TYPE_ARRAY == pToken->eType )
		{
			int size = pToken->nSize;
			while( 0 < size )
			{
				if( !pNode->t->pStr[ idx] )	*(int*)pStruct = 0;
				else						*(int*)pStruct = atoi( pNode->t->pStr[ idx] );
				++idx;
				size -= sizeof(int);
				pStruct += sizeof(int);
			}
			--idx;
		}
		else
		{
			*(int*)pStruct = atoi( szToken );
		}
	}
	else if( pToken->strToken == "float" )
	{
		*(float*)pStruct = (float)atof( szToken );
	}
	else if( pToken->strToken == "short" )
	{
		*(short*)pStruct = (short)atoi( szToken );
	}
	else if( pToken->strToken == "char" )
	{
		if( TYPE_ARRAY == pToken->eType )
		{
			strcpy_s( (char*)pStruct, strlen(szToken)+1, szToken );
		}
		else
		{
			*(char*)pStruct = *szToken;
		}
	}
	else if( pToken->strToken == "size" )
	{
		// script를 읽어드릴때 size값은 자동으로 설정된다.
	}
	else if( pToken->strToken == "dummy" )
	{
		// 아무일도 없다.
	}

	return ++idx;
}


//-----------------------------------------------------------------------------//
// 파일포인터 fp에 토큰타입에 맞게 텍스트형태로 저장한다.
//-----------------------------------------------------------------------------//
void CLinearMemLoader::WriteTextData( FILE *fp, BYTE *pSrc, SToken *pTok )
{
	if( pTok->strToken == "int" )
	{
		if( TYPE_ARRAY == pTok->eType )
		{
			int size = pTok->nSize;
			while( 0 < size )
			{
				fprintf( fp, "%d ", *(int*)pSrc );
				size -= sizeof(int);
				pSrc += sizeof(int);
			}
		}
		else
		{
			fprintf( fp, "%d", *(int*)pSrc );
		}
	}
	else if( pTok->strToken == "float" )
	{
		fprintf( fp, "%f", *(float*)pSrc );
	}
	else if( pTok->strToken == "short" )
	{
		fprintf( fp, "%d", *(short*)pSrc );
	}
	else if( pTok->strToken == "char" )
	{
		if( TYPE_ARRAY == pTok->eType )
		{
			fprintf( fp, "\"%s\"", (char*)pSrc );
		}
		else
		{
			fprintf( fp, "%c", *(char*)pSrc );
		}
	}
	else if( pTok->strToken == "size" )
	{
		fprintf( fp, "%d", *(int*)pSrc );
	}
	else if( pTok->strToken == "dummy" )
	{
		// 아무일도 없다.
	}

}


//-----------------------------------------------------------------------------//
// Tab 갯수만큼 이동
//-----------------------------------------------------------------------------//
void CLinearMemLoader::WriteTab( FILE *fp, int tab )
{
	for( int t=0; t < tab; ++t )
		fprintf( fp, "\t" );
}
