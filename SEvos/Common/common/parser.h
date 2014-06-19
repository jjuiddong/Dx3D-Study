
/*
	Script Parser 클래스

	2003-03-11	programer: jaejung
	2003-09-19	template 클래스로 수정
	2005-01-11	Token 분리하기 쉽게 수정 (macro 추가)

	2007-12-26	주석기능 추가 '$'
	
	2010-06-20 : vs2005로 컴파일 할 수있게 수정했다.

	2011-02-23 : 메모리 누수 제거 DeleteParseList()에 버그가 있었음
*/

#if !defined(__PARSER_H__)
#define __PARSER_H__

#include <windows.h>
#include <stdio.h>
#include "parserdef.h"


//////////////////////////////////////////////////////////////
// Class
template< class T >
class CParser
{
public:
	CParser(): m_pRoot(NULL) { }
	virtual ~CParser() { DeleteParseList(m_pRoot); }
public:
	// DataList
	typedef struct _tagPARSELIST
	{
		T *t;
		_tagPARSELIST *pNext;
		_tagPARSELIST *pPrev;
		_tagPARSELIST *pParent;
		_tagPARSELIST *pChild;
		_tagPARSELIST() : t(NULL),pNext(NULL), pPrev(NULL), pParent(NULL), pChild(NULL) { }
	} PARSELIST;
protected:
	PARSELIST* m_pRoot;
public:
	//-----------------------------------------------------------------------------//
	// Name: Script Load
	// Desc: 
	// Date: (이재정 2003-03-11 16:3)
	// Update : 
	//-----------------------------------------------------------------------------//
	PARSELIST* OpenScriptFile( char *szFileName )
	{
		FILE *fp;
		fopen_s( &fp, szFileName, "r" );
		if( !fp )
			return NULL;

		DeleteParseList( m_pRoot );
		if( fgetc(fp) == 'G' && fgetc(fp) == 'P' && fgetc(fp) == 'J' )
		{
			m_pRoot = new PARSELIST;
			m_pRoot->pChild = ParseRec( fp, NULL, NULL );
		}

		fclose( fp );
		return m_pRoot;
	}


	//-----------------------------------------------------------------------------//
	// Token 을찾아서 포인터를 리턴한다.
	//-----------------------------------------------------------------------------//
	static PARSELIST* FindToken( PARSELIST *pParseList, T t, BOOL bSearchSub=FALSE ) // bSearchSub = FALSE
	{
		PARSELIST *pNode = pParseList;

		while( pNode )
		{
			if( t == *pNode->t )
				break;

			// 하위트리 검색
			if( bSearchSub )
			{
				if( pNode->pChild )
				{
					CParser<T>::PARSELIST *p = FindToken( pNode->pChild, t, bSearchSub );
					if( p ) return p;
				}
			}
			pNode = pNode->pNext;
		}
		return pNode;
	}


	//-----------------------------------------------------------------------------//
	// 자식노드 갯수를 리턴한다.
	//-----------------------------------------------------------------------------//
	static int GetChildCount( PARSELIST *pList )
	{
		if( !pList ) return 0;
		if( !pList->pChild ) return 0;

		int count = 0;
		PARSELIST *pNode = pList->pChild;
		while( pNode )
		{
			++count;
			pNode = pNode->pNext;
		}
		return count;
	}


	//-----------------------------------------------------------------------------//
	// opt = 1 : 전체검사
	// opt = 2 : 연결된것만 검사
	// 
	//-----------------------------------------------------------------------------//
	static int GetNodeCount( PARSELIST *pList, T t, int opt=2 )
	{
		if( !pList ) return 0;

		int count = 0;
		PARSELIST *pNode = pList;
		while( pNode )
		{
			if( t == *pNode->t )
			{
				++count;
			}
			else
			{
				if( 1 == opt )
				{
					// 다음 Node 검사
				}				
				else if( 2 == opt )
					break;
			}
			pNode = pNode->pNext;
		}

		return count;
	}

