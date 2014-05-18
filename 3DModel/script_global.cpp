
#include "global.h"
#include "script_global.h"
#include "script_parser.h"
#include <algorithm>

using namespace std;
using namespace ns_script;

stack<string> ns_script::g_Include;



///////////////////////////////////////////////////////////////////////////////////
// CPreCompiler


//-----------------------------------------------------------------------------//
// define으로 설정된 값을 저장한다. precompiler와 비슷하다.
// 저장 실패하면 FALSE를 리턴한다.
// pID : 맵의 키값이 된다.
// type: 토큰타입
// pStr: 저장될 토큰데이타
//-----------------------------------------------------------------------------//
BOOL CPreCompiler::InsertDefine( char *pID, Tokentype type, char *pStr )
{
	map<string,STokDat>::iterator it = m_DefMap.find( pID );
	if( m_DefMap.end() != it ) return FALSE; // 같은 키값의 데이타가 이미 존재함 에러!!

	STokDat t;
	t.str = pStr;
	t.tok = type;
	m_DefMap.insert( map<string,STokDat>::value_type(pID, t) );
	return TRUE;
}


//-----------------------------------------------------------------------------//
// 저장된 define정보를 찾는다.
// 데이타가 없다면 FALSE를 리턴한다.
//-----------------------------------------------------------------------------//
BOOL CPreCompiler::FindDefine( char *pID, Tokentype *pType, string *pStr )
{
	map<string,STokDat>::iterator it = m_DefMap.find( pID );
	if( m_DefMap.end() == it ) return FALSE;

	*pStr = it->second.str;
	*pType = it->second.tok;
	return TRUE;
}


//-----------------------------------------------------------------------------//
// 전처리 작업
//-----------------------------------------------------------------------------//
void CPreCompiler::PreCompile( SParseTree *pTree )
{
	if( !pTree ) return;
	if( Exp == pTree->nodekind )
	{
		if( IdK == pTree->kind )
		{
			Tokentype t;
			string str;
			if( FindDefine( pTree->attr.name, &t, &str) )
			{
				if( NUM == t )
				{
					pTree->kind = ConstIntK;
					pTree->attr.num = atoi( str.c_str() );
				}
				else if( STRING == t )
				{
					pTree->kind = ConstStrK;
					strcpy( pTree->attr.name, str.c_str() );
				}
			}
		}
	}
	for( int i=0; i < MAXCHILD; ++i )
	{
		if( pTree->child[ i] )
			PreCompile( pTree->child[ i] );
	}
	PreCompile( pTree->sibling);
}


 
///////////////////////////////////////////////////////////////////////////////////
// CTypeTable

char *g_DefautType[] = { "void", "bool", "int", "float", "string" };
const int g_DefautTypeSize = sizeof(g_DefautType) / sizeof(char*);

CTypeTable::CTypeTable( CTypeTable *parent ) 
{
	m_pPt = parent;

	// 기본적인건 미리 생성한다.
	for( int i=0; i < g_DefautTypeSize; ++i )
		NewType( STypeData(g_DefautType[i], g_DefautType[i],0,DT_DATA) );

}

CTypeTable::~CTypeTable() 
{
	map< string, list<STypeData>* >::iterator itor = m_TypeTable.begin();
	while( m_TypeTable.end() != itor )
	{
		if( itor->second )
			delete itor->second;
		++itor;
	}
	m_TypeTable.clear();
}


