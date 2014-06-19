
#include "stdafx.h"
//#include "global.h"
#include "script_global.h"
#include "script_parser.h"

using namespace std;
using namespace ns_script;



/////////////////////////////////////////////////////////////////////////////////
// Scanner

// lookup table of reserved words
typedef struct _SReservedWord
{
	char *str;
	Tokentype tok;
} SReservedWord;

SReservedWord reservedWords[] = 
{
	{"include", INCLUDE },
	{"define", DEFINE },
	{"event", EVENT },
	{"if", IF },
	{"else", ELSE },
	{"while", WHILE },
	{"class", CLASS },
	{"fn", FUNCTION },
	{"import", IMPORT },
	{"scriptwait", SCRWAIT },
	{"scriptend", SCREND },
	{"scriptexit", SCREXIT },

};
const int g_rlusize = sizeof(reservedWords) / sizeof(SReservedWord);

static Tokentype reservedLookup( char *s )
{
	int i=0;
	for( i=0; i < g_rlusize; ++i )
	{
		if( !_stricmp(s, reservedWords[ i].str) )
			return reservedWords[ i].tok;
	}
	return ID;
}

CScanner::CScanner()
{
	m_fp = NULL;
}

CScanner::~CScanner() 
{
	// 아직 아무일도 없음
}


BOOL CScanner::LoadFile( char *szFileName, BOOL bTrace ) // bTrace=FALSE
{
	if( m_fp ) fclose( m_fp );
	fopen_s( &m_fp, szFileName, "r" );
	if( !m_fp )
	{
		g_Dbg.Console( "[%s] 파일이 없습니다.\n", szFileName );
		return FALSE;
	}
	m_bTrace = bTrace;
	Clear();

	return TRUE;
}


Tokentype CScanner::GetToken()
{
	if( !m_fp ) return ENDFILE;

	if( m_TokQ.size() == 0 )
	{
		for( int i=0; i < MAX_QSIZE; ++i )
		{
			STokDat d;
			char buf[ MAX_TOKENLEN];
			d.tok = _GetToken( buf );
			d.str = buf;
			m_TokQ.push_back( d );
		}
	}
	else
	{
		STokDat d;
		char buf[ MAX_TOKENLEN];
		d.tok = _GetToken( buf );
		d.str = buf;
		m_TokQ.push_back( d );
		m_TokQ.pop_front();
	}
	
	return m_TokQ[ 0].tok;
}


Tokentype CScanner::GetTokenQ( int nIdx )
{
	if( !m_fp ) return ENDFILE;
	return m_TokQ[ nIdx].tok;
}


char* CScanner::GetTokenStringQ( int nIdx )
{
	if( !m_fp ) return NULL;
	return 	(char*)m_TokQ[ nIdx].str.c_str();
}


char* CScanner::CopyTokenStringQ( int nIdx )
{
	if( !m_fp ) return NULL;

	int len = m_TokQ[ nIdx].str.size();
	char *p = new char[ len + 1];
	strcpy_s( p, len+1, m_TokQ[ nIdx].str.c_str() );
	return p;
}


char CScanner::GetNextChar()
{
	if( m_nLinePos >= m_nBufSize )
	{
		++m_nLineNo;
		if( fgets(m_Buf, MAX_BUFF-1, m_fp) )
		{
			m_nLinePos = 0;
			m_nBufSize = strlen( m_Buf );
			if( m_bTrace )
				g_Dbg.Console( "%4d: %s", m_nLineNo, m_Buf );
		}
		else 
		{
			return EOF;
		}
	}

	return m_Buf[ m_nLinePos++];
}


void CScanner::UngetNextChar()
{
	--m_nLinePos;
}


