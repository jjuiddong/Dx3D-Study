
#include "global.h"
#include "script_global.h"
#include "script_codegen.h"

using namespace std;
using namespace ns_script;



//-----------------------------------------------------------------------------//
// 기계어코드를 생성한다.
// pSourceFileName: 원본 파일이름, 화장자를 gm으로 바꾼다.
//-----------------------------------------------------------------------------//
BOOL CCodeGen::CodeGenerate( SParseTree *pTree, CTypeTable *pTypeTab, CStringTable *pStrTab, char *pSourceFileName )
{
	char filename[ MAX_PATH];
	strcpy( filename, pSourceFileName );
	strcpy( &filename[ strlen(filename)-3], "gm" );

	m_Fp = fopen( filename, "w" );
	if( !m_Fp ) return FALSE;

//	m_bComment = FALSE;
	m_pStrTab = pStrTab;

	// String Table 저장
	int symsize = pTree->pSym->GetTotalMemSize();
	fwrite( &symsize, 1, sizeof(int), m_Fp ); // symbol table크기를 저장한다.
	pStrTab->WriteStringTable( m_Fp );	// string table 저장 

	char s[ MAX_PATH];
	strcpy( s, "File: " );
	strcat( s, pSourceFileName );
	EmitComment( "GameMachine compilation to GM code" );
	EmitComment( s );
	// generate standard prelud
	EmitComment( "Standard prelud:" );
	EmitRM( "LD", sp, 0, ac, "load maxaddress from location 0" );
	EmitRM( "LD", bp, 0, ac, "load maxaddress from location 0" );
	EmitRM( "ST", ac, 0, ac, "clear location 0" );
	EmitComment( "End of standard prelude." );

	// 임포트된 전역변수를 로드한다.
	GenLoadGlobalVarPointer( pTree->pSym );

	CodeGen( pTree, pTypeTab );

	// event에 관련된 나머지부분을 처리한다.
	// 1. scriptwait 명령어부분에 goto eventlist 코드추가
	// 2. event list loop 코드추가
	if( 0 > m_EventListLoc ) // event list가 없다면 halt로 goto
		m_EventListLoc = EmitSkip( 0 );

	list<int>::iterator i = m_ScrWaitLoc.begin();
	while( m_ScrWaitLoc.end() != i )
	{
		int loc = *i++;
		EmitBackup( loc );
		EmitRM( "LDC", pc, m_EventListLoc, 0, "jmp to eventlist" );
	}
	EmitRestore();
	EmitRM( "LDC", pc, m_EventListLoc, 0, "jmp to eventlist" );

	EmitComment( "End of execution." );
	EmitRO( "HALT", 0, 0, 0, " " );

	// binary 저장
	WriteBinary( pTree, pStrTab, pSourceFileName );

	fclose( m_Fp );
	return TRUE;
}


//-----------------------------------------------------------------------------//
// 코드생성
//-----------------------------------------------------------------------------//
BOOL CCodeGen::CodeGen( SParseTree *pTree, CTypeTable *pTypeTab )
{
	if( pTree )
	{
		switch( pTree->nodekind )
		{
		case Stmt:
			GenStmt( pTree, pTypeTab );
			break;
		case Exp:
			GenExp( pTree, pTypeTab );
			break;
		default:
			break;
		}
		CodeGen( pTree->sibling, pTypeTab );
	}
	return TRUE;
}