//-----------------------------------------------------------------------------//
// NewType()함수는 타입을 생성한다.
// map에 저장되는 정보는 type의 멤버변수 혹은 멤버함수의 이름과 타입이다.
// data,class,function 모두 map에 저장되며 적어도 하나의 STypeData구조체가 
// map의 list에 저장된다. 
// list에 가장 첫번째 정보는 그 데이타타입의 type,state 등의 정보를 가지며 
// name 값은 NULL로 저장된다. 왜냐하면 name은 이미 map의 key로 사용되기 때문이며
// 멤버변수,함수가 를 제외한 데이타를 검색하기위해 name값은 NULL로 저장된다.
//-----------------------------------------------------------------------------//
BOOL CTypeTable::NewType( STypeData Type )
{
	if( IsType(Type.name) ) return FALSE;
	list<STypeData> *plist = new list<STypeData>;

	char key[ 64];
	strcpy( key, Type.name );

	// name값은 NULL로 설정되고, 후에 type을 검색할때 멤버변수,함수가 아닌 data
	// 를 검색하기 위해 설정된다.
//	Type.name = NULL;
	Type.name[ 0] = NULL;
	plist->push_back( Type );
	m_TypeTable.insert( map< string, list<STypeData>* >::value_type(key,plist) );

	return TRUE;
}

// szTypeName 타입에 멤버변수나 함수를 추가한다.
// szTypeName은 키가된다.
BOOL CTypeTable::AddType( char *pTypeName, STypeData Member )
{
	// pTypeName이 NULL일경우 전역변수, 함수가 된다.
	if( !pTypeName )
	{
		NewType( Member );
	}
	else
	{
		if( !IsType(pTypeName) ) return FALSE;
		m_TypeTable[ pTypeName]->push_back( Member );
	}
	return TRUE;
}


// szTypeName의 타입의 멤버변수 혹은 함수의 타입을 얻는다.
char* CTypeTable::GetType( char *pTypeName, char *pMemberName ) // pMemberName=NULL
{
	if( !pTypeName ) return NULL;
	if( !IsType(pTypeName) ) return NULL;
	list<STypeData> *plist = m_TypeTable[ pTypeName];
	list<STypeData>::iterator itor = find( plist->begin(), plist->end(), STypeData(pMemberName,NULL,0,DT_DATA) );
	if( plist->end() == itor ) return NULL;
	if( NULL == itor->type[ 0] ) return NULL;
	return itor->type;
}

BOOL CTypeTable::IsType( char *pTypeName, char *pMemberName ) // pMemberName=NULL
{
	if( _IsType(pTypeName, pMemberName) )
		return TRUE;

	if( !m_pPt ) return FALSE;
	return m_pPt->IsType( pTypeName, pMemberName );
}


BOOL CTypeTable::_IsType( char *szTypeName, char *pMemberName )
{
	if( !szTypeName ) return FALSE;
	map< string, list<STypeData>* >::iterator itor = m_TypeTable.find( szTypeName );
	if( m_TypeTable.end() == itor ) return FALSE;
	list<STypeData> *plist = itor->second;
	if( !pMemberName ) return TRUE;
	list<STypeData>::iterator itor2 = find( plist->begin(), plist->end(), STypeData(pMemberName,NULL,0,DT_DATA) );
	if( plist->end() == itor2 ) return FALSE;
	return TRUE;
}


//-----------------------------------------------------------------------------//
// 각타입내의 멤버변수메모리 오프셋값을 계산한다.
//-----------------------------------------------------------------------------//
void CTypeTable::CalcMemOffset()
{
	map< string, list<STypeData>* >::iterator i = m_TypeTable.begin();
	while( m_TypeTable.end() != i )
	{
		list<STypeData> *plist = i->second;
		list<STypeData>::iterator k = plist->begin();

		int memoffset = 0;
		while( plist->end() != k )
		{
			k->offset = memoffset;
			k->size = 1;			// 메모리 크기는 4byte단위며 일반적으로 1을 가진다.
			// 함수는 메모리크기에 영향을 미치지 않으며, 
			// 클래스내 클래스는 아직 지원하지 않는다.
//			if( DT_FUNC != k->state && DT_CLASS != k->state )
			if( DT_FUNC != k->state )
				++memoffset;
			++k;
		}
		// list의 첫번째값에 메모리크기를 저장한다.(list의 첫번째값이 전체를 뜻한다.)
//		plist->front().offset = memoffset;
		plist->front().size = memoffset;

		++i;
	}
}