Tokentype CScanner::_GetToken( char *pToken )
{
	BOOL bFloat = FALSE;
	Tokentype currentToken;
	StateType state = START;

	int nTok = 0;
	while( DONE != state )
	{
		char c = GetNextChar();
		BOOL save = TRUE;
		switch( state )
		{
		case START:
			if( isdigit(c) )
				state = INNUM;
			else if( isalpha(c) || '@' == c || '_' == c )
				state = INID;
			else if( '=' == c )
			{
				state = INEQ;
				save = FALSE;
			}
			else if( '!' == c )
			{
				state = INNEQ;
				save = FALSE;
			}
			else if( '|' == c )
			{
				state = INOR;
				save = FALSE;
			}
			else if( '&' == c )
			{
				state = INAND;
				save = FALSE;
			}
			else if( '"' == c )
			{
				state = INSTR;
				save = FALSE;
			}
			else if( '/' == c )
			{
				state = INDIV;
				save = FALSE;
			}
			else if( '$' == c )
			{
				state = INCOMMENT;
				save = FALSE;
			}
			else if( (' ' == c) || ('\t' == c) || ('\n' == c) )
				save = FALSE;
			else
			{
				state = DONE;
				switch( c )
				{
				case EOF:
					save = FALSE;
					currentToken = ENDFILE;
//					fclose( m_fp );
//					m_fp = NULL;
					break;
				case '*':
					currentToken = TIMES;
					break;
				case '+':
					currentToken = PLUS;
					break;
				case '-':
					currentToken = MINUS;
					break;
				case '%':
					currentToken = REMAINDER;
					break;
				case '<':
					currentToken = RT;
					break;
				case '>':
					currentToken = LT;
					break;
				case '(':
					currentToken = LPAREN;
					break;
				case ')':
					currentToken = RPAREN;
					break;
				case '{':
					currentToken = LBRACE;
					break;
				case '}':
					currentToken = RBRACE;
					break;
				case ',':
					currentToken = COMMA;
					break;
				case ':':
					currentToken = COLON;
					break;
				default:
					currentToken = _ERROR;
					break;
				}
			}
			break;

		case INCOMMENT:
			save = FALSE;
			if( '\n' == c ) state = START;
			break;

		case INNUM:
			if( !isdigit(c) && '.' != c )
			{
				UngetNextChar();
				save = FALSE;
				state = DONE;
				currentToken = NUM;
			}
			break;
		case INID:
			if( !isalpha(c) && !isdigit(c) && ('_' != c) && ('.' != c) && ('@' != c) )
			{
				UngetNextChar();
				save = FALSE;
				state = DONE;
				currentToken = ID;
			}
			break;
		case INDIV:
			if( '/' == c )
			{
				state = INCOMMENT;
			}
			else
			{
				UngetNextChar();
				currentToken = DIV;
				state = DONE;
			}
			save = FALSE;
			break;

		case INEQ:
			if( '=' == c )
			{
				currentToken = EQ;
			}
			else
			{
				UngetNextChar();
				currentToken = ASSIGN;
			}
			save = FALSE;
			state = DONE;
			break;
		case INNEQ:
			if( '=' == c )
			{
				currentToken = NEQ;
			}
			else
			{
				UngetNextChar();
				currentToken = _ERROR;
			}
			save = FALSE;
			state = DONE;
			break;
		case INOR:
			if( '|' == c )
			{
				currentToken = OR;
			}
			else
			{
				UngetNextChar();
				currentToken = _ERROR;
			}
			save = FALSE;
			state = DONE;
			break;
		case INAND:
			if( '&' == c )
			{
				currentToken = AND;
			}
			else
			{
				UngetNextChar();
				currentToken = _ERROR;
			}
			save = FALSE;
			state = DONE;
			break;
		case INSTR:
			if( '"' == c )
			{
				save = FALSE;
				state = DONE;
				currentToken = STRING;
			}
			break;
		case DONE:
		default:
			g_Dbg.Console( "scanner bug: state = %d", state );
			state = DONE;
			currentToken = _ERROR;
			break;
		}

		if( (save) && (nTok <= MAX_TOKENLEN) )
		{
//			if( (INNUM==state) && ('.' == c) )
//				bFloat = TRUE;

			pToken[ nTok++] = c;
		}
		if( DONE == state )
		{
			pToken[ nTok] = '\0';
			if( ID == currentToken )
				currentToken = reservedLookup( pToken );
		}
	}

	if( m_bTrace )
	{
		g_Dbg.Console( "    %d: ", m_nLineNo );
//		printToken( currentToken, pString );
	}

	return currentToken;
}


void CScanner::Clear()
{
	m_nLineNo = 0;
	m_nLinePos = 0;
	m_nBufSize = 0;
}


/////////////////////////////////////////////////////////////////////////////////
// CParser

ns_script::CParser::CParser()
{
	m_pScan = new CScanner;
	m_bTrace = FALSE;
	m_bError = FALSE;

}

ns_script::CParser::~CParser()
{
	delete m_pScan;
}