	//-----------------------------------------------------------------------------//
	// Date: (이재정 2005-01-13 5:38)
	//-----------------------------------------------------------------------------//
	static void DeleteParseList( PARSELIST *pScrList )
	{
		if (!pScrList) return;
		if (pScrList->t) delete pScrList->t;
		DeleteParseList(pScrList->pChild);
		DeleteParseList(pScrList->pNext);
		delete pScrList;
	}


protected:
	//-----------------------------------------------------------------------------//
	// Name: Script Parsing
	// Desc: 
	// Date: (이재정 2003-03-11 16:4)
	// Update : 
	//-----------------------------------------------------------------------------//
	PARSELIST* ParseRec( FILE *fp, PARSELIST *pParent, PARSELIST *pPrev )
	{
		char c;
		BOOL bLoop=TRUE;
		PARSELIST *pList = NULL;

		do
		{
			c = fgetc(fp);
			switch( c )
			{
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				break;
			case '}':
				bLoop = FALSE;
				break;

			case '{':
				if( pList )
				{
					pList->pChild = ParseRec( fp, pList, NULL );
					pList->pNext = ParseRec( fp, pParent, pList );
				}
				bLoop = FALSE;
				break;
			default:
				UngetNextChar( fp );
				if( !pList )
				{
					T *t = ParseData( fp );
					if (t)
					{
						pList = new PARSELIST;
						pList->t = t;
						pList->pParent = pParent;
						pList->pPrev = pPrev;
					}
				}
				else
				{
					pList->pNext = ParseRec( fp, pParent, pList );
					UngetNextChar( fp );
				}
				break;
			}

		} while (bLoop && (EOF != c));

		return pList;
	}

	//-----------------------------------------------------------------------------//
	// Name: Script Parsing
	// Desc: 
	// Date: (이재정 2003-03-11 16:9)
	// Update : 
	//-----------------------------------------------------------------------------//
	T* ParseData( FILE *fp )
	{
		int state=0;
		char c;
		BOOL bLoop=TRUE;
		char str[ 128] = {0,};
		T* t = NULL;

		do
		{
			c = fgetc(fp);
			switch( c )
			{
			case ' ':
			case '\t':
			case '\r':
			case ',':
			case '(': // 3DMax Script 때문에 추가됨
			case ')': // 3DMax Script 때문에 추가됨
			case '[': // 3DMax Script 때문에 추가됨
			case ']': // 3DMax Script 때문에 추가됨
				break;

			// 주석
			case '$':
				{
					char tmp[ 128];
					fgets( tmp, 128, fp );
					bLoop = FALSE;
				}
				break;

			// the end
			case '{':
			case '}':
				UngetNextChar( fp );
				bLoop = FALSE;
				break;

			case '\n':
				bLoop = FALSE;
				break;

			case '"':
				{
					if( !t ) 
						t = new T;
					int cnt=0;
					while( EOF != (c = fgetc(fp)) && '"' != c && '\n' != c && '\r' != c )
						str[ cnt++] = c;
					str[ cnt] = NULL;
					*t << str;

					if( EOF == c )
					{
						bLoop = FALSE;
					}
				}
				break;

			default:
				{
					if( !t )
						t = new T;
					int cnt=1;
					str[ 0]=c;

					BOOL bFloat = FALSE; // float형일경우 '+-' 도 읽어드려야한다. ex "-1.62921e-007"
					while( EOF != (c = fgetc(fp)) && (is_alpha(c) || is_digit(c) || is_operator(c) || (bFloat && (c == '-')) || (bFloat && (c == '+')) || (bFloat && (c == 'e')) ) )
					{
						if( '.' == c ) bFloat = TRUE; // 소수점이 나오면 float형이 된다.
						str[ cnt++] = c;
					}
					str[ cnt] = NULL;

					*t << str;

					if (EOF == c) break;
					else UngetNextChar( fp );
				}
				break;
			}

		} while (bLoop && (EOF != c));

		return t;
	}