//-----------------------------------------------------------------------------//
// CalcMemOffset() 함수 호출후에야 이 함수가 동작할수 있다.
// 해당 타입의 메모리크기를 리턴한다. (DWORD단위)
//-----------------------------------------------------------------------------//
int CTypeTable::GetTypeMemSize( char *pTypeName )
{
	if( !pTypeName ) return 0;
	if( !IsType(pTypeName) ) return 0;

	// 함수를 제외한 크기를 구한다.
	list<STypeData> *plist = m_TypeTable[ pTypeName];
//	return plist->front().offset;
	return plist->front().size;
}


//-----------------------------------------------------------------------------//
// CalcMemOffset() 함수 호출후에야 이 함수가 동작할수 있다.
// 해당 타입의 메모리오프셋을 리턴한다.
//-----------------------------------------------------------------------------//
int CTypeTable::GetTypeMemOffset( char *pTypeName, char *pMemberName ) // pMemberName=NULL
{
	if( !pTypeName ) return 0;
	if( !IsType(pTypeName) ) return 0;
	list<STypeData> *plist = m_TypeTable[ pTypeName];
	list<STypeData>::iterator itor = find( plist->begin(), plist->end(), STypeData(pMemberName,NULL,0,DT_DATA) );
	if( plist->end() == itor ) return 0;
	return itor->offset;
}


int CTypeTable::GetTypeId( char *pTypeName, char *pMemberName )
{
	if( !pTypeName ) return 0;
	if( !IsType(pTypeName) ) return 0;
	list<STypeData> *plist = m_TypeTable[ pTypeName];
	list<STypeData>::iterator itor = find( plist->begin(), plist->end(), STypeData(pMemberName,NULL,0,DT_DATA) );
	if( plist->end() == itor ) return 0;
	return itor->id;
}


list<STypeData>* CTypeTable::GetMemberList( char *pTypeName )
{
	if( !pTypeName ) return 0;
	if( !IsType(pTypeName) ) return 0;
	return m_TypeTable[ pTypeName];
}


///////////////////////////////////////////////////////////////////////////////////
// CSymTable

//-----------------------------------------------------------------------------//
// 심볼 생성
// 심볼 생성시 타입과 데이타는 설정되어있지 않은 상태이다.
// import : 게임내의 데이타와 상호작용하는 변수이면 true
// id : import=true일때 활성화 되며 게임내데이타와 매핑을위해 필요함
//-----------------------------------------------------------------------------//
BOOL CSymTable::AddSymbol( char *pSymbolName, BOOL import, int id ) // import=FALSE, id=0
{
	if( IsSymbol(pSymbolName) ) return FALSE;
	if( strchr(pSymbolName,'.') ) return FALSE;	// 클래스 멤버변수는 심볼에 추가되지않는다.

	SSymData sd;
	strcpy( sd.name, pSymbolName );
	sd.type[ 0] = NULL;
//	sd.type = NULL;
	sd.offset = 0;
	sd.import = import;
	sd.id = id;
	sd.load = FALSE;

	m_SymTab.insert( map<string,SSymData>::value_type(pSymbolName,sd) );
	return TRUE;
}


// 심볼에 해당하는 타입 설정
BOOL CSymTable::SetType( char *pSymbolName, char *pTypeName )
{
	SSymData *p = _FindData( pSymbolName );
	if( !p ) return FALSE;
	if( NULL != p->type[ 0] ) return FALSE; // 타입이 이미 설정되었다면 다시 설정할수 없다.
//	p->type = pTypeName;
	if( pTypeName )	strcpy( p->type, pTypeName );
	return TRUE;
}


//-----------------------------------------------------------------------------//
// 변수명으로 심볼테이블 정보를 얻는다.
// 없다면 부모 심볼테이블을 검색하고 이를 반복한다.
// pSymbolName : 변수명
// return value: 해당 변수의 심볼정보를 리턴한다.
//-----------------------------------------------------------------------------//
CSymTable::SSymData* CSymTable::_FindData( char *pSymbolName )
{
	map<string,SSymData>::iterator itor = m_SymTab.find( pSymbolName );
	if( m_SymTab.end() == itor )
	{
		// 없다면 부모테이블을 검색한다.
		if( m_pPt )
			return m_pPt->_FindData( pSymbolName );	
	}
	else
	{
		return &itor->second;
	}	
	return NULL;
}