//-----------------------------------------------------------------------------//
// Statement 코드 생성 
//-----------------------------------------------------------------------------//
BOOL CCodeGen::GenStmt( SParseTree *pTree, CTypeTable *pTypeTab )
{
	CSymTable *psym = pTree->pSym;

	switch( pTree->kind )
	{
	case ProgramK:
		CodeGen( pTree->child[ 0], pTypeTab );
		CodeGen( pTree->child[ 1], pTypeTab );
		CodeGen( pTree->child[ 2], pTypeTab );
		CodeGen( pTree->child[ 3], pTypeTab );
		CodeGen( pTree->child[ 4], pTypeTab );
		break;

	case ComposeK:
		CodeGen( pTree->child[ 0], pTypeTab );
		break;

	case AssignK:
		{
			int id = psym->GetSymbolId( pTree->child[ 0]->attr.name );

			CodeGen( pTree->child[ 1], pTypeTab );

			if( id )
			{
				// import data
				GenImportData( pTree, pTypeTab );
			}
			else
			{
				int offset = psym->GetMemOffset( pTree->child[ 0]->attr.name );
				EmitRM( "ST", ac, offset, gp, "assign: store value" );

				// 클래스일경우 임포트된 멤버변수를 모두 로드한다.
				if( psym->IsImportClass(pTree->child[ 0]->attr.name) )
					GenLoadClassPointer( pTree->child[ 0]->attr.name, psym, pTypeTab );
			}
		}
		break;

	case IfK:
		{
			EmitComment( "-> if" );
			SParseTree *p1 = pTree->child[ 0];
			SParseTree *p2 = pTree->child[ 1];
			SParseTree *p3 = pTree->child[ 2];
			// generate code for test expression
			CodeGen( p1, pTypeTab );
			int savedLoc1 = EmitSkip( 1 );
			EmitComment( "if: jump to else belongs here" );
			// recurse on then part
			CodeGen( p2, pTypeTab );
			int savedLoc2 = EmitSkip( 1 );
			EmitComment( "if: jump to end belongs here" );
			int currentLoc = EmitSkip( 0 );
			EmitBackup( savedLoc1 );
			EmitRM_Abs( "JEQ", ac, currentLoc, "if: jmp to else" );
			EmitRestore();
			// recurse on else part
			CodeGen( p3, pTypeTab );
			currentLoc = EmitSkip( 0 );
			EmitBackup( savedLoc2 );
			EmitRM_Abs( "LDA", pc, currentLoc, "jmp to end" );
			EmitRestore();
			EmitComment( "<- if" );
		}
		break;

	case WhileK:
		{
			EmitComment( "-> while" );
			int savedLoc1 = EmitSkip( 0 ); // head of while
			CodeGen( pTree->child[ 0], pTypeTab );
			int savedLoc2 = EmitSkip( 1 ); // exp == 0 이면 while문을 벗어난다.
			CodeGen( pTree->child[ 1], pTypeTab );
			EmitComment( "jump to head of while" );
			EmitRM( "LDC", pc, savedLoc1, 0, "jump to head of while" );
			int currentLoc = EmitSkip( 0 );
			EmitBackup( savedLoc2 );
			EmitRM( "JEQ", ac, currentLoc, 0, "while: jmp to outside while" );
			EmitRestore();
		}
		break;

	case EventK:
		{
			// EventList가 시작되는 location을 저장한다.
			if( -1 == m_EventListLoc )
				m_EventListLoc = EmitSkip( 0 );

			EmitComment( "-> event" );
			SParseTree *p1 = pTree->child[ 0];
			SParseTree *p2 = pTree->child[ 1];
			
			// generate code for test expression
			CodeGen( p1, pTypeTab );
			int savedLoc1 = EmitSkip( 1 );
			EmitComment( "event: jump to end belongs here" );
			// recurse on then part
			CodeGen( p2, pTypeTab );
			int currentLoc = EmitSkip( 0 );
			EmitBackup( savedLoc1 );
			EmitRM_Abs( "JEQ", ac, currentLoc, "event: jmp to end" );
			EmitRestore();
			EmitComment( "<- event" );
		}
		break;

	case VarDecK:
	case ClassK:
	case FuncDecK:
	case IncludeK:
		// 아무일도 없음
		break;

	case ScriptWaitK:
		// ScriptWait 명령이 호출된 위치를 저장한다.
		// EventList 코드생성부분에서 goto문이 작성된다.
		EmitComment( "Script Wait. Goto Event List" );
		m_ScrWaitLoc.push_back( EmitSkip(1) );
		break;

	case ScriptEndK:
		EmitComment( "Script End." );
		EmitRO( "HALT", 1, 0, 0, " " );
		break;

	case ScriptExitK:
		EmitComment( "Script Exit." );
		EmitRO( "HALT", 2, 0, 0, " " );
		break;

	default:
		g_Dbg.Console( "컴파일이 잘못되었습니다 GenStmt() \n" );
		break;
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// Exp 코드생성
//-----------------------------------------------------------------------------//
BOOL CCodeGen::GenExp( SParseTree *pTree, CTypeTable *pTypeTab )
{
	CSymTable *psym = pTree->pSym;

	switch( pTree->kind )
	{
	case ConditionOpK:
	case OpK:
		{
			CodeGen( pTree->child[ 0], pTypeTab );
			EmitRO( "PUSH", ac, 0, 0, "op: push left" );
			CodeGen( pTree->child[ 1], pTypeTab );
			EmitRO( "POP", ac1, 0, 0, "op: load left" );

			switch( pTree->attr.op )
			{
			case PLUS:		EmitRO( "ADD", ac, ac1, ac, "op +" );	break;
			case MINUS:		EmitRO( "SUB", ac, ac1, ac, "op -" );	break;
			case TIMES:		EmitRO( "MUL", ac, ac1, ac, "op *" );	break;
			case DIV:		EmitRO( "DIV", ac, ac1, ac, "op /" );	break;
			case REMAINDER:	EmitRO( "REM", ac, ac1, ac, "op %" );	break;
			case OR:		EmitRO( "OR", ac, ac1, ac, "op ||" );	break;
			case AND:		EmitRO( "AND", ac, ac1, ac, "op &&" );	break;

			case LT:	// ac > ac1
				EmitRO( "SUB", ac, ac, ac1, "op >" );
				EmitRM( "JLT", ac, 2, pc, "br if true" );
				EmitRM( "LDC", ac, 0, ac, "false case" );
				EmitRM( "LDA", pc, 1, pc, "unconditional jmp" );
				EmitRM( "LDC", ac, 1, ac, "true case" );
				break;

			case RT:	// ac < ac1
				EmitRO( "SUB", ac, ac1, ac, "op <" );
				EmitRM( "JLT", ac, 2, pc, "br if true" );
				EmitRM( "LDC", ac, 0, ac, "false case" );
				EmitRM( "LDA", pc, 1, pc, "unconditional jmp" );
				EmitRM( "LDC", ac, 1, ac, "true case" );
				break;

			case EQ:	// ==
				EmitRO( "SUB", ac, ac1, ac, "op ==" );
				EmitRM( "JEQ", ac, 2, pc, "br if true" );
				EmitRM( "LDC", ac, 0, ac, "false case" );
				EmitRM( "LDA", pc, 1, pc, "unconditional jmp" );
				EmitRM( "LDC", ac, 1, ac, "true case" );
				break;

			case NEQ:	// !=
				EmitRO( "SUB", ac, ac1, ac, "op ==" );
				EmitRM( "JNE", ac, 2, pc, "br if false" );
				EmitRM( "LDC", ac, 0, ac, "false case" );
				EmitRM( "LDA", pc, 1, pc, "unconditional jmp" );
				EmitRM( "LDC", ac, 1, ac, "true case" );
				break;
			}
		}
		break;

	case ConstIntK:	
		EmitComment( "-> Const Int" );
		EmitRM( "LDC", ac, pTree->attr.num, 0, "load const" );
		break;

	case ConstFloatK:
		EmitComment( "-> Const Float" );
		EmitRM( "LDC", ac, pTree->attr.fnum, 0, "load const" );
		break;

	case ConstStrK:
		{
			// 스트링테이블은 전역변수 다음에 저장되기때문에 string offset값은 
			// 심볼테이블갯수 값이 더해진다.
			EmitComment( "-> Const String" );
			int offset = m_pStrTab->GetOffset( pTree->attr.name ) + psym->GetTotalMemSize();
			EmitRM( "LDC", ac, offset, 0, pTree->attr.name );
		}
		break;

	case IdK:
		{
			// import변수라면 게임에서 data를 받아오는 처리가 필요하다.
			// 그냥 변수일경우 stack상에 저장된 데이타를 얻으면 된다.

			EmitComment( "-> Idk" );

			int id = psym->GetSymbolId( pTree->attr.name );
			if( 0 == id )
			{
				int offset = psym->GetMemOffset( pTree->attr.name );
				EmitRM( "LD", ac, offset, gp, "load id value" );
			}
			else
			{
				// import data
				GenImportData( pTree, pTypeTab );
			}

			EmitComment( "<- Idk" );
		}
		break;

	case CallK:
		GenCall( pTree, pTypeTab );
		break;

	default:
		g_Dbg.Console( "컴파일이 잘못되었습니다 GenExp() \n" );
		break;
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// Function Call Code 생성
// import 변수를 일괄적으로 처리하기위해서 만들어진 함수다.
// import변수는 처음 할당될 때 어플리캐이션으로 부터 포인터를 얻어와서, 다음부터
// 어플리케이션에 접근하지 않고 바로 데이타를 할당하거나, 엑세스할수있게 된다.
//
// stack 구조
//		top		return space
//		 |		--------------
//		 |		stack pointer
//		 |		--------------
//	   bottom   argument
// 함수호출이 끝난후 return value는 ac 레지스터에 저장된다.
//-----------------------------------------------------------------------------//
void CCodeGen::GenCall( SParseTree *pTree, CTypeTable *pTypeTab )
{
	int id;
	char *comment;
	CSymTable *psym = pTree->pSym;
	
	switch( pTree->kind )
	{
	case CallK:
		{
			EmitRM( "LDA", ac2, 0, sp, "Load current sp" );			// sp를 저장한다.

			SParseTree *p1 = pTree->child[ 0];
			SParseTree *p2 = pTree->child[ 1];

			//----------------------------------------------------
			// 가장 마지막에 있는 인자부터 push 한다.
			stack< SParseTree* > que;
			while( p2 )
			{
				que.push( p2 );
				p2 = p2->sibling;
			}
			while( !que.empty() )
			{
				SParseTree *p = que.top(); que.pop();
				GenExp( p, pTypeTab );
				EmitRO( "PUSH", ac, 0, 0, "push argument" );
			}
			//----------------------------------------------------

			// class member함수 호출이라면 인자에 class pointer를 넣는다. (stack top에 this 포인터가 들어감)
			if( strchr(p1->attr.name, '.') )
			{
				int offset = psym->GetClassMemOffset( p1->attr.name );
				EmitRM( "LD", ac, offset, gp, "load this pointer" );
				EmitRO( "PUSH", ac, 0, 0, "push this pointer" );
			}

			id = psym->GetSymbolId( p1->attr.name );
			comment = p1->attr.name;

			EmitRO( "PUSH", ac2, 0, 0, "push sp" );					// sp를 push한다.
			EmitRO( "PUSH", ac, 0, 0, "push return_value space" );	// 리턴받을 정보가 저장될 공간을 만든다.

			EmitCall( id, comment );

			EmitRO( "POP", ac, 0, 0, "pop return_value" );			// 리턴값 ac에 할당
			EmitRO( "POP", ac1, 0, 0, "pop sp" );					// 함수가 호출되기전의 stack pointer로 돌린다.
			EmitRM( "LDA", sp, 0, ac1, "restore sp" );				// sp를 저장한다.
		}
		break;

	case IdK:
		g_Dbg.Console( "CodeGen Err,  GenCall.\n" );
		break;

	case AssignK:
		g_Dbg.Console( "CodeGen Err,  GenCall.\n" );
		break;
	}
}


//-----------------------------------------------------------------------------//
// 임포트된 클래스의 멤버변수는 어플리케이션으로 부터 포인터를 얻어온다.
// 함수콜부분은 GenCall() 함수와 동일하다.
// pSymbolName: 임포트된 클래스의 변수명
//-----------------------------------------------------------------------------//
void CCodeGen::GenLoadClassPointer( char *pSymbolName, CSymTable *pSym, CTypeTable *pTypeTab )
{
	if( pSym->IsLoaded(pSymbolName) ) return;

	char *ptype = pSym->GetType( pSymbolName );
	list<STypeData> *pmember = pTypeTab->GetMemberList( ptype );
	if( pmember->size() <= 1 ) return;

	EmitComment( "-> Load Import Class" );
	char buf[ 128];

	int class_offset = pSym->GetClassMemOffset( pSymbolName );
	list<STypeData>::iterator it = pmember->begin();
	while( pmember->end() != it )
	{
		STypeData dat = *it++;
		if( DT_FUNC == dat.state || DT_CLASS == dat.state ) continue;

		sprintf( buf, "Load Pointer id: %s", dat.name );
		EmitComment( buf );

		EmitRM( "LDA", ac2, 0, sp, "Load current sp" );		// sp를 저장한다.

		// 포인터를 얻기위해서 인자로 ID만 필요하다.
		EmitRM( "LDC", ac, dat.id, 0, "load id" );
		EmitRO( "PUSH", ac, 0, 0, "push argument" );	// 인자를 push 한다.

		// 인자에 class pointer를 넣는다. (stack top에 this 포인터가 들어감)
		EmitRM( "LD", ac, class_offset, gp, "load this pointer" );
		EmitRO( "PUSH", ac, 0, 0, "push this pointer" );
		EmitRO( "PUSH", ac2, 0, 0, "push sp" );					// sp를 push한다.
		EmitRO( "PUSH", ac, 0, 0, "push return_value space" );	// 리턴받을 정보가 저장될 공간을 만든다.

		EmitCall( LoadPointer, "LoadPointer" );

		EmitRO( "POP", ac, 0, 0, "pop return_value" );			// 리턴값 ac에 할당
		EmitRO( "POP", ac1, 0, 0, "pop sp" );					// 함수가 호출되기전의 stack pointer로 돌린다.
		EmitRM( "LDA", sp, 0, ac1, "restore sp" );				// sp를 저장한다.

		// 리턴된 포인터를 저장한다.
		EmitRM( "ST", ac, class_offset + dat.offset, gp, "assign: store pointer" );		
	}

	pSym->Loaded( pSymbolName );
}


//-----------------------------------------------------------------------------//
// 임포트된 글로벌 변수를 어플리케이션 으로부터 포인터를 얻어온다.
// GenClassLoadPointer() 함수는 클래스 멤버변수를 임포트하지만 이 함수는 글로벌 변수를
// 처리하는데서 다르다. 
//-----------------------------------------------------------------------------//
void CCodeGen::GenLoadGlobalVarPointer( CSymTable *pSym )
{
	list<string> lst;
	pSym->GetGlobalImportData( &lst );

	EmitComment( "-> Load Import Global variable" );
	char buf[ 128];

	list<string>::iterator it = lst.begin();
	while( lst.end() != it )
	{
		int id = pSym->GetSymbolId( (char*)it->c_str() );
		int offset = pSym->GetMemOffset( (char*)it->c_str() );

		sprintf( buf, "Load Pointer id: %s", it->c_str() );
		EmitComment( buf );

		EmitRM( "LDA", ac2, 0, sp, "Load current sp" );		// sp를 저장한다.

		// 포인터를 얻기위해서 인자로 ID만 필요하다.
		EmitRM( "LDC", ac, id, 0, "load id" );
		EmitRO( "PUSH", ac, 0, 0, "push argument" );	// 인자를 push 한다.

		EmitRO( "PUSH", ac2, 0, 0, "push sp" );					// sp를 push한다.
		EmitRO( "PUSH", ac, 0, 0, "push return_value space" );	// 리턴받을 정보가 저장될 공간을 만든다.

		EmitCall( LoadPointer, "LoadPointer" );

		EmitRO( "POP", ac, 0, 0, "pop return_value" );			// 리턴값 ac에 할당
		EmitRO( "POP", ac1, 0, 0, "pop sp" );					// 함수가 호출되기전의 stack pointer로 돌린다.
		EmitRM( "LDA", sp, 0, ac1, "restore sp" );				// sp를 저장한다.

		// 리턴된 포인터를 저장한다.
		EmitRM( "ST", ac, offset, gp, "assign: store pointer" );		

		++it;
	}
}


//-----------------------------------------------------------------------------//
// 임포트된 데이타에 접근하기위한 코드를 생성한다.
// 임포트된 변수는 어플리케이션의 포인터를 가지고 있다. 그래서 머신은 자신의
// 메모리에서 검색하는게 아니라 외부메모리를 참조해야 하기때문에 기존의 데이타를
// 얻어오는 방법과 달라진다. 
// 외부메모리에서 데이타를 얻거나 저장하기 위해서는 opILDF,opILD, opISTF, opIST
// 명령어를 사용해야 한다. 현재 int, float형만 지원한다. 
//-----------------------------------------------------------------------------//
void CCodeGen::GenImportData( SParseTree *pTree, CTypeTable *pTypeTab )
{
	CSymTable *psym = pTree->pSym;
	
	switch( pTree->kind )
	{
	case IdK:
		{
			char *type = psym->GetType( pTree->attr.name );
			int offset = psym->GetMemOffset( pTree->attr.name );
			if( !strcmp("float", type) )
			{
				EmitRM( "ILDF", ac, offset, gp, "load id import value" );
			}
			else if( !strcmp("int", type) )
			{
				EmitRM( "ILD", ac, offset, gp, "load id import value" );
			}
			else
			{
				// err, 시스템상으로는 여기에 도달할수 없다.
				g_Dbg.Console( "CodeGen Err,  GenImportData.\n" );
			}
		}
		break;

	case AssignK:
		{
			char *type = psym->GetType( pTree->child[ 0]->attr.name );
			int offset = psym->GetMemOffset( pTree->child[ 0]->attr.name );
			if( !strcmp("float", type) )
			{
				EmitRM( "ISTF", ac, offset, gp, "assign: store import value" );
			}
			else if( !strcmp("int", type) )
			{
				EmitRM( "IST", ac, offset, gp, "assign: store import value" );
			}
			else
			{
				// error
				g_Dbg.Console( "CodeGen Err,  class타입은 할당되지 않습니다.\n" );
			}
		}
		break;
	}
}


OPCODE FindOp( char *command )
{
	// op code 찾기
	OPCODE op = opHALT;
	int n = op;
	while( (op < opRALim) && (strncmp(g_opCodeTab[ op], command, 4) != 0) )
	{
		++n;
		op = (OPCODE)n;
	}
	if( op >= opRALim )
		return opHALT;
	return op;	
}


//-----------------------------------------------------------------------------//
// code를 바이너리로 저장한다.
// pc (program counter) 순으로 저장된다.
//-----------------------------------------------------------------------------//
BOOL CCodeGen::WriteBinary( SParseTree *pTree, CStringTable *pStrTab, char *pSourceFileName )
{
	m_InstList.sort(); // loc 오름차순 정렬

	// 확장자를 bgm으로 바꾼다.
	char filename[ MAX_PATH];
	strcpy( filename, pSourceFileName );
	strcpy( &filename[ strlen(filename)-3], "bgm" );

	FILE *fp = fopen( filename, "wb" );
	if( !fp ) return FALSE;

	// String Table 저장
	int symsize = pTree->pSym->GetTotalMemSize();
	fwrite( &symsize, 1, sizeof(int), fp ); // symbol table크기를 저장한다.
	pStrTab->WriteStringTable( fp );		// string table 저장 

	// instruction 수 저장
	int instcnt = m_InstList.size();
	fwrite( &instcnt, 1, sizeof(int), fp );

	list<SInst>::iterator i = m_InstList.begin();
	while( m_InstList.end() != i )
	{
		OPCODE op = FindOp( i->op );
		fwrite( &op, 1, sizeof(int), fp );
		fwrite( &i->r, 1, sizeof(int), fp );
		fwrite( &i->s, 1, sizeof(int), fp );
		fwrite( &i->t, 1, sizeof(int), fp );
		++i;
	}
	fclose( fp );

	return TRUE;
}


//--------------------------------------------------------
// procedure emitRO emits a register-only
// TM instruction
// op = the opcode
// r = target register
// s = 1st source register
// t = 2nd source register
// c = a comment to be printed if m_bComment is TRUE
//--------------------------------------------------------
void CCodeGen::EmitRO( char *op, int r, int s, int t, char *c )
{
	fprintf( m_Fp, "%3d:  %5s  %d,%d,%d ", m_nEmitLoc++, op, r, s, t );
	if( m_bComment ) fprintf( m_Fp, "\t%s", c );
	fprintf( m_Fp, "\n" );
	if( m_nHighEmitLoc < m_nEmitLoc ) m_nHighEmitLoc = m_nEmitLoc;

	m_InstList.push_back( SInst(m_nEmitLoc-1, op, r, s, t, c) );
}


//--------------------------------------------------------
// procedure emitRM emits a register-to-memory
// TM instruction
// op = the opcode
// r = target register
// d = the offset
// s = the base register
// c = a comment to be printed if m_bComment is TRUE
//--------------------------------------------------------
void CCodeGen::EmitRM( char *op, int r, int d, int s, char *c )
{
	fprintf( m_Fp, "%3d:  %5s  %d,%d(%d) ", m_nEmitLoc++, op, r, d, s );
	if( m_bComment ) fprintf( m_Fp, "\t%s", c );
	fprintf( m_Fp, "\n" );
	if( m_nHighEmitLoc < m_nEmitLoc ) m_nHighEmitLoc = m_nEmitLoc;

	m_InstList.push_back( SInst(m_nEmitLoc-1, op, r, d, s, c) );
}


//-----------------------------------------------------------------------------//
// 함수 Call명령어를 삽입한다. 
//-----------------------------------------------------------------------------//
void CCodeGen::EmitCall( int id, char *c )
{
	fprintf( m_Fp, "%3d:   CALL  %d,0,0 ", m_nEmitLoc++, id );
	if( m_bComment ) fprintf( m_Fp, "\t%s", c );
	fprintf( m_Fp, "\n" );
	if( m_nHighEmitLoc < m_nEmitLoc ) m_nHighEmitLoc = m_nEmitLoc;

	m_InstList.push_back( SInst(m_nEmitLoc-1, "CALL", id, 0, 0, c) );
}


int CCodeGen::EmitSkip( int howMany )
{
	int i = m_nEmitLoc;
	m_nEmitLoc += howMany;
	if( m_nHighEmitLoc < m_nEmitLoc ) m_nHighEmitLoc = m_nEmitLoc;
	return i;
}


void CCodeGen::EmitBackup( int loc )
{
	if( loc > m_nHighEmitLoc ) EmitComment( "BUG in emitBackup" );
	m_nEmitLoc = loc;
}


void CCodeGen::EmitRestore()
{
	m_nEmitLoc = m_nHighEmitLoc;
}


//--------------------------------------------------------
// procedure emitRM_Abs converts an absolute reference
// to a pc-relative reference when emitting a
// register-to-memory TM instruction
// op = the opcode
// r = target register
// a = the absolute location in memory
// c = a comment to be printed if m_bComment is TRUE
//--------------------------------------------------------
void CCodeGen::EmitRM_Abs( char *op, int r, int a, char *c )
{
	int d = a-(m_nEmitLoc+1);
	fprintf( m_Fp, "%3d:  %5s  %d,%d(%d) ", m_nEmitLoc, op, r, d, pc );
	++m_nEmitLoc;
	if( m_bComment ) fprintf( m_Fp, "\t%s", c );
	fprintf( m_Fp, "\n" );
	if( m_nHighEmitLoc < m_nEmitLoc ) m_nHighEmitLoc = m_nEmitLoc;

	m_InstList.push_back( SInst(m_nEmitLoc-1, op, r, d, pc, c) );
}


void CCodeGen::EmitComment( char *c )
{
	if( m_bComment ) fprintf( m_Fp, "* %s\n", c );
}


