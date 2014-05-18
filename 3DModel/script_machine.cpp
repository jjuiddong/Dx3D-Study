
#include "global.h"
#include "script_global.h"
#include "script_codegen.h"
#include "script_machine.h"

using namespace ns_script;


////////////////////////////////////////////////////////////////////////////////////////////////
// CMachine

CMachine::CMachine()
{

}
CMachine::~CMachine() 
{
	Clear();
}


//-----------------------------------------------------------------------------//
// 스크립트 실행
//-----------------------------------------------------------------------------//
BOOL CMachine::ExecuteScript( char *pFileName, void callback (int,ns_script::CProcessor*), char *pArgument )
{
	ProcStack *st = new ProcStack;
	if( !ExecuteScriptChild(st, pFileName, callback, pArgument) )
	{
		delete st;
		return FALSE;
	}

	m_ProcList.push_back( st );
	return TRUE;
}


//-----------------------------------------------------------------------------//
// 실행되고 있는 프로세서의 자식으로 프로세서가 동작한다.
// 즉 자식 프로세서가 종료되면 부모 프로세서로 권한이 바뀌게 된다는 의미이다.
// pStack: 부모 프로세서가 저장된 스택
// pFileName : 실행될 스크립트 파일명
// callback : 콜백함수
// pArgument : 실행인자 
//-----------------------------------------------------------------------------//
BOOL CMachine::ExecuteScriptChild( ProcStack *pStack, char *pFileName, 
								  void callback (int,ns_script::CProcessor*), char *pArgument )
{
	CProcessor *proc = new CProcessor;
	if( !proc->ExecuteScript(this, pFileName, callback, pArgument) )
	{
		delete proc;
		return FALSE;
	}

	pStack->push( proc );
	return TRUE;
}


//-----------------------------------------------------------------------------//
// Processor 실행
// return value : -2 = NotRun
//				  -1 = Success
//				   0 = Halt 0	Normal End
//				   1 = Halt 1	ScriptEnd
//				   2 = Halt 2	ScriptExit
//				   3 = New Execute Script
//-----------------------------------------------------------------------------//
int CMachine::Run()
{
	if( m_ProcList.empty() ) return -2;

	ProcList::iterator it = m_ProcList.begin();
	while( m_ProcList.end() != it )
	{
		ProcStack *st = *it;
		//int result = st->top()->Run();

		CProcessor *p = st->top();
		int result = p->Run();

		if( 0 <= result )
		{
			// ScriptEnd, Normal End
			if( 0 == result || 1 == result )
			{
				CProcessor *p = st->top(); st->pop();
				delete p;
				if( st->empty() ) delete st;
				it = m_ProcList.erase( it );				
			}
			// ScriptExit
			else if( 2 == result )
			{
				Clear();
			}
			// New ExecuteScript
			else if( 3 == result )
			{
				CProcessor *proc = st->top();
				ExecuteScriptChild( st, proc->GetArgumentStr(0), proc->m_pCallFunc, NULL );
			}
		}
		++it;
	}

	return -1;
}


void CMachine::Clear()
{
	ProcList::iterator it = m_ProcList.begin();
	while( m_ProcList.end() != it )
	{
		ProcStack *st = *it++;
		while( !st->empty() )
		{
			delete st->top();
			st->pop();
		}
		delete st;
	}
	m_ProcList.clear();

}


////////////////////////////////////////////////////////////////////////////////////////////////
// CProcessor


char *g_stepResultTab[] = 
{
	"OK", "Input", "Halted", "Instruction Memory Fault", 
	"Data Memory Fault", "Division by 0"
};


CProcessor::CProcessor() : m_pGm(NULL), m_StepCnt(0), m_bRun(FALSE)
{
	SetNumberOfExecuteInst( 200 );
	m_pArgument[ 0] = NULL;
}

CProcessor::~CProcessor() 
{
	Clear();
}

void CProcessor::Clear()
{
	if( m_pGm )
	{
		fclose( m_pGm );
		m_pGm = NULL;
	}
}


//-----------------------------------------------------------------------------//
// gm 스크립트 실행
//-----------------------------------------------------------------------------//
BOOL CProcessor::ExecuteScript( CMachine *pMachine, char *pFileName, void callback (int,ns_script::CProcessor*), 
							    char *pArgument )
{
	m_pMachine = pMachine;
	m_iCountFlag = 1;
	m_TraceFlag = 1;
	m_pCallFunc = callback;
	if( pArgument ) strcpy( m_pArgument, pArgument );

	char filename[ MAX_PATH];
	strcpy( filename, pFileName );
	strcpy( &filename[ strlen(filename)-3], "gm" );

	// BinaryFile(bgm) 을 먼저읽고 없으면 gm파일을 읽는다.	
	BOOL result = ReadInstructions_Binary( filename );
	if( !result ) result = ReadInstructions( filename );
	if( !result ) return FALSE;

	m_bRun = TRUE;

	Clear();

	return TRUE;
}