//-----------------------------------------------------------------------------//
// pSymbolName 의 타입을 얻는 함수다.
// pSymbolName은 클래스 변수 혹은 함수, 혹은 전역 변수, 전역 함수 일수 있다.
// 클래스 멤버변수,함수를 구분짓기 위해서는 pSymbolName내에 '.'이 있는지 확인해야하고
// 없다면 전역변수 혹은 함수가 된다.
// 만약 '.'이 있다면 처음 token에서의 변수이름을 얻어내서 SymbolTable을 통해서
// 타입을 얻는다. 이후에는 재귀적으로 타입을 추출해서 최종 타입을 얻어낸다.
// 아직까지 클래스내 클래스는 지원하지 않으므로 재귀함수로는 구현되지 않았다.
//-----------------------------------------------------------------------------//
char* CSymTable::GetType( char *pSymbolName )
{
	char *reval = NULL;
	char *pdot = strchr( pSymbolName, '.' );
	if( pdot )
	{
		// 클래스 멤버변수 or 멤버함수
		char var[ 64];
		strncpy( var, pSymbolName, pdot-pSymbolName );
		var[ pdot-pSymbolName] = NULL;
		SSymData *p = _FindData( var );
		if( !p ) return NULL;
		reval = m_pTypeTab->GetType( p->type, pdot+1 );
	}
	else
	{
		// 전역변수
		SSymData *p = _FindData( pSymbolName );
		if( p )
		{
			if( NULL != p->type[ 0] )
				reval = p->type;
		}
	}
	return reval;
}


// SetData() 함수는 심볼의 Type이 설정되어 있을경우만 제대로 작동한다.
BOOL CSymTable::SetData( char *pSymbolName, void *data )
{
	SSymData *p = _FindData( pSymbolName );
	if( !p ) return FALSE;
	if( !p->type ) return FALSE;

	if( !strcmp(p->type,"int") )
	{
		p->data.num = *(int*)data;
	}
	else if( !strcmp(p->type,"float") )
	{
		p->data.fnum = *(float*)data;
	}
	else if( !strcmp(p->type,"string") )
	{
		p->data.name = (char*)data;
	}
	else // class
	{
		p->data._class = data;
	}	
	return TRUE;
}

int CSymTable::GetIntData( char *pSymbolName )
{
	SSymData *p = _FindData( pSymbolName );
	if( !p ) return 0;
	if( !p->type ) return 0;
	return p->data.num;
}

float CSymTable::GetFloatData( char *pSymbolName )
{
	SSymData *p = _FindData( pSymbolName );
	if( !p ) return 0.f;
	if( !p->type ) return 0.f;
	return p->data.fnum;
}

char* CSymTable::GetStrData( char *pSymbolName )
{
	SSymData *p = _FindData( pSymbolName );
	if( !p ) return NULL;
	if( !p->type ) return NULL;
	return p->data.name;
}

void* CSymTable::GetClassData( char *pSymbolName )
{
	SSymData *p = _FindData( pSymbolName );
	if( !p ) return NULL;
	if( !p->type ) return NULL;
	return p->data._class;
}


//-----------------------------------------------------------------------------//
// GetType() 함수와 비슷한 구조를 가진다.
//-----------------------------------------------------------------------------//
BOOL CSymTable::IsSymbol( char *pSymbolName )
{
	BOOL reval = FALSE;

	char *pdot = strchr( pSymbolName, '.' );
	if( pdot )
	{
		// 클래스 멤버변수 or 멤버함수
		char var[ 64];
		strncpy( var, pSymbolName, pdot-pSymbolName );
		var[ pdot-pSymbolName] = NULL;
		SSymData *p = _FindData( var );
		if( !p ) return FALSE;
		reval = m_pTypeTab->IsType( p->type, pdot+1 );
	}
	else
	{
		// 전역변수
		reval = _IsSymbolRec( pSymbolName );
	}

	return reval;
}