SParseTree* ns_script::CParser::Parse( char *szFileName, CPreCompiler *pPreC, BOOL bTrace ) // bTrace=FALSE
{
	if( !m_pScan->LoadFile(szFileName, bTrace) )
		return NULL;

	m_pPreCompiler = pPreC;
	strcpy_s( m_FileName, sizeof(m_FileName), szFileName );

	m_Token = m_pScan->GetToken();
	if( ENDFILE == m_Token ) return NULL;

	SParseTree *p = program();

	if( ENDFILE != m_Token )
	{
		SyntaxError( " code ends before file " );
		PrintToken( m_Token, m_pScan->GetTokenStringQ(0) );
		return NULL;
	}

	return p;
}


//	program -> include_list import_list define_list statement_list event_list
SParseTree* ns_script::CParser::program()
{
	SParseTree *t = NewStmt( ProgramK, m_pScan );
	int i=0;
	t->child[ i] = include_list();
	if( t->child[ i] ) ++i;
	t->child[ i] = import_list();
	if( t->child[ i] ) ++i;
	t->child[ i] = define_list();
	if( t->child[ i] ) ++i;
	t->child[ i] = statement_list();
	if( t->child[ i] ) ++i;
	t->child[ i] = event_list();
	if( t->child[ i] ) ++i;
	return t;
}


SParseTree* ns_script::CParser::include_list()
{
	SParseTree *t = NULL, *p = NULL;
	while( INCLUDE == m_Token )
	{
		if( NULL == t )
		{
			t = include_stmt();
			p = t;
		}
		else
		{
			t->sibling = include_stmt();
			t = t->sibling;
		}
	}
	return p;
}


SParseTree*  ns_script::CParser::include_stmt()
{
	SParseTree *t = NewStmt( IncludeK, m_pScan );
	match( INCLUDE );
	t->child[ 0] = str();

	// include 는 따로 처리된다.
	g_Include.push( t->child[ 0]->attr.name );

	return t;
}


//	import_list -> { 'import' declation }
SParseTree* ns_script::CParser::import_list()
{
	SParseTree *t = NULL, *p = NULL;
	while( IMPORT == m_Token )
	{
		match( IMPORT );

		if( NULL == t )
		{
			t = declation();
			p = t;
		}
		else
		{
			t->sibling = declation();
			t = t->sibling;
		}
	}	
	return p;
}


//	declation -> class_stmt | func_dec | var_dec
SParseTree* ns_script::CParser::declation()
{
	SParseTree *t = NULL;
	if( CLASS == m_Token )
	{
		t = class_stmt();
	}
	else if( FUNCTION == m_Token )
	{
		t = func_dec();
	}
	else if( ID == m_Token )
	{
		t = var_dec();
	}
	else
	{
		SyntaxError( "unexpected token -> " );
		PrintToken( m_Token, m_pScan->GetTokenStringQ(0) );
		g_Dbg.Console( "\n" );
	}
	return t;
}


SParseTree* ns_script::CParser::class_list()
{
	SParseTree *t = NULL, *p = NULL;
	while( CLASS == m_Token )
	{
		if( NULL == t )
		{
			t = class_stmt();
			p = t;
		}
		else
		{
			t->sibling = class_stmt();
			t = t->sibling;
		}
	}	
	return p;
}


//	class_stmt -> 'class' id '{' variable_list function_list '}'
SParseTree* ns_script::CParser::class_stmt()
{
	SParseTree *t = NewStmt( ClassK, m_pScan );

	match( CLASS );
	t->attr.name = m_pScan->CopyTokenStringQ( 0 );
	match( ID );
	match( LBRACE );
	t->child[ 0] = variable_list();
	t->child[ 1] = function_list();
	match( RBRACE );

	return t;
}


//	variable_list -> { var_dec }
SParseTree* ns_script::CParser::variable_list()
{
	SParseTree *t = NULL, *p = NULL;
	while( ID == m_Token )
	{
		if( NULL == t )
		{
			t = var_dec();
			p = t;
		}
		else
		{
			t->sibling = var_dec();
			t = t->sibling;
		}
	}	
	return p;
}


