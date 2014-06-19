//-----------------------------------------------------------------------------//
// 2006-08-15 programer: jaejung ┏(⊙д⊙)┛
// 
// 2008-01-05 예전에 만든것을 업그레이드 함
//-----------------------------------------------------------------------------//

#if !defined(__SCRIPT_CODEGEN_H__)
#define __SCRIPT_CODEGEN_H__


namespace ns_script
{
	typedef enum
	{
		// RR instructions
		opHALT,		// RR	halt, operands must be zero
		opIN,		// RR	read into reg(r), s and t are ignored
		opOUT,		// RR	write from reg(r), s and t are ignored
		opADD,		// RR	reg(r) = reg(s) + reg(t)
		opSUB,		// RR	reg(r) = reg(s) - reg(t)
		opMUL,		// RR	reg(r) = reg(s) * reg(t)
		opDIV,		// RR	reg(r) = reg(s) / reg(t)
		opREM,		// RR	reg(r) = reg(s) % reg(t)
		opAND,		// RR	reg(r) = reg(s) && reg(t)
		opOR,		// RR	reg(r) = reg(s) || reg(t)
		opRRLim,	// limit of RR opcodes

		// RM instructions
		opLD,		// RM	reg(r) = mem(d+reg(s))
		opILDF,		// RM	reg(r) = (float)*mem(d+reg(s))
		opILD,		// RM	reg(r) = *mem(d+reg(s))
		opST,		// RM	mem(d+reg(s)) = reg(r)
		opISTF,		// RM	*mem(d+reg(s)) = (float)reg(r)
		opIST,		// RM	*mem(d+reg(s)) = reg(r)
		opRMLim,	// limit of RM opcodes

		// RA instructions
		opLDA,		// RA	reg(r) = d+reg(s)
		opLDC,		// RA	reg(r) = d, reg(s) is ignored
		opJLT,		// RA	if reg(r)<0 then reg(7) = d+reg(s)
		opJLE,		// RA	if reg(r)<=0 then reg(7) = d+reg(s)
		opJGT,		// RA	if reg(r)>0 then reg(7) = d+reg(s)
		opJGE,		// RA	if reg(r)>=0 then reg(7) = d+reg(s)
		opJEQ,		// RA	if reg(r)==0 then reg(7) = d+reg(s)
		opJNE,		// RA	if reg(r)!=0 then reg(7) = d+reg(s)

		opPUSH,		// RA	push reg(r)
		opPOP,		// RA   pop  reg(r)
		opCALL,		// RR	Call r 

		opRALim

	} OPCODE;

	static char *g_opCodeTab[] = 
	{
		// RR opcodes
		"HALT", "IN", "OUT", "ADD", "SUB", "MUL", "DIV", "REM", "AND", "OR", "????",

		// RM opcodes
		"LD", "ILDF", "ILD", "ST", "ISTF", "IST", "????",

		// RA opcodes
		"LDA", "LDC", "JLT", "JLE", "JGT", "JGE", "JEQ", "JNE", "PUSH", "POP",  "CALL", "????"
	};


	class CCodeGen
	{
	public:
		CCodeGen():m_bComment(TRUE),m_nEmitLoc(0),m_nHighEmitLoc(0),m_EventListLoc(-1) {}
		virtual ~CCodeGen() {}
	
	protected:
		enum Register
		{
			ac = 0,		// accumulator
			ac1,		// 2nd accumulator
			ac2,		// 3nd accumulator
			bp = 4,		// base pointer
			gp,			// global pointer
						// points to bottom of memory for (global) variable storage
			sp,			// stack pointer
			pc			// programe counter
		};

		FILE *m_Fp;
		int m_nEmitLoc;
		int m_nHighEmitLoc;
		BOOL m_bComment;
		CStringTable *m_pStrTab;
		std::list<int> m_ScrWaitLoc;	// ScriptWait 명령이 호출된 위치를 저장한다.
		int m_EventListLoc;

		// Binary저장을 위해 만들어짐
		typedef struct _tagSInst
		{
			int loc;
			char op[ 8];
			int r,s,t;
			char comment[ 64];
			_tagSInst() {}
			_tagSInst( int _loc, char *_op, int _r, int _s, int _t, char *_comment ) : loc(_loc), r(_r), s(_s), t(_t)
				{ strcpy( op, _op ); if( _comment ) strcpy( comment, _comment ); }
			bool operator < ( const _tagSInst& rhs )
				{ return loc < rhs.loc; }
		} SInst;
		std::list<SInst> m_InstList;

	public:
		BOOL CodeGenerate( SParseTree *pTree, CTypeTable *pTypeTab, CStringTable *pStrTab, char *pSourceFileName );

	protected:
		BOOL CodeGen( SParseTree *pTree, CTypeTable *pTypeTab );
		BOOL GenStmt( SParseTree *pTree, CTypeTable *pTypeTab );
		BOOL GenExp( SParseTree *pTree, CTypeTable *pTypeTab );
		void GenCall( SParseTree *pTree, CTypeTable *pTypeTab );
		void GenLoadClassPointer( char *pSymbolName, CSymTable *pSym, CTypeTable *pTypeTab );
		void GenLoadGlobalVarPointer( CSymTable *pSym );
		void GenImportData( SParseTree *pTree, CTypeTable *pTypeTab );
		void EmitRO( char *op, int r, int s, int t, char *c );
		void EmitRM( char *op, int r, int d, int s, char *c );
		void EmitRM_Abs( char *op, int r, int a, char *c );
		void EmitCall( int id, char *c );
		void EmitComment( char *c );
		int EmitSkip( int howMany );
		void EmitBackup( int loc );
		void EmitRestore();
		BOOL WriteBinary( SParseTree *pTree, CStringTable *pStrTab, char *pSourceFileName );
	};
}

#endif // __SCRIPT_CODEGEN_H__