//-----------------------------------------------------------------------------//
// 임포트된 클래스라면 TRUE를 리턴한다.
// 현재 모든 클래스는 임포트 클래스다.
//-----------------------------------------------------------------------------//
BOOL CSymTable::IsImportClass( char *pSymbolName )
{
	SSymData *p = NULL;

	char *pdot = strchr( pSymbolName, '.' );
	if( pdot )
	{
		// 클래스 변수
		char var[ 64];
		strncpy( var, pSymbolName, pdot-pSymbolName );
		var[ pdot-pSymbolName] = NULL;
		p = _FindData( var );
	}
	else
	{
		p = _FindData( pSymbolName );
	}
	if( !p ) return FALSE;

	// 디폴트타입이 아니라면 클래스 타입이다.
	for( int i=0; i < g_DefautTypeSize; ++i )
	{
		if( !strcmp(g_DefautType[ i], p->type) )
			break;
	}

	return (i >= g_DefautTypeSize);
}


//-----------------------------------------------------------------------------//
// import된 변수가 어플리케이션으로 부터 포인터를 할당 받았다며 load변수를 
// TRUE로 설정한다.
//-----------------------------------------------------------------------------//
void CSymTable::Loaded( char *pSymbolName )
{
	// 클래스 멤버변수인지 전역변수인지 구분한다.
	char *pdot = strchr( pSymbolName, '.' );
	if( pdot )
	{
		// 클래스 멤버변수
		char var[ 64];
		strncpy( var, pSymbolName, pdot-pSymbolName );
		var[ pdot-pSymbolName] = NULL;
		SSymData *p = _FindData( var );
		if( p ) 
			p->load = TRUE;
	}
	else
	{
		SSymData *p = _FindData( pSymbolName );
		if( p )
		{
			if( p->import ) 
				p->load = TRUE;
		}
	}
}


//-----------------------------------------------------------------------------//
// 현재 map에 심볼이 있는지 검색한다. 현재 클래스에서 없다면
// 부모클래스까지 검사한다 
//-----------------------------------------------------------------------------//
BOOL CSymTable::_IsSymbolRec( char *szSymbolName )
{
	if( _IsSymbol(szSymbolName) )
		return TRUE;
	if( !m_pPt ) return FALSE;
	return m_pPt->IsSymbol( szSymbolName );
}

BOOL CSymTable::_IsSymbol( char *szSymbolName )
{
	map<string,SSymData>::iterator itor = m_SymTab.find( szSymbolName );
	if( m_SymTab.end() == itor )
		return FALSE;
	return TRUE;
}


//-----------------------------------------------------------------------------//
// mempos 값을 계산한다.
//-----------------------------------------------------------------------------//
void CSymTable::CalcMemOffset()
{
	if( !m_pTypeTab ) return;
	int mem = 0;
	map<string,SSymData>::iterator itor = m_SymTab.begin();
	while( m_SymTab.end() != itor )
	{
		SSymData dat = itor->second;
		if( !dat.type )
		{
			++itor;
			continue;
		}
		itor->second.offset = mem;
		mem += m_pTypeTab->GetTypeMemSize( dat.type );

		++itor;
	}
	m_nTotalMemSize = mem;
}


//-----------------------------------------------------------------------------//
// CalcMemOffset() 함수가 호출된후에야 이 함수가 동작한다.
// mempos값은 변수가 저장되는 메모리 offset값이다. (DWORD단위)
// mempos값은 스택상에 변수가 저장될때 저장될 위치를 가르키며 DWORD단위이다. 
// 스택이 아래로 자라날지, 위로 자라날지에 따라 관계된다. 현재 스택이 아래로 
// 자라나는것으로 한다. (stack의 높은주소에서 낮은주소로 push됨을 뜻한다.) 
//-----------------------------------------------------------------------------//
int CSymTable::GetMemOffset( char *pSymbolName ) 
{
	int reval = 0;

	char *pdot = strchr( pSymbolName, '.' );
	if( pdot )
	{
		// 클래스 멤버변수
		char var[ 64];
		strncpy( var, pSymbolName, pdot-pSymbolName );
		var[ pdot-pSymbolName] = NULL;
		SSymData *p = _FindData( var );
		if( !p ) return 0;

		int offset = m_pTypeTab->GetTypeMemOffset( p->type, pdot+1 );
		reval = p->offset + offset;
	}
	else
	{
		// 전역변수
		SSymData *p = _FindData( pSymbolName );
		reval = p->offset;
	}

	return reval;
}