// var_dec -> type id ':' num
SParseTree* ns_script::CParser::var_dec()
{
	SParseTree *t = NewStmt( VarDecK, m_pScan );
	t->child[ 0] = type();
	t->child[ 1] = varname();
	match( COLON );
	t->child[ 2] = num();

/*
	SParseTree *t = NewStmt( VarDecK, m_pScan );

	SParseTree *c = var();
	SParseTree *p = c;
	while( COMMA == m_Token )
	{
		match( COMMA );
		p->sibling = var();
		p = p->sibling;
	}
	if( COMMA == m_Token ) // 방어코드 (변수뒤에 콤마를 넣는 실수가 많아서 추가함 {방어코드})
		match( COMMA );

	t->child[ 0] = c;
/**/
	return t;
}


//	function_list -> { func_dec }
SParseTree* ns_script::CParser::function_list()
{
	SParseTree *t = NULL, *p = NULL;
	while( FUNCTION == m_Token )
	{
		if( NULL == t )
		{
			t = func_dec();
			p = t;
		}
		else
		{
			t->sibling = func_dec();
			t = t->sibling;
		}
	}	
	return p;
}


//	func_dec -> 'fn' type id '(' param_list ')' ':' num
SParseTree* ns_script::CParser::func_dec()
{
	SParseTree *t = NewStmt( FuncDecK, m_pScan );

	match( FUNCTION );
	t->child[ 0] = type();
	t->child[ 1] = func();
	match( LPAREN );
	t->child[ 2] = param_list();
	match( RPAREN );
	match( COLON );
	t->child[ 3] = num();

	return t;
}

SParseTree* ns_script::CParser::func()
{
	SParseTree *t = NewStmt( FuncK, m_pScan );
	t->attr.name = m_pScan->CopyTokenStringQ( 0 );
	match( ID );
	return t;
}


//	param_list -> param_stmt {,param_stmt}
SParseTree* ns_script::CParser::param_list()
{
	SParseTree *t = param_stmt();
	if( !t ) return NULL;

	SParseTree *p = t;
	while( COMMA == m_Token )
	{
		match( COMMA );
		SParseTree *q = param_stmt();
		if( q )
		{
			p->sibling = q;
			p = q;
		}
	}
	return t;
}


SParseTree* ns_script::CParser::param_stmt()
{
	if( ID != m_Token ) return NULL;
	SParseTree *t = NewExp( ParamK, m_pScan );
	t->attr.name = m_pScan->CopyTokenStringQ( 0 );
	match( ID );
	return t;
}


//	statement_list -> { statement }
SParseTree* ns_script::CParser::statement_list()
{
	SParseTree *t = NULL, *p, *q;
	while( (q = statement()) )
	{
		if( !t )
		{
			t = q;
			p = q;
		}
		else
		{
			p->sibling = q;
			p = q;
		}
	}
	return t;
}


//	define_list -> { define_stmt }
SParseTree* ns_script::CParser::define_list()
{
	while( DEFINE == m_Token )
		define_stmt();
	return NULL;
}


//	define_stmt -> 'define' id num | 'define' id string
SParseTree* ns_script::CParser::define_stmt()
{
	// Scanner의 프리컴파일 테이블에 저장된다.
	match( DEFINE );
	char *pstr = m_pScan->CopyTokenStringQ( 0 );
	match( ID );
	char *pdata = m_pScan->CopyTokenStringQ( 0 );
	m_pPreCompiler->InsertDefine( pstr, m_Token, pdata );
	match( m_Token );

	return NULL;
}


//	statement -> if_stmt | while_stmt | event_stmt | assign_stmt | exp
SParseTree* ns_script::CParser::statement()
{
	SParseTree *t = NULL;
	switch( m_Token )
	{
	case IF: t = if_stmt(); break;
	case WHILE: t = while_stmt(); break;
//	case EVENT: t = event_stmt(); break;
	case SCRWAIT: t = scriptwait(); break;
	case SCREND: t = scriptend(); break;
	case SCREXIT: t = scriptexit(); break;
	case ID:
		if( LPAREN == m_pScan->GetTokenQ(1) ) t = call_stmt();
		else								  t = assign_stmt();
		break;
	default: t = exp(); break;
	}
	return t;
}


//	assign_stmt -> id = exp
SParseTree* ns_script::CParser::assign_stmt()
{
	SParseTree *t = NewStmt( AssignK, m_pScan );
	t->child[ 0] = id();
	if( ASSIGN == m_Token )
	{
		match( ASSIGN );
		t->child[ 1] = exp();
	}
	return t;
}