//-----------------------------------------------------------------------------//
// 정해진 갯수만큼 instruction을 실행한다.
// return value : -2 = NotRun
//				  -1 = Success
//				   0 = Halt 0	Normal End
//				   1 = Halt 1	ScriptEnd
//				   2 = Halt 2	ScriptExit
//				   3 = New Execute Script
//-----------------------------------------------------------------------------//
int CProcessor::Run()
{
	if( !m_bRun ) return -2;

	int cnt = 0, stepResult = srOKAY;
	while( stepResult == srOKAY && cnt < m_nNumberOfExecuteInst )
	{
		stepResult = stepTM();
		++cnt;
	}
	m_StepCnt += cnt;

//	if( m_iCountFlag )
//		g_Dbg.Console( "Number of instructions executed = %d\n", m_StepCnt );
//	if( srINPUT != stepResult )
//		g_Dbg.Console( "%s\n", g_stepResultTab[ stepResult] );
	if( srHALT == stepResult )
	{
		g_Dbg.Console( "Simulation done.\n" );
		m_bRun = FALSE;
		return m_CurInst.iarg1; // halt type
	}
	else if( srEXESCRIPT == stepResult )
	{
		// 권한을 CMachine에게 맡긴다.
		return 3;
	}
	return -1;
}


//-----------------------------------------------------------------------------//
// doCommand
//-----------------------------------------------------------------------------//
int CProcessor::doCommand()
{
	int stepResult = srOKAY;

	m_StepCnt = 0;
	while( stepResult == srOKAY )
	{
//		m_iLoc = m_Reg[ REG_PC];
//		if( m_TraceFlag ) 
//			WriteInstruction( iloc );

		stepResult = stepTM();

		if( srINPUT == stepResult )	// 값을 입력받기위해 함수를 종료한다.
			break;

		m_StepCnt++;
	}

	if( m_iCountFlag )
		g_Dbg.Console( "Number of instructions executed = %d\n", m_StepCnt );
	if( srINPUT != stepResult )
		g_Dbg.Console( "%s\n", g_stepResultTab[ stepResult] );
	if( srHALT == stepResult )
		g_Dbg.Console( "Simulation done.\n" );

	return TRUE;
}


CProcessor::STEPRESULT CProcessor::stepTM()
{
	int pc = (int)m_Reg[ REG_PC];
	if( (pc < 0) || (pc > IADDR_SIZE) )
		return srIMEM_ERR;
	m_Reg[ REG_PC] = (int)(pc + 1);
	m_CurInst = m_iMem[ pc];

	STEPRESULT result;
	// 입력받아야될 데이타가 있다면
	if( opIN == m_CurInst.iop )
	{
		g_Dbg.Console( "Enter value for IN istruction: " );
//		m_State = SS_DATA_INPUT;
		result = srINPUT;
	}
	else
	{
		result = stepAcu( &m_CurInst );
	}

	return result;
}