//-----------------------------------------------------------------------------//
// pSymbolName 변수명이 클래스명을 포함할때 (ex "enemy.state")
// 클래스의 오프셋을 리턴한다.
//-----------------------------------------------------------------------------//
int CSymTable::GetClassMemOffset( char *pSymbolName )
{
	int reval = 0;
	char *pdot = strchr( pSymbolName, '.' );
	if( pdot )
	{
		// 클래스 멤버변수
		char var[ 64];
		strncpy( var, pSymbolName, pdot-pSymbolName );
		var[ pdot-pSymbolName] = NULL;
		SSymData *p = _FindData( var );
		if( !p ) return 0;
		reval = p->offset;
	}
	else
	{
		// 전역변수
		SSymData *p = _FindData( pSymbolName );
		reval = p->offset;
	}
	return reval;
}


//-----------------------------------------------------------------------------//
// 심볼이 import된 변수이거나, 클래스일경우 타입의 ID를 리턴한다. 
//-----------------------------------------------------------------------------//
int CSymTable::GetSymbolId( char *pSymbolName )
{
	int reval = 0;

	// 클래스 멤버변수인지 전역변수인지 구분한다.
	char *pdot = strchr( pSymbolName, '.' );
	if( pdot )
	{
		// 클래스 멤버변수
		char var[ 64];
		strncpy( var, pSymbolName, pdot-pSymbolName );
		var[ pdot-pSymbolName] = NULL;
		SSymData *p = _FindData( var );
		if( !p ) return 0;
		reval = m_pTypeTab->GetTypeId( p->type, pdot+1 );
	}
	else
	{
		SSymData *p = _FindData( pSymbolName );
		if( !p )
		{
			// 전역 함수일 경우
			reval = m_pTypeTab->GetTypeId( pSymbolName );
		}
		else
		{
			if( p->import ) 
				reval = p->id;
		}
	}
	return reval;
}


//-----------------------------------------------------------------------------//
// 심볼이 import된 변수이거나, 클래스일경우 어플리케이션으로 부터 포인터를 로드
// 되었는지 여부를 리턴한다.
//-----------------------------------------------------------------------------//
BOOL CSymTable::IsLoaded( char *pSymbolName )
{
	BOOL reval = FALSE;

	// 클래스 멤버변수인지 전역변수인지 구분한다.
	char *pdot = strchr( pSymbolName, '.' );
	if( pdot )
	{
		// 클래스 멤버변수
		char var[ 64];
		strncpy( var, pSymbolName, pdot-pSymbolName );
		var[ pdot-pSymbolName] = NULL;
		SSymData *p = _FindData( var );
		if( !p ) return 0;
		reval = p->load;
	}
	else
	{
		SSymData *p = _FindData( pSymbolName );
		if( p )
		{
			if( p->import ) 
				reval = p->load;
		}
	}
	return reval;
}


//-----------------------------------------------------------------------------//
// 임포트된 전역변수 (클래스가 아닌) 를 리스트에 저장해서 리턴한다.
// plst : 정보가 저장되어서 리턴된다.
//-----------------------------------------------------------------------------//
void CSymTable::GetGlobalImportData( list< string > *plst )
{
	map<string,SSymData>::iterator itor = m_SymTab.begin();
	while( m_SymTab.end() != itor )
	{
		SSymData dat = itor->second;
		if( dat.import && dat.id )
			plst->push_back( dat.name );
		++itor;
	}
}


