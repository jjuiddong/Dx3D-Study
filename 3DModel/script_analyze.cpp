
#include "global.h"
#include "script_global.h"
#include "script_analyze.h"

using namespace std;
using namespace ns_script;


//-----------------------------------------------------------------------------//
// ParseTree 분석
// pFileName : 소스 파일명 (에러출력 때문에 필요함)
//-----------------------------------------------------------------------------//
BOOL CAnalyze::Build( char *pFileName, SParseTree *pParseTree, CSymTable *pSymTab, 
					  CTypeTable *pTypeTab, CStringTable *pStrTab )
{
	m_pStrTab = pStrTab;
	strcpy( m_FileName, pFileName );

	BuildSymTable( NULL, pParseTree, pSymTab, pTypeTab );
	TypeCheck( NULL, pParseTree, pSymTab, pTypeTab );
	return TRUE;
}


//-----------------------------------------------------------------------------//
// ParseTree를 순회하면서 심볼테이블, 타입테이블을 생성한다
// pParent : 현재트리의 부모트리를 뜻한다. Root일경우 NULL이 된다.
// pTree : 현재 분석될 트리를 뜻한다.
// 변수에 데이타가 할당될때 타입이 정해지기 때문에 Build시에는 에러처리는 거의없다.
//-----------------------------------------------------------------------------//
BOOL CAnalyze::BuildSymTable( SParseTree *pParent, SParseTree *pTree, CSymTable *pSymTab, CTypeTable *pTypeTab )
{
	if( !pTree ) return FALSE;
	pTree->pSym = pSymTab;

	switch( pTree->nodekind )
	{
	case Stmt:
		{
			switch( pTree->kind )
			{
			case ProgramK:
				BuildSymTable( pTree, pTree->child[ 0], pSymTab, pTypeTab );
				BuildSymTable( pTree, pTree->child[ 1], pSymTab, pTypeTab );
				BuildSymTable( pTree, pTree->child[ 2], pSymTab, pTypeTab );
				BuildSymTable( pTree, pTree->child[ 3], pSymTab, pTypeTab );
				BuildSymTable( pTree, pTree->child[ 4], pSymTab, pTypeTab );
				break;

			case IncludeK:
				break;

			// 클래스선언된 것은 TypeTable에 추가된다.
			case ClassK:
				pTypeTab->NewType( STypeData(pTree->attr.name,pTree->attr.name,0,DT_CLASS) );
				BuildSymTable( pTree, pTree->child[ 0], pSymTab, pTypeTab );
				BuildSymTable( pTree, pTree->child[ 1], pSymTab, pTypeTab );
				break;
			// 클래스 선언시 혹은 import시 만들어진다.
			case VarDecK:
				{
					BOOL _class = (ClassK==pParent->kind);

					SParseTree *p1 = pTree->child[ 0];
					SParseTree *p2 = pTree->child[ 1];
					SParseTree *p3 = pTree->child[ 2];

					if( _class )	// 클래스 멤버변수
					{
						if( !pTypeTab->AddType(pParent->attr.name, STypeData(p2->attr.name,
											   p1->attr.name,p3->attr.fnum,DT_DATA)) )
							BuildError( pTree, "클래스와 멤버변수의 관계가 잘못되었습니다.\n", 1 );
					}
					else // import 변수
					{
						pSymTab->AddSymbol( p2->attr.name, TRUE, p3->attr.fnum );
						pSymTab->SetType( p2->attr.name, p1->attr.name );
					}

					BuildSymTable( pParent, pTree->child[ 0], pSymTab, pTypeTab );
					BuildSymTable( pParent, pTree->child[ 1], pSymTab, pTypeTab );
				}
				break;
			// 클래스 선언시 혹은 import시 만들어진다.
			case FuncDecK:
				{
					SParseTree *p1 = pTree->child[ 0];
					SParseTree *p2 = pTree->child[ 1];
					SParseTree *p3 = pTree->child[ 3];

					// 함수의 인자갯수를 얻는다.
					int argcnt = GetFunctionArgumentCount( pTree );
					if( !pTypeTab->AddType(pParent->attr.name, STypeData(p2->attr.name, 
										   p1->attr.name,p3->attr.fnum,DT_FUNC,argcnt)) )
					{
						BuildError( pTree, "클래스와 멤버함수의 관계가 잘못되었습니다.\n", 1 );
					}
					BuildSymTable( pTree, pTree->child[ 0], pSymTab, pTypeTab );
					BuildSymTable( pTree, pTree->child[ 1], pSymTab, pTypeTab );
					BuildSymTable( pTree, pTree->child[ 2], pSymTab, pTypeTab );
					BuildSymTable( pTree, pTree->child[ 3], pSymTab, pTypeTab );
				}
				break;

			// 전역,지역변수 개념은 없다.
			case ComposeK:
				BuildSymTable( pTree, pTree->child[ 0], pSymTab, pTypeTab );
				break;

			// 스크립트는 할당시 타입이 결정되고, 심볼테이블에 추가된다.
			case AssignK:
				pSymTab->AddSymbol( pTree->child[ 0]->attr.name );
				BuildSymTable( pParent, pTree->child[ 0], pSymTab, pTypeTab );
				BuildSymTable( pParent, pTree->child[ 1], pSymTab, pTypeTab );
				break;

			// If, While, Event의 ParentNode는 ComposeK를 가르키게 해야한다.
			case IfK:
				BuildSymTable( pParent, pTree->child[ 0], pSymTab, pTypeTab );
				BuildSymTable( pParent, pTree->child[ 1], pSymTab, pTypeTab );
				BuildSymTable( pParent, pTree->child[ 2], pSymTab, pTypeTab );
				break;

			case WhileK:
				BuildSymTable( pParent, pTree->child[ 0], pSymTab, pTypeTab );
				BuildSymTable( pParent, pTree->child[ 1], pSymTab, pTypeTab );
				break;

			case EventK:
				BuildSymTable( pParent, pTree->child[ 0], pSymTab, pTypeTab );
				BuildSymTable( pParent, pTree->child[ 1], pSymTab, pTypeTab );
				break;

			case ScriptWaitK:
			case ScriptEndK:
			case ScriptExitK:
				break;
			}
		}
		break;

	case Exp:
		{
			switch( pTree->kind )
			{
			case CallK:
				BuildSymTable( pTree, pTree->child[ 0], pSymTab, pTypeTab );
				BuildSymTable( pTree, pTree->child[ 1], pSymTab, pTypeTab );
				break;

			case OpK:
			case ConditionOpK:
				BuildSymTable( pTree, pTree->child[ 0], pSymTab, pTypeTab );
				BuildSymTable( pTree, pTree->child[ 1], pSymTab, pTypeTab );
				break;

			case ConstStrK:
				m_pStrTab->AddString( pTree->attr.name );
				break;

			case TypeK:
			case IdK:
			case FuncK:
			case ParamK:
			case ConstIntK:
			case ConstFloatK:
				break;
			}
		}
		break;
	}

	BuildSymTable( pParent, pTree->sibling, pSymTab, pTypeTab );
	return TRUE;
}