CProcessor::STEPRESULT CProcessor::stepAcu( INSTRUCTION *pcurrentinst )
{
	int r,s,t,m;

	switch( (opClass(pcurrentinst->iop)) )
	{
	// opcode r,s,t
	case opclRR:
		r = pcurrentinst->iarg1;
		s = pcurrentinst->iarg2;
		t = pcurrentinst->iarg3;
		break;

	// opcode r,d(s)
	// a = d + reg[s]; dMem[ a] 참조
	case opclRM:
		r = pcurrentinst->iarg1;
		s = pcurrentinst->iarg3;
		m = pcurrentinst->iarg2 + (int)m_Reg[ s];
		if( (m < 0) || (m > DADDR_SIZE) )
			return srDMEM_ERR;
		break;

	// opcode r,s,t
	// reg[ r] = s + reg[t]
	case opclRA:
		r = pcurrentinst->iarg1;
		s = pcurrentinst->iarg3;
		m = pcurrentinst->iarg2 + (int)m_Reg[ s];
		break;
	}

	switch( pcurrentinst->iop )
	{
	case opHALT:
		g_Dbg.Console( "HALT: %1d,%1d,%1d\n", r,s,t );
		return srHALT;

	case opIN:
		g_Dbg.Console( "%d\n", m_Num );
		m_Reg[ r] = (float)m_Num;
		break;

	case opOUT:
		g_Dbg.Console( "OUT instruction prints: %1.0f\n", m_Reg[r] );
		break;

	case opADD: m_Reg[ r] = m_Reg[ s] + m_Reg[ t]; break;
	case opSUB: m_Reg[ r] = m_Reg[ s] - m_Reg[ t]; break;
	case opMUL: m_Reg[ r] = m_Reg[ s] * m_Reg[ t]; break;
	case opAND: m_Reg[ r] = m_Reg[ s] && m_Reg[ t]; break;
	case opOR:  m_Reg[ r] = m_Reg[ s] || m_Reg[ t]; break;

	case opDIV:
		if( m_Reg[ t] != 0 ) m_Reg[ r] = m_Reg[ s] / m_Reg[ t];
		else return srZERODIVIDE;
		break;
	case opREM:
		if( m_Reg[ t] != 0 ) m_Reg[ r] = (float)((int)m_Reg[ s] % (int)m_Reg[ t]);
		else return srZERODIVIDE;
		break;

	// stack은 아래에서 위로 자란다.
	case opPUSH:
		if( (m_Reg[ REG_SP] < 0) || (m_Reg[ REG_SP] > DADDR_SIZE) )
			return srDMEM_ERR;
		m_dMem[ (int)--m_Reg[ REG_SP]] = m_Reg[ r];
		break;

	case opPOP:
		if( (m_Reg[ REG_SP] < 0) || (m_Reg[ REG_SP] >= DADDR_SIZE) )
			return srDMEM_ERR;
		m_Reg[ r] = m_dMem[ (int)m_Reg[ REG_SP]++];
		break;

	case opCALL:
		if( 2 == CallFunction(r) )
			return srEXESCRIPT;
		break;

		//******************** RM instruction **************
	case opLD: m_Reg[ r] = m_dMem[ m]; break;
	case opILDF: m_Reg[ r] = *(float*)*(DWORD*)&m_dMem[ m]; break;
	case opILD: m_Reg[ r] = *(int*)*(DWORD*)&m_dMem[ m]; break;

	case opST: m_dMem[ m] = m_Reg[ r]; break;
	case opISTF: *(float*)*(DWORD*)&m_dMem[ m] = m_Reg[ r]; break;
	case opIST:	*(int*)*(DWORD*)&m_dMem[ m] = (int)m_Reg[ r]; break;

		//******************** RA instruction **************
	case opLDA: m_Reg[ r] = (float)m; break;
	case opLDC: m_Reg[ r] = (float)pcurrentinst->iarg2; break;

	case opJLT: if( m_Reg[ r] < 0 ) m_Reg[ REG_PC] = m; break;
	case opJLE: if( m_Reg[ r] <= 0 ) m_Reg[ REG_PC] = m; break;
	case opJGT: if( m_Reg[ r] > 0 ) m_Reg[ REG_PC] = m; break;
	case opJGE: if( m_Reg[ r] >= 0 ) m_Reg[ REG_PC] = m; break;
	case opJEQ: if( m_Reg[ r] == 0 ) m_Reg[ REG_PC] = m; break;
	case opJNE: if( m_Reg[ r] != 0 ) m_Reg[ REG_PC] = m; break;
	}

	return srOKAY;
}


//-----------------------------------------------------------------------------//
// CallFunction
// return value:	1 = Success
//					2 = ExecuteScript
//-----------------------------------------------------------------------------//
int CProcessor::CallFunction( int funcid )
{
	switch( funcid )
	{
	case DbgPrint:		g_Dbg.Console( "%1.0f\n", GetArgumentFloat(0) ); break;
	case DbgPrintStr:	g_Dbg.Console( "%s\n", GetArgumentStr(0) ); break;
	case ExeScript:
		g_Dbg.Console( "ExecuteScript %s\n", GetArgumentStr(0) );
		return 2;
	// 실행인자값 리턴
	case _GetArgument: SetReturnValue( m_pArgument ); break;

	default: if( m_pCallFunc ) m_pCallFunc( funcid, this ); break;
	}

	return 1;
}


// stack 내의 함수 인자를 얻는다.
// return value + stack pointer 때문에 +2 계산이 추가된다.
void* CProcessor::GetArgumentClass( int idx )
{
	int value = *(int*)&m_dMem[ (int)m_Reg[ REG_SP]+2+idx];
	return (void*)value;
}

float CProcessor::GetArgumentFloat( int idx )
{
	float value = m_dMem[ (int)m_Reg[ REG_SP]+2+idx];
	return value;
}