//	call_stmt -> func '(' arg_list ')'
SParseTree* ns_script::CParser::call_stmt()
{
	SParseTree *t = NewExp( CallK, m_pScan );
	t->child[ 0] = func();
	match( LPAREN );
	t->child[ 1] = arg_list();
	match( RPAREN );
	return t;
}


//	arg_list -> exp {, exp }
// 순서대로 sibling에 저장된다.
SParseTree* ns_script::CParser::arg_list()
{
	SParseTree *t = exp();
	if( !t ) return NULL;

	SParseTree *p = t;
	while( COMMA == m_Token )
	{
		match( COMMA );
		SParseTree *q = exp();
		if( q )
		{
			p->sibling = q;
			p = q;
		}
	}
	return t;
}


// exp --> term {addop term}
SParseTree* ns_script::CParser::exp()
{
	SParseTree *t = term();
	while( (PLUS == m_Token) || (MINUS == m_Token) )
	{
		SParseTree *op = NewExp( OpK, m_pScan );
		op->attr.op = m_Token;
		match( m_Token );
		op->child[ 0] = t;
		op->child[ 1] = term();
		t = op;
	}
	return t;
}


// term --> factor {mulop factor}
SParseTree* ns_script::CParser::term()
{
	SParseTree *t = factor();
	while( (TIMES == m_Token) || (DIV == m_Token) || (REMAINDER == m_Token) )
	{
		SParseTree *op = NewExp( OpK, m_pScan );
		op->attr.op = m_Token;
		match( m_Token );
		op->child[ 0] = t;
		op->child[ 1] = factor();
		t = op;
	}
	return t;
}

// factor --> (exp) | id | num | string | call_stmt
SParseTree* ns_script::CParser::factor()
{
	SParseTree *t = NULL;

	// (exp)
	if( LPAREN == m_Token )
	{
		match( LPAREN );
		t = exp();
		match( RPAREN );
	}
	// call_stmt
	else if( (ID == m_Token) && (LPAREN == m_pScan->GetTokenQ(1)) )
	{
		t = call_stmt();
	}
	// id
	else if( ID == m_Token )
	{
		t = id();
	}
	// num
	else if( NUM == m_Token || PLUS == m_Token || MINUS == m_Token )
	{
		t = num();
	}
	// string
	else if( STRING == m_Token )
	{
		t = str();
	}
	return t;
}


//	if_stmt -> if '(' con_exp ')' compose_stmt | 
//			   if '(' con_exp ')' compose_stmt else compose_stmt
SParseTree* ns_script::CParser::if_stmt()
{
	SParseTree *t = NewStmt( IfK, m_pScan );
	match( IF );
	match( LPAREN );
	t->child[ 0] = con_exp();
	match( RPAREN );
	t->child[ 1] = compose_stmt();
	if( ELSE == m_Token )
	{
		match( ELSE );
		t->child[ 2] = compose_stmt();
	}
	return t;
}


//	compose_stmt -> statement | '{' statement_list '}'
SParseTree* ns_script::CParser::compose_stmt()
{
	SParseTree *t;
	if( LBRACE == m_Token )
	{
		match( LBRACE );
		t = NewStmt( ComposeK, m_pScan );
		t->child[ 0] = statement_list();
		match( RBRACE );
	}
	else
	{
		t = statement();
	}
	return t;
}


//  con_exp -> con_term { and,or op con_term }
SParseTree* ns_script::CParser::con_exp()
{
	SParseTree *t = con_term();
	while( (AND == m_Token) || (OR == m_Token) )
	{
		SParseTree *op = NewExp( ConditionOpK, m_pScan );
		op->attr.op = m_Token;
		match( m_Token );
		op->child[ 0] = t;
		op->child[ 1] = con_term();
		t = op;
	}
	return t;
}


//  con_term -> con_factor { lt,rt,eq,neq op con_term }
SParseTree* ns_script::CParser::con_term()
{
	SParseTree *t = con_factor();
	while( (LT == m_Token) || (RT == m_Token) || (EQ == m_Token) || (NEQ == m_Token) )		   
	{
		SParseTree *op = NewExp( ConditionOpK, m_pScan );
		op->attr.op = m_Token;
		match( m_Token );
		op->child[ 0] = t;
		op->child[ 1] = con_term();
		t = op;
	}
	return t;
}


