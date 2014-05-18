//-----------------------------------------------------------------------------//
// 2006-08-15 programer: jaejung ┏(⊙д⊙)┛
// 
// 2008-01-01 예전에 만든 컴파일러 코드를 수정해서 만들었다
//-----------------------------------------------------------------------------//

#if !defined(__SCRIPT_GLOBAL_H__)
#define __SCRIPT_GLOBAL_H__

#pragma warning (disable: 4786)
#include <list>
#include <map>
#include <deque>
#include <string>
#include <stack>

namespace ns_script
{
	enum { MAXCHILD=5, MAX_TABLE=16, MAX_MULTICOMPILE=64 };

	enum Tokentype
	{
		_ERROR, ENDFILE, ID, NUM, FNUM, STRING, ASSIGN, LPAREN, RPAREN, LBRACE, RBRACE, COMMA, COLON,
		PLUS, MINUS, TIMES, DIV, REMAINDER, 
		LT/* < */, RT/* > */, NEQ/* != */, EQ/* == */, OR/* || */, AND/* && */,
	
		INCLUDE, EVENT, IF, ELSE, WHILE, CLASS, FUNCTION, IMPORT, DEFINE, SCRWAIT, SCREND, SCREXIT
	};

	enum NodeKind { Stmt, Exp };
	enum Kind
	{
		ProgramK, ClassK, VarDecK, VarNameK, FuncDecK, AssignK, IfK, WhileK, EventK, ComposeK, IncludeK, 
		ScriptWaitK, ScriptEndK, ScriptExitK, // statement
		TypeK, ConditionOpK, OpK, IdK, FuncK, ParamK, ConstIntK, ConstFloatK, ConstStrK, CallK, // exp
	};


	class CScanner;

	// Type Table
	enum DataType
	{ DT_DATA, DT_CLASS, DT_FUNC };

	// TypeData
	typedef struct _tagSTypeData
	{
		char name[ 64];		// 변수이름
		char type[ 64];		// 타입이름	(NULL일때는 타입이 정해지지 않았을경우다)
		int id;			// 아이디 (게임내 데이타와 맵핑을 위해서 필요한값 unique)
		int offset;		// 메모리오프셋 (스택상에 저장될 위치)
		int size;		// 메모리 사이즈
		DataType state;	// 데이터(int,float,string)인지 혹은 클래스, 함수인지를 나타낸다.
		int argcnt;		// state=DT_FUNC 인경우 인자갯수를 나타낸다. (이 스크립터는 함수의 갯수만 검사한다.)

		_tagSTypeData() {}
		_tagSTypeData( char *n, char *t, int i, DataType d, int cnt=0 ):
			id(i), state(d), argcnt(cnt) 
			{  name[ 0] = NULL, type[ 0] = NULL;
				if( n ) strcpy_s( name, n );
				if( t ) strcpy_s(type,t); 
			}
		BOOL operator == ( const _tagSTypeData& rhs )
			{ if(!name[0] && !rhs.name[0] ) return TRUE; if(!name[0]||!rhs.name[0]) return FALSE; return !strcmp(name,rhs.name); }
	} STypeData;


	// TypeTable
	// 지역타입, 전역타입은 구현되지 않았다.
	// 현재 TypeTable은 하나만 존재하는걸로 가정한다.
	class CTypeTable
	{
	public:
		CTypeTable( CTypeTable *parent );
		virtual ~CTypeTable();
	protected:
		CTypeTable *m_pPt;	// parent type table
		std::map< std::string, std::list<STypeData>* > m_TypeTable;
	public:
		BOOL NewType( STypeData Type );
		BOOL AddType( char *pTypeName, STypeData Member );
		char* GetType( char *pTypeName, char *pMemberName=NULL );
		BOOL IsType( char *pTypeName, char *pMemberName=NULL );
		void CalcMemOffset();
		int GetTypeMemSize( char *pTypeName );
		int GetTypeMemOffset( char *pTypeName, char *pMemberName=NULL );
		int GetTypeId( char *pTypeName, char *pMemberName=NULL );
		std::list<STypeData>* GetMemberList( char *pTypeName );
	protected:
		BOOL _IsType( char *pTypeName, char *pMemberName ); // 현재 table에서만 검색
	};