// stack 내의 함수 인자를 얻는다.
char* CProcessor::GetArgumentStr( int idx )
{
	// string은 pointer기능을 가진다.
	float i = m_dMem[ (int)m_Reg[ REG_SP]+2+idx];

	// 소수점부분을 가진다면 외부메모리 주소를 뜻한다.
	BOOL ext = FALSE;
	int f = (int)i;
	if( 0.f != (i - (float)f) )
		ext = TRUE;
	
	char *str = NULL;
	if( ext )	str = (char*)*(DWORD*)&i;
	else		str = (char*)&m_dMem[ (int)i];
	return str;
}


// stack의 return space에  리턴값을 저장한다.
void CProcessor::SetReturnValue( int value )
{
	 m_dMem[ (int)m_Reg[ REG_SP]] = (float)value;
}
void CProcessor::SetReturnValue( float value )
{
	 m_dMem[ (int)m_Reg[ REG_SP]] = value;
}
void CProcessor::SetReturnValue( DWORD value )
{
	 m_dMem[ (int)m_Reg[ REG_SP]] = *(float*)&value;
}
void CProcessor::SetReturnValue( void *value )
{
	DWORD d = (DWORD)value;
	float f = *(float*)&d;
	m_dMem[ (int)m_Reg[ REG_SP]] = f;
}



//-----------------------------------------------------------------------------//
// bgm 파일을 읽는다.
//-----------------------------------------------------------------------------//
BOOL CProcessor::ReadInstructions_Binary( char *pFileName )
{
	// 확장자를 bgm으로 바꾼다.
	char filename[ MAX_PATH];
	strcpy( filename, pFileName );
	strcpy( &filename[ strlen(filename)-2], "bgm" );

	FILE *fp = fopen( filename, "rb" );
	if( !fp ) return FALSE;

	ZeroMemory( m_Reg, sizeof(m_Reg) );
	ZeroMemory( m_dMem, sizeof(m_dMem) );
	m_dMem[ 0] = DADDR_SIZE;
	ZeroMemory( m_iMem, sizeof(m_iMem) );

	// string table을 얻어온다.
	// symbolsize, string table 크기를 얻는다.
	int strtabsize, symsize;
	fread( &symsize, 1, sizeof(int), fp ); // symbol table크기를 저장한다.
	fread( &strtabsize, 1, sizeof(int), fp );
	fread( &m_dMem[ symsize], 1, sizeof(int)*strtabsize, fp );

	// instruction 수 얻어온다.
	int instcnt = 0;
	fread( &instcnt, 1, sizeof(int), fp );
	fread( m_iMem, instcnt, sizeof(INSTRUCTION), fp );

	return TRUE;
}