//  con_factor -> (con_exp) | exp
SParseTree* ns_script::CParser::con_factor()
{
	SParseTree *t = NULL;

	// (exp)
	if( LPAREN == m_Token )
	{
		match( LPAREN );
		t = con_exp();
		match( RPAREN );
	}
	// exp
	else 
	{
		t = exp();
	}
	return t;
}


//	while_stmt -> while '(' con_exp ')' compose_stmt
SParseTree* ns_script::CParser::while_stmt()
{
	SParseTree *t = NewStmt( WhileK, m_pScan );
	match( WHILE );
	match( LPAREN );
	t->child[ 0] = con_exp();
	match( RPAREN );
	t->child[ 1] = compose_stmt();
	return t;
}


//	event_list -> { event_stmt }
SParseTree* ns_script::CParser::event_list()
{
	SParseTree *t = NULL, *p = NULL;
	while( EVENT == m_Token )
	{
		if( NULL == t )
		{
			t = event_stmt();
			p = t;
		}
		else
		{
			t->sibling = event_stmt();
			t = t->sibling;
		}
	}
	return p;
}


//	event_stmt -> event '(' con_exp ')' compose_stmt
SParseTree* ns_script::CParser::event_stmt()
{
	SParseTree *t = NewStmt( EventK, m_pScan );
	match( EVENT );
	match( LPAREN );
	t->child[ 0] = con_exp();
	match( RPAREN );
	t->child[ 1] = compose_stmt();
	return t;
}


SParseTree* ns_script::CParser::scriptwait()
{
	SParseTree *t = NewStmt( ScriptWaitK, m_pScan );
	match( SCRWAIT );
	return t;
}

SParseTree* ns_script::CParser::scriptend()
{
	SParseTree *t = NewStmt( ScriptEndK, m_pScan );
	match( SCREND );
	return t;
}

SParseTree* ns_script::CParser::scriptexit()
{
	SParseTree *t = NewStmt( ScriptExitK, m_pScan );
	match( SCREXIT );
	return t;
}


SParseTree* ns_script::CParser::str()
{
	SParseTree *t = NewExp( ConstStrK, m_pScan );
	t->attr.name = m_pScan->CopyTokenStringQ( 0 );
	match( STRING );
	return t;
}


// 모든 숫자는 float으로 처리된다.
SParseTree* ns_script::CParser::num()
{
	SParseTree *t = NewExp( ConstFloatK, m_pScan );
	
	BOOL bminus = FALSE;
	if( PLUS == m_Token )
	{
		match( PLUS );
	}
	else if( MINUS == m_Token )
	{
		bminus = TRUE;
		match( MINUS );
	}
	else if( NUM == m_Token )
	{
	}

	t->attr.fnum = (float)atof( m_pScan->GetTokenStringQ(0) );
	if( bminus ) t->attr.fnum = -t->attr.fnum;
	match( NUM );

	return t;
}


SParseTree* ns_script::CParser::id()
{
	SParseTree *t = NewExp( IdK, m_pScan );
	t->attr.name = m_pScan->CopyTokenStringQ( 0 );
	match( ID );
	return t;
}


SParseTree* ns_script::CParser::varname()
{
	SParseTree *t = NewExp( VarNameK, m_pScan );
	t->attr.name = m_pScan->CopyTokenStringQ( 0 );
	match( ID );
	return t;
}

//	type -> int | float | string | class
SParseTree* ns_script::CParser::type()
{
	SParseTree *t = NewExp( TypeK, m_pScan );
	t->attr.name = m_pScan->CopyTokenStringQ( 0 );
	match( ID );

	return t;
}


BOOL ns_script::CParser::match( ns_script::Tokentype t )
{
	if( m_Token == t )
	{
		m_Token = m_pScan->GetToken();
	}
	else
	{
		SyntaxError( "unexpected token -> " );
		PrintToken( m_Token, m_pScan->GetTokenStringQ(0) );
		g_Dbg.Console( "\n" );
	}
	return TRUE;
}


void ns_script::CParser::SyntaxError( char *szMsg, ... )
{
	m_bError = TRUE;
	char buf[ 256];
	va_list marker;
	va_start(marker, szMsg);
	vsprintf_s(buf, sizeof(buf), szMsg, marker);
	va_end(marker);
	g_Dbg.Console( ">>>" );
	g_Dbg.Console( "Syntax error at line %s %d: %s", m_FileName, m_pScan->GetLineNo(), buf );
}