	//-----------------------------------------------------------------------------//
	// Name: File Pointer를 뒤로 한칸
	// Desc: 
	// Date: (이재정 2003-02-12 11:43)
	// Update : 
	//-----------------------------------------------------------------------------//
	void UngetNextChar( FILE *fp )
	{
		fseek( fp, ftell(fp)-1, SEEK_SET );
	}
};





///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
// Base Token Data
typedef struct _SBaseToken
{
	int nType;
	int nCount;
	_SBaseToken():nType(0),nCount(0) {}
	_SBaseToken& operator << ( char *str ) { }
	BOOL operator== ( const _SBaseToken& rhs ) { return FALSE; }

} SBaseToken;


///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
// Script Parsing Macro

#define PARSEBEGINE(type) \
	type& type::operator<<( char *c ) { \
		switch( nType ) {
#define PARSEEND() \
		case -1: break;\
		} return *this;\
	}
#define PARSETYPEBEGIN(type) \
		case type:{\
			switch( ++nCount ){
#define PARSETYPEEND() \
		case -1: break; }\
		} break;

#define PARSE_FLOAT_VALUE(nOrder,a) case nOrder: a=(float)atof(c); break;
#define PARSE_INT_VALUE(nOrder,a) case nOrder: a=atoi(c); break;
#define PARSE_STRING_VALUE(nOrder,a) case nOrder: strcpy( a, c ); break;



///////////////////////////////////////////////////////////////////////////////////
// Default DataList

typedef struct _SDftData : public _SBaseToken
{
	_SDftData() 
	{
		ZeroMemory( pStr, sizeof(pStr) );
	}
	_SDftData( char *str ) 
	{ 
		if( !str ) return;
		ZeroMemory( pStr, sizeof(pStr) );
		pStr[ 0] = new char[ strlen(str)+1];
		strcpy_s( pStr[ 0], strlen(str)+1, str );
		pStr[ strlen(str)] = NULL;
		nCount = 1;
	}

	char *pStr[ 64];

	virtual ~_SDftData() 
	{
		for( int i=0; i < nCount; ++i )
			delete[] pStr[ i];
	}

	_SDftData& operator << ( char *str ) 
	{
		if( !str )
		{
			pStr[ nCount] = new char[ 1];
			pStr[ nCount][ 0] = NULL;
			++nCount;
			return *this;
		}
		if( 0 >= strlen(str) )
		{
			pStr[ nCount] = new char[ 1];
			pStr[ nCount][ 0] = NULL;
			++nCount;
			return *this;
		}

		pStr[ nCount] = new char[ strlen(str)+1];
		strcpy_s( pStr[ nCount], strlen(str)+1, str );
		++nCount;

		return *this;
	}

	BOOL operator == ( const _SDftData& rhs )
	{
		if( !pStr[ 0] ) return FALSE;
		if( !rhs.pStr[ 0] ) return FALSE;
		if( !strcmp(pStr[ 0], rhs.pStr[ 0]) ) return TRUE;
		return FALSE;
	}

} SDftData;

typedef CParser<SDftData> SDftDataParser;
typedef CParser<SDftData>::PARSELIST SDftDataList;




/*
///////////////////////////////////////////////////////////////////////////////////
// sample code

PARSEBEGINE(_tagSPspData)

	PARSETYPEBEGIN( PSP_ZIPSWITCH )
		PARSE_INT_VALUE(1,U.SZip::nFlag)
	PARSETYPEEND()

	PARSETYPEBEGIN( PSP_FTPINFO )
		PARSE_STRING_VALUE(1,U.SFtpInfo::szFtpName)
		PARSE_STRING_VALUE(2,U.SFtpInfo::szFtpID)
		PARSE_STRING_VALUE(3,U.SFtpInfo::szFtpPass)
	PARSETYPEEND()

PARSEEND()
/**/

#endif // __PARSER_H__