//-----------------------------------------------------------------------------//
// 변수 할당, 함수인자 타입을 체크한다.
//-----------------------------------------------------------------------------//
BOOL CAnalyze::TypeCheck( SParseTree *pParent, SParseTree *pTree, CSymTable *pSymTab, CTypeTable *pTypeTab )
{
	if( !pTree ) return TRUE;
	for( int i=0; i < MAXCHILD; ++i )
	{
		if( pTree->child[ i] )
			TypeCheck( pTree, pTree->child[ i], pTree->child[ i]->pSym, pTypeTab );
	}

	switch( pTree->nodekind )
	{
	case Stmt:
		{
			switch( pTree->kind )
			{
			case ProgramK:
			case ClassK:
			case VarDecK:
//			case VarK:
			case FuncDecK:
			case IncludeK:
			case ComposeK:
				break;

			// 스크립트는 할당시 타입이 결정되고, 심볼테이블에 추가된다.
			case AssignK:
				{
					// 변수가 선언만되고 값을 할당하지 않았다면 넘어감
					if( !pTree->child[ 1] )
						break;

					char *type = pSymTab->GetType( pTree->child[ 0]->attr.name );
					// 변수에 처음 값을 할당하면 타입을 설정한다.
					if( !type )
					{
						pSymTab->SetType( pTree->child[ 0]->attr.name, pTree->child[ 1]->type );
					}
					else
					{
						// 타입이 설정된 상태라면 서로의 타입을 비교한다.
						if( strcmp(type, pTree->child[ 1]->type) )
						{
							// float -> int
							// int -> float 은 허용한다.
							if( !strcmp(type, "float") || !strcmp(type, "int") &&
								!strcmp(pTree->child[ 1]->type, "float") || !strcmp(pTree->child[ 1]->type, "int") )
								break;

							TypeError( pTree, "다른 타입의 값을 할당하였습니다.\n" );
						}
					}
				}
				break;

			case IfK:
			case WhileK:
			case EventK:
				if( !pTree->child[ 0]->type || strcmp("bool", pTree->child[ 0]->type) )
					TypeError( pTree, "조건문에는 and, or 연산만 가능합니다.\n" );
				break;
			}
		}
		break;

	case Exp:
		{
			switch( pTree->kind )
			{
			case CallK:
				pTree->type = GetSymbolType( pTree->child[ 0]->attr.name, pSymTab, pTypeTab, pTree );
				if( !pTree->type )
					TypeError( pTree->child[ 0], "존재하지 않는 함수입니다.\n", 1 );
				break;

			case TypeK:
				if( IsSymbol(pTree->attr.name, pSymTab, pTypeTab, pTree) )
				{
					pTree->type = pTree->attr.name;
				}
				else
				{
					TypeError( pTree, "존재하지 않는 Type입니다.\n", 1 );
					pTree->type = "void"; // 디폴트 void
				}
				break;

			case IdK:
				{
					if( IsSymbol(pTree->attr.name, pSymTab, pTypeTab, pTree) )
					{
						pTree->type = GetSymbolType( pTree->attr.name, pSymTab, pTypeTab, pTree );
					}
					else
					{
						TypeError( pTree, "존재하지 않는 ID입니다.\n", 1 );
						pTree->type = "void"; // 디폴트 void
					}
				}
				break;

			case OpK:
				{
					// 산술연산은 int, float형만 가능하다.

					pTree->type = "void"; // default
					if( strcmp("int",pTree->child[ 0]->type) && strcmp("float",pTree->child[ 0]->type) )
						TypeError( pTree, "잘못된 연산을 하였습니다.\n" );
					else if( strcmp(pTree->child[ 0]->type, pTree->child[ 1]->type) )
						TypeError( pTree, "맞지않은 타입을 연산하였습니다.\n" );
					else if( EQ == pTree->attr.op || LT == pTree->attr.op || RT == pTree->attr.op )
						pTree->type = "bool";
					else
					{
						pTree->type = pTree->child[ 0]->type;
					}
				}
				break;

			case ConditionOpK:	pTree->type = "bool";	break;
			case ConstIntK:		pTree->type = "int";	break;
			case ConstFloatK:	pTree->type = "float";	break;
			case ConstStrK:		pTree->type = "string";	break;
			case VarNameK:		break;
			}
		}
		break;
	}

	TypeCheck( pParent, pTree->sibling, pSymTab, pTypeTab );

	return TRUE;
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
char* CAnalyze::GetSymbolType( char *pSymbolName, ns_script::CSymTable *pSymTab, ns_script::CTypeTable *pTypeTab, 
							   ns_script::SParseTree *pTree )
{
	char *reval = NULL;
	char *type = pSymTab->GetType( pSymbolName );
	reval = type;

	char *pdot = strchr( pSymbolName, '.' );
	if( !pdot )
	{
		// 전역변수 or 전역함수
		char *globalfunc = pTypeTab->GetType( pSymbolName );
		if( type && globalfunc )
			TypeError( pTree, "Type과 ID에 동일한 ID가 존재합니다.\n" );
		reval = (type)? type : globalfunc;
	}
	return reval;
}
// GetSymbolType 함수와 비슷하지만 심볼의 타입에 상관없이 심볼이 등록되어있는지만 검사한다.
BOOL CAnalyze::IsSymbol( char *pSymbolName, ns_script::CSymTable *pSymTab, ns_script::CTypeTable *pTypeTab, 
						 ns_script::SParseTree *pTree )
{
	BOOL reval = FALSE;
	BOOL bsym = pSymTab->IsSymbol( pSymbolName );
	reval = bsym;

	char *pdot = strchr( pSymbolName, '.' );
	if( !pdot )
	{
		// 전역변수 or 전역함수
		BOOL bglobalfunc = pTypeTab->IsType( pSymbolName );
		if( bsym && bglobalfunc )
			TypeError( pTree, "Type과 ID에 동일한 ID가 존재합니다.\n" );
		reval = bsym || bglobalfunc;
	}
	return reval;
}


//-----------------------------------------------------------------------------//
// 함수선언Tree에서 함수인자갯수를 계산해서 리턴한다.
//	func_dec -> 'fn' type id '(' param_list ')' ':' num
//-----------------------------------------------------------------------------//
int CAnalyze::GetFunctionArgumentCount( ns_script::SParseTree *pFuncTree )
{
	int reval = 0;
	SParseTree *p = pFuncTree->child[ 2];
	while( p )
	{
		reval++;
		p = p->sibling;
	}
	return reval;
}


void CAnalyze::BuildError( SParseTree *t, char *message, int type )
{
	if( 1 == type ) // link err
		g_Dbg.Console( "build error [%s], %s", m_FileName, message );
	else if( 0 == type ) // syntax err
		g_Dbg.Console( "build error [%s] at line (%d): token %s %s", m_FileName, t->lineno, t->attr.name, message );
	m_bBuildError = TRUE;
}

void CAnalyze::TypeError( SParseTree *t, char *message, int type )
{
	if( 0 == type )
		g_Dbg.Console( "type error [%s] at line (%d): %s", m_FileName, t->lineno, message );
	else if( 1 == type ) // syntax err
		g_Dbg.Console( "type error [%s] at line (%d): token [%s] %s", m_FileName, t->lineno, t->attr.name, message );
	m_bTypeError = TRUE;
}
