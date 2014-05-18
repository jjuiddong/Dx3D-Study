
#include "global.h"
#include "script_global.h"
#include "script_parser.h"
#include "script_analyze.h"
#include "script_codegen.h"
#include "script_machine.h"
#include "script.h"

using namespace std;
using namespace ns_script;

CScript::CScript()
{
	m_pMachine = new CMachine;
	m_IncTime = 0;

}
CScript::~CScript() 
{
	if( m_pMachine ) delete m_pMachine;
}


//-----------------------------------------------------------------------------//
// 스크립트를 컴파일하고 실행한다.
// pFileName : 실행 될 스크립트 파일명
// callback : 콜백함수
// pArgument : 실행인자값 (스트링)
//-----------------------------------------------------------------------------//
BOOL CScript::Execute( char *pFileName, void callback (int,ns_script::CProcessor*), char *pArgument ) // pArgument=NULL
{
	// TypeTable 생성
	// TypeTable은 오직 하나만 존재한다.
	CTypeTable *ptype = new CTypeTable( NULL );
	CPreCompiler *ppre = new CPreCompiler();

	BOOL result = Compile( pFileName, callback, ptype, ppre );
	delete ptype;
	delete ppre;
	if( !result ) return FALSE;

	// Simulation
	g_Dbg.Console( "Execute...\n" );
	g_Dbg.Console( "%s\n", pFileName );
	m_pMachine->ExecuteScript( pFileName, callback, pArgument );

	return TRUE;
}


//-----------------------------------------------------------------------------//
// 소스파일을 컴파일 하고 게임에서 사용될 원시코드를 생성한다.
// 생성된 파일이름은 소스파일과 같고 확장자는 .gm 이다. (GameMachine 약자)
// szFileName : 컴파일될 소스파일 이름
//-----------------------------------------------------------------------------//
BOOL CScript::Compile( char *pFileName, void callback (int,ns_script::CProcessor*), 
					  ns_script::CTypeTable *pType, ns_script::CPreCompiler *pPreC ) // pType=NULL, pPreC=NULL
{
	g_Dbg.Console( "Compiling...\n" );
	g_Dbg.Console( "%s\n", pFileName );

	// Parse
	CParser parser;
	ns_script::SParseTree *pt = parser.Parse( pFileName, pPreC );
	if( parser.IsError() ) return FALSE;
	if( !pt ) return FALSE;

	// include된 파일을 compile한다.
	while( !g_Include.empty() )
	{
		string filename = g_Include.top();
						  g_Include.pop();
		Compile( (char*)filename.c_str(), callback, pType, pPreC );
	}

	// PreCompiler
	pPreC->PreCompile( pt );

	CSymTable *psym = new CSymTable( pType, NULL );

	// Analyze
	CAnalyze analyze;
	CStringTable strtable;
	pt->pSym = psym;
	analyze.Build( pFileName, pt, psym, pType, &strtable );
	if( analyze.IsError() ) return FALSE;

	pType->CalcMemOffset();
	psym->CalcMemOffset();

	// Code Generate
	CCodeGen cgen;
	cgen.CodeGenerate( pt, pType, &strtable, pFileName );

	DeleteParseTree( pt );
	delete psym;

	return TRUE;
}


//-----------------------------------------------------------------------------//
// Instruction 실행
// nDelta : 프레임간 경과시간 (millisecond 단위)
//-----------------------------------------------------------------------------//
int CScript::Run( int nDelta )
{
	m_IncTime += nDelta;
	if( m_IncTime < 33 ) return 0;	// 30 frame

	m_IncTime = 0;
	return m_pMachine->Run();
}