//-----------------------------------------------------------------------------//
// ReadInstructions
//-----------------------------------------------------------------------------//
BOOL CProcessor::ReadInstructions( char *pFileName )
{
	m_pGm = fopen( pFileName, "r" );
	if( !m_pGm ) return FALSE;

	OPCODE op;
	int arg1, arg2, arg3;
	int loc, lineNo;

	ZeroMemory( m_Reg, sizeof(m_Reg) );
	ZeroMemory( m_dMem, sizeof(m_dMem) );
	m_dMem[ 0] = DADDR_SIZE;
	ZeroMemory( m_iMem, sizeof(m_iMem) );
	lineNo = 0;

	// string table을 얻어온다.
	// symbolsize, string table 크기를 얻는다.
	int strtabsize, symsize;
	fread( &symsize, 1, sizeof(int), m_pGm ); // symbol table크기를 저장한다.
	fread( &strtabsize, 1, sizeof(int), m_pGm );
	fread( &m_dMem[ symsize], 1, sizeof(int)*strtabsize, m_pGm );

	while( !feof(m_pGm) )
	{
		fgets( m_Line, LINESIZE, m_pGm );
		m_inCol = 0;
		++lineNo;
		m_lineLen = strlen( m_Line ) - 1;
		if( m_Line[ m_lineLen] == '\n' ) m_Line[ m_lineLen] = '\0';
		else m_Line[ ++m_lineLen] = '\0';
		if( (nonBlank()) && (m_Line[ m_inCol] != '*') )
		{
			if( 0 > getNum() )
				return error( "Bad location", lineNo, -1 );
			loc = m_Num; // getnum() 함수로 얻어온 값은 num변수에 저장된다.
			if( loc > IADDR_SIZE )
				return error( "Location too large", lineNo, loc );
			if( !skipCh( ':' ) )
				return error( "Missing colon", lineNo, loc );
			if( !getWord() )	// getword() 함수로 얻어온 값은 word변수에 저장된다.
				return error( "Missing opcode", lineNo, loc );

			// op code 찾기
			op = opHALT;
			int n = op;
			while( (op < opRALim) && (strncmp(g_opCodeTab[ op], m_Word, 4) != 0) )
			{
				++n;
				op = (OPCODE)n;
			}
			if( strncmp(g_opCodeTab[ op], m_Word, 4) != 0 )
				return error( "Illegal opcode", lineNo, loc );

			// op code 범주 구분 {opclRR, opclRM, opclRA}

			switch( opClass(op) )
			{
			case opclRR:
				if( (0 > getNum()) || (m_Num < 0) )
					return error( "Bad first register", lineNo, loc );
				arg1 = m_Num;
				if( !skipCh(',') )
					return error( "Missing comma", lineNo, loc );
				if( (0 > getNum()) || (m_Num < 0) || (m_Num >= NO_REGS) )
					return error( "Bad second register", lineNo, loc );
				arg2 = m_Num;
				if( !skipCh(',') )
					return error( "Missing comma", lineNo, loc );
				if( (0 > getNum()) || (m_Num < 0) || (m_Num >= NO_REGS) )
					return error( "Bad third register", lineNo, loc );
				arg3 = m_Num;
				break;

			case opclRM:
			case opclRA:
				if( (0 > getNum()) || (m_Num < 0) )
					return error( "Bad first register", lineNo, loc );
				arg1 = m_Num;
				if( !skipCh(',') )
					return error( "Missing comma", lineNo, loc );
				if( 0 > getNum() )
					return error( "Bad displacement", lineNo, loc );
				arg2 = m_Num;
				if( !skipCh('(') && !skipCh(',') )
					return error( "Missing LParen", lineNo, loc );
				if( (0 > getNum()) || (m_Num < 0) || (m_Num >= NO_REGS) )
					return error( "Bad second register", lineNo, loc );
				arg3 = m_Num;
				break;
			}
			m_iMem[ loc].iop = op;
			m_iMem[ loc].iarg1 = arg1;
			m_iMem[ loc].iarg2 = arg2;
			m_iMem[ loc].iarg3 = arg3;
		}
	}

	fclose( m_pGm );
	m_pGm = NULL;

	return TRUE;
}


int CProcessor::opClass( int c )
{
	if		( c <= opRRLim ) return ( opclRR );
	else if	( c <= opRMLim ) return ( opclRM );
	else					 return ( opclRA );
}


int CProcessor::getNum()
{
	int sign;
	int term;
	int temp = FALSE;
	m_Num = 0;
	do
	{
		sign = 1;
		while( nonBlank() && ((m_Ch == '+') || (m_Ch == '-')) )
		{
			temp = FALSE;
			if( m_Ch == '-' ) sign = -sign;
			getCh();
		}
		term = 0;
		nonBlank();
		while( isdigit(m_Ch) )
		{
			temp = TRUE;
			term = term * 10 + (m_Ch - '0' );
			getCh();
		}
		m_Num = m_Num + (term * sign);
	}
	while( (nonBlank()) && ((m_Ch == '+') || (m_Ch == '-')) );
	return temp;
}


int CProcessor::getWord()
{
	int temp = FALSE;
	int length = 0;
	if( nonBlank() )
	{
		while( isalnum(m_Ch) )
		{
			if( length < WORDSIZE-1) m_Word[ length++] = m_Ch;
			getCh();
		}
		m_Word[ length] = '\0';
		temp = (length != 0);
	}
	return temp;
}


int CProcessor::skipCh( char c )
{
	int temp = FALSE;
	if( nonBlank() && (m_Ch == c) )
	{
		getCh();
		temp = TRUE;
	}
	return temp;
}


void CProcessor::getCh()
{
	if( ++m_inCol < m_lineLen )
		m_Ch = m_Line[ m_inCol];
	else
		m_Ch = ' ';
}


int CProcessor::nonBlank()
{
	while( (m_inCol < m_lineLen) && (m_Line[ m_inCol] == ' ') )
		m_inCol++;
	if( m_inCol < m_lineLen )
	{
		m_Ch = m_Line[ m_inCol];
		return TRUE;
	}
	else
	{
		m_Ch = ' ';
		return FALSE;
	}
}


int CProcessor::error( char *msg, int lineNo, int instNo )
{
	g_Dbg.Console( "Line %d", lineNo );
	if( instNo >= 0 ) g_Dbg.Console( " (Instruction %d)", instNo );
	g_Dbg.Console( "   %s\n", msg );
	return 1;
}