	// Symbol Table
	// 변수이름, 데이타, 라인번호를 저장한다.
	// 전역변수,지역변수를 구분할수 있는 기능은 구현되어있다.
	class CSymTable
	{
	public:
		CSymTable( CTypeTable *typetab, CSymTable *parent ) { m_nTotalMemSize=0; m_pTypeTab=typetab; m_pPt = parent; }
		virtual ~CSymTable() {}
	protected:
		typedef struct _tagSymData
		{
			BOOL import;		// 게임내 데이타에서 import된 변수이면 true
			BOOL load;			// 어플리케이션으로 부터 포인터를 로드했다면 TRUE
			char type[ 64];		// 변수타입
			char name[ 64];		// 변수이름
			int offset;		// 메모리오프셋 (스택상에 저장될 위치)
			int id;			// 아이디 (게임내 데이타와 맵핑을 위해서 필요한값 unique)
			union
			{
				int num;
				float fnum;
				char *name;
				void *_class;
			} data;
		} SSymData;

		CTypeTable *m_pTypeTab;	// current type table
		CSymTable *m_pPt;		// parent symbol table
		std::map<std::string,SSymData> m_SymTab;
		int m_nTotalMemSize;

	public:
		BOOL AddSymbol( char *pSymbolName, BOOL import=FALSE, int id=0 );
		BOOL SetType( char *pSymbolName, char *pTypeName );
		char* GetType( char *pSymbolName );
		BOOL IsSymbol( char *pSymbolName );
		BOOL IsImportClass( char *pSymbolName );
		void Loaded( char *pSymbolName );

		void CalcMemOffset();
		int GetMemOffset( char *pSymbolName );
		int GetClassMemOffset( char *pSymbolName );
		int GetSymbolId( char *pSymbolName );
		BOOL IsLoaded( char *pSymbolName );
		int GetTotalMemSize() { return m_nTotalMemSize; }

		// data 관련
		BOOL SetData( char *pSymbolName, void *data );
		int GetIntData( char *pSymbolName );
		float GetFloatData( char *pSymbolName );
		char* GetStrData( char *pSymbolName );
		void* GetClassData( char *pSymbolName );
		void GetGlobalImportData( std::list< std::string > *plst );

	protected:
		BOOL _IsSymbolRec( char *szSymbolName );
		BOOL _IsSymbol( char *szSymbolName ); // 현재 table에서만 검색
		SSymData* _FindData( char *pSymbolName );
		char* _GetType( char *pTypeName, char *pMemberName );

	};



	// String Table
	// 스크립트내에 사용되는 ConstString을 모두저장한다.
	class CStringTable
	{
	public:
		CStringTable() : m_nMaxStrSize(0) {}
		virtual ~CStringTable() {}

	protected:
		typedef struct _tagStrInfo
		{
			char str[ 128];
			int offset;	// StringTable에서 string의 위치 (4byte단위)

			_tagStrInfo() {}
			_tagStrInfo( char *c, int o ):offset(o) { strcpy_s(str,c); }
			BOOL operator== ( const _tagStrInfo& rhs )
				{ if( !str ) return FALSE; return !strcmp(str,rhs.str); }
		} SStrInfo;

		int m_nMaxStrSize;	// 4바이트단위
		std::list< SStrInfo > m_StrList;

	public:
		void AddString( char *str );
		int GetOffset( char *str );
		void WriteStringTable( FILE *fp );

	protected:

	};


	// import변수 관련 함수
	enum SystemFunc
	{
		// system 함수
		LoadVariable = 1,
		StoreVariable,
		LoadPointer,
		// global 함수
		DbgPrint = 1000,
		DbgPrintStr,		
		ExeScript,
		_GetArgument,
	};

	// ParseTree 형태
	typedef struct _tagParseTree
	{
		NodeKind nodekind;
		Kind kind;
		union
		{
			int num;
			float fnum;
			char *name;
			void *_class;
			Tokentype op;
		} attr;

		int lineno;
		char *type;
		CSymTable *pSym;
		_tagParseTree *child[ MAXCHILD];
		_tagParseTree *sibling;

	} SParseTree;


	// PreCompiler
	// 전처리 작업기 이며, define으로 정의된 스트링을 원래값으로 복원한다.
	// CParser에 의해 토큰이 파징된후 이 클래스가 실행된다.
	class CPreCompiler
	{
	public:
		CPreCompiler() {}
		virtual ~CPreCompiler() {}

	protected:
		typedef struct _tagSTokDat
		{
			std::string str;
			Tokentype tok;
		} STokDat;
		std::map< std::string, STokDat > m_DefMap;
	public:
		void PreCompile( SParseTree *pTree );
		BOOL InsertDefine( char *pID, Tokentype type, char *pStr );
		BOOL FindDefine( char *pID, Tokentype *pType, std::string *pStr );
	};


	extern std::stack<std::string> g_Include;

	// Parse Node 생성함수
	SParseTree* NewStmt( Kind k, CScanner *pS );
	SParseTree* NewExp( Kind k, CScanner *pS );
	void PrintToken( Tokentype token, char *szTokenString );
	void DeleteParseTree( SParseTree *pTree );

}

#endif // __SCRIPT_GLOBAL_H__
