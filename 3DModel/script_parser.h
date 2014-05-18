//-----------------------------------------------------------------------------//
// 2006-08-15 programer: jaejung ┏(⊙д⊙)┛
// 
// 2008-01-02 예전에 만든거 업그레이드 됨
// Parser, Scanner 클래스를 한곳에 모았다, Scanner는 예전 것과 같고 
// 새BNF 문법에 맞게 Parser를 수정했다.
//-----------------------------------------------------------------------------//

#if !defined(__SCRIPT_PARSER_H__)
#define __SCRIPT_PARSER_H__


namespace ns_script
{

	// SourceCode 를 읽어서 토근단위로 분리한다.
	class CScanner
	{
	public:
		CScanner();
		virtual ~CScanner();

	protected:
		enum { MAX_QSIZE=4, MAX_BUFF=128, MAX_TOKENLEN=64 };
		enum StateType { START, INASSIGN, INCOMMENT, INNUM, INID, INSTR, INDIV, INEQ, INNEQ, INOR, INAND, DONE };

		typedef struct _tagSTokDat
		{
			std::string str;
			ns_script::Tokentype tok;
		} STokDat;

		FILE *m_fp;
		int m_nLineNo;
		int m_nLinePos;
		int m_nBufSize;
		char m_Buf[ MAX_BUFF];
		std::deque<STokDat> m_TokQ;
		BOOL m_bTrace;

	public:
		BOOL LoadFile( char *szFileName, BOOL bTrace=FALSE );
		ns_script::Tokentype GetToken();
		ns_script::Tokentype GetTokenQ( int nIdx );
		char* GetTokenStringQ( int nIdx );
		char* CopyTokenStringQ( int nIdx );
		int GetLineNo() { return m_nLineNo; }
		void Clear();

	protected:
		char GetNextChar();
		void UngetNextChar();
		ns_script::Tokentype _GetToken( char *pToken );

	};



	// 스크립트 BNF
	//
	//	program -> include_list import_list define_list statement_list event_list
	//
	//	include_list -> { include_stmt }
	//	include_stmt -> 'include' string
	//	
	//	import_list -> { 'import' declation }
	//	declation -> class_stmt | func_dec | var_dec
	//
	//	class_list -> { class_stmt }
	//	class_stmt -> 'class' id '{' variable_list function_list '}'
	//	variable_list -> { var_declist }
	//	var_declist -> var_dec {, var_dec }
	//	var_dec -> type id ':' num
	//	function_list -> { func_dec }
	//	func_dec -> 'fn' type func '(' param_list ')' ':' num
	//	func -> alpha
	//	param_list -> param_stmt {,param_stmt}
	//	param_stmt -> alpha
	//
	//	define_list -> { define_stmt }
	//	define_stmt -> 'define' id num | 'define' id string
	//
	//	statement_list -> { statement }
	//	statement -> { if_stmt | while_stmt | assign_stmt | scriptwait | scriptend | scriptexit | exp }
	//	assign_stmt -> id = exp
	//	call_stmt -> func '(' arg_list ')'
	//	arg_list -> exp {,exp}
	//	exp -> term {addop term}
	//	term -> factor {mulop factor}
	//	factor -> (exp) | id | num | string | call_stmt
	//
	//	if_stmt -> if '(' con_exp ')' compose_stmt | 
	//			   if '(' con_exp ')' compose_stmt else compose_stmt

	//  con_exp -> con_term { and,or op con_term }
	//  con_term -> con_factor { lt,rt,eq,neq op con_factor }
	//  con_factor -> (con_exp) | exp

	//	compose_stmt -> statement | '{' statement_list '}'
	//	conditionop -> < | >= | < | <= | && | '||' | == | !=
	//	while_stmt -> while '(' con_exp ')' compose_stmt
	//	scriptwait -> 'scriptwait'
	//	scriptend -> 'scriptend'
	//	scriptexit -> 'scriptexit'
	//
	//	event_list -> { event_stmt }
	//	event_stmt -> event '(' con_exp ')' compose_stmt
	//
	//	id -> alphabet{.alphabet}
	//	string -> '"'alphabet'"'
	//	type -> int | float | string | class
	//	alpha -> alphabet
	//  num -> num | +num | -num

	// SourceCode를 분석해서 트리를 만든다.
	class CPreCompiler;
	class CParser
	{
	public:
		CParser();
		virtual ~CParser();

	protected:
		CScanner *m_pScan;
		CPreCompiler *m_pPreCompiler;
		char m_FileName[ MAX_PATH];
		ns_script::Tokentype m_Token;
		BOOL m_bTrace;
		BOOL m_bError;

	public:
		SParseTree* Parse( char *szFileName, CPreCompiler *pPreC, BOOL bTrace=FALSE );
		BOOL IsError() { return m_bError; }

	protected:
		void SyntaxError( char *szMsg, ... );
		BOOL match( ns_script::Tokentype t );
		void PreCompiler( SParseTree *pTree );

		SParseTree* program();
		SParseTree* include_list();
		SParseTree* include_stmt();

		SParseTree* import_list();
		SParseTree* declation();

		SParseTree* class_list();
		SParseTree* class_stmt();
		SParseTree* variable_list();
		SParseTree* var_dec();
		SParseTree* function_list();
		SParseTree* func_dec();
		SParseTree* func();
		SParseTree* param_list();
		SParseTree* param_stmt();

		SParseTree* define_list();
		SParseTree* define_stmt();

		SParseTree* statement_list();
		SParseTree* statement();
		SParseTree* assign_stmt();
		SParseTree* call_stmt();
		SParseTree* arg_list();
		SParseTree* exp_list();
		SParseTree* if_stmt();
		SParseTree* compose_stmt();
		SParseTree* while_stmt();
		SParseTree* scriptwait();
		SParseTree* scriptend();
		SParseTree* scriptexit();
		SParseTree* exp();
		SParseTree* term();
		SParseTree* factor();
		SParseTree* con_exp();
		SParseTree* con_term();
		SParseTree* con_factor();

		SParseTree* event_list();
		SParseTree* event_stmt();

		SParseTree* id();
		SParseTree* varname();
		SParseTree* type();
		SParseTree* str();
		SParseTree* num();

	};

}

#endif // __SCRIPT_PARSER_H__