///////////////////////////////////////////////////////////////////////////////////
// CStringTable


void CStringTable::AddString( char *str )
{
	if( !str ) return;

	m_StrList.push_back( SStrInfo(str,m_nMaxStrSize) );
	int size = strlen( str ) + 1; // NULL문자까지 포함
	// 4byte 단위로 바꾼다.
	size = (size + 3) & ~3;
	m_nMaxStrSize += size / 4;
}


int CStringTable::GetOffset( char *str )
{
	list<SStrInfo>::iterator itor = find( m_StrList.begin(), m_StrList.end(), SStrInfo(str,0) );
	if( m_StrList.end() == itor ) return 0;
	return itor->offset;
}


//-----------------------------------------------------------------------------//
// StringTable을 저장한다.
//-----------------------------------------------------------------------------//
void CStringTable::WriteStringTable( FILE *fp )
{
	// 총 스트링테이블 크기를 저장한후 테이블을 저장한다.
	fwrite( &m_nMaxStrSize, 1, sizeof(int), fp );

	int c = NULL;
	list<SStrInfo>::iterator i = m_StrList.begin();
	while( m_StrList.end() != i )
	{
		SStrInfo info = *i++;
		fwrite( info.str, sizeof(char), strlen(info.str)+1, fp );
//		fputc( '\0', fp );
		// string을 저장후 4byte단위로 설정된 길이의 나머지를 저장한다.
		int len = (strlen( info.str ) + 1);
		int dword = (len + 3) & ~3;
		for( int k=0; k < dword-len; ++k )
			fwrite( &c, sizeof(char), 1, fp );
//			fputc( '\0', fp );
	}
}



///////////////////////////////////////////////////////////////////////////////////
// Global Function

ns_script::SParseTree* ns_script::NewStmt( Kind k, ns_script::CScanner *pS )
{
	SParseTree *t = new SParseTree;
	t->nodekind = Stmt;
	t->kind = k;
	t->lineno = pS->GetLineNo();
	t->attr.name = 0;
	t->type = NULL;
	ZeroMemory( t->child, sizeof(t->child) );
	t->sibling = NULL;

	return t;
}


ns_script::SParseTree* ns_script::NewExp( Kind k, ns_script::CScanner *pS )
{
	SParseTree *t = new SParseTree;
	t->nodekind = Exp;
	t->kind = k;
	t->lineno = pS->GetLineNo();
	t->attr.name = 0;
	t->type = NULL;
	ZeroMemory( t->child, sizeof(t->child) );
	t->sibling = NULL;

	return t;
}


void ns_script::PrintToken( ns_script::Tokentype token, char *szTokenString )
{
	switch( token )
	{
	case ID:
	case STRING:	g_Dbg.Console( "string = %s\n", szTokenString ); break;

	case ASSIGN:	g_Dbg.Console( "=" ); break;
	case TIMES:		g_Dbg.Console( "*" ); break;
	case LPAREN:	g_Dbg.Console( "(" ); break;
	case RPAREN:	g_Dbg.Console( ")" ); break;
	case LBRACE:	g_Dbg.Console( "{" ); break;
	case RBRACE:	g_Dbg.Console( "}" ); break;
	case COMMA:		g_Dbg.Console( "," ); break;
	case NUM:		g_Dbg.Console( "number" ); break;

	default: 		g_Dbg.Console( "UnKnown token %d, %s\n", token, szTokenString ); break;
	}
}


//-----------------------------------------------------------------------------//
// ParseTree 제거
//-----------------------------------------------------------------------------//
void ns_script::DeleteParseTree( SParseTree *pTree )
{
	if( !pTree ) return;
	for( int i=0; i < MAXCHILD; ++i )
		DeleteParseTree( pTree->child[ i] );
	DeleteParseTree( pTree->sibling );
	
	switch( pTree->kind )
	{
	case ConstStrK:
	case IdK:
	case VarNameK:
	case FuncK:
	case ParamK:
	case TypeK:
		delete[] pTree->attr.name;
		break;
	}
	delete pTree;
}

