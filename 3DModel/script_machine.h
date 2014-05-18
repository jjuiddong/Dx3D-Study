//-----------------------------------------------------------------------------//
// 2006-08-15  programer: jaejung ┏(⊙д⊙)┛
// 
// 2008-01-05 예전에 만든것을 업그레이드 함
//			  CMachine, CProcessor 추가됨
//-----------------------------------------------------------------------------//

#if !defined(__SCRIPT_MACHINE_H__)
#define __SCRIPT_MACHINE_H__


namespace ns_script
{
	class CProcessor;

	// 병렬처리 가상머신이다.
	// CProcessor를 생성해서 gm파일을 실행한다. 
	// CProcessor내에서 다시 스크립트를 실행될 수 있기 때문에 Stack에 쌓아서
	// CProcessor를 실행시킨다. (함수호출과 같은 방식이다.)
	// 하나 이상의 Stack을 관리하면서 여러개의 Processor를 실행될수 있도록 했다.
	class CMachine
	{
	public:
		CMachine();
		virtual ~CMachine();
	protected:
		typedef std::stack<CProcessor*>	ProcStack;
		typedef std::list<ProcStack*>	ProcList;
		ProcList m_ProcList;
	public:
		BOOL ExecuteScript( char *pFileName, void callback (int,ns_script::CProcessor*), char *pArgument );
		int Run();
		void Clear();
	protected:
		BOOL ExecuteScriptChild( ProcStack *pStack, char *pFileName, void callback (int,ns_script::CProcessor*), char *pArgument );
	};


	// gm파일을 읽어서 Instruction을 처리한다.
	class CProcessor
	{
	public:
		CProcessor();
		virtual ~CProcessor();

		enum { IADDR_SIZE = 2048, DADDR_SIZE = 1024, NO_REGS = 8, LINESIZE=128, WORDSIZE=20 };
		enum STEPRESULT { srOKAY, srINPUT, srEXESCRIPT, srHALT, srIMEM_ERR, srDMEM_ERR, srZERODIVIDE, };
		enum OPCLASS
		{
			opclRR,		// reg operand r,s,t
			opclRM,		// reg r, mem d+s
			opclRA		// reg r, int d+s
		};

		typedef struct
		{
			int iop;
			int iarg1;
			int iarg2;
			int iarg3;

		} INSTRUCTION;

		// CCodeGen클래스와 동일함
		enum Register
		{
			REG_AC = 0,		// accumulator
			REG_AC1,		// 2nd accumulator
			REG_AC2,		// 3nd accumulator
			REG_BP = 4,		// base pointer
			REG_GP,			// global pointer
							// points to bottom of memory for (global) variable storage
			REG_SP,			// stack pointer
			REG_PC			// programe counter
		};

		typedef void CALLFUNC (int,CProcessor*);

		CMachine *m_pMachine;
		BOOL m_bRun;						// Processor가 실행중이라면 TRUE
		int m_nNumberOfExecuteInst;			// Run() 함수에서 실행될 instruction갯수
		INSTRUCTION m_iMem[ IADDR_SIZE];	// Instruction 메모리
		INSTRUCTION m_CurInst;				// 현재 Instruction
		float m_dMem[ DADDR_SIZE];			// Memory / stack
		float m_Reg[ NO_REGS];				// Register
		int m_StepCnt;						// 실행된 Instruction수
		CALLFUNC *m_pCallFunc;				// CallBack 함수
		int m_TraceFlag;					// Debug용
		int m_iCountFlag;					// Debug용

		// 
		char m_pgmName[ 20];	// filename
		char m_pArgument[ 64];	// 실행 인자값
		FILE *m_pGm;			// code file pointer
		char m_Line[ LINESIZE]; // 읽어들인 라인 스트링
		int m_lineLen;			// 읽어들이는 라인의 길이
		int m_inCol;			// 읽어들이는 한라인의 index 값이다.
		int m_Num;				// getnum() 함수의 결과값으로 사용함
		char m_Word[ WORDSIZE];	// getword() 함수의 결과값으로 사용함
		char m_Ch;				// getch() 함수의 결과값으로 사용함
		int m_Done;

	public:
		BOOL ExecuteScript( CMachine *pMachine, char *pFileName, void callback (int,ns_script::CProcessor*), char *pArgument );
		int Run();
		void* GetArgumentClass( int idx );
		float GetArgumentFloat( int idx );
		char* GetArgumentStr( int idx );
		void SetReturnValue( DWORD value );
		void SetReturnValue( int value );
		void SetReturnValue( float value );
		void SetReturnValue( void *value );
		void SetNumberOfExecuteInst( int num ) { m_nNumberOfExecuteInst = num; }
		void Clear();

	protected:
		BOOL ReadInstructions( char *pFileName );
		BOOL ReadInstructions_Binary( char *pFileName );
		int doCommand();
		STEPRESULT stepTM();
		STEPRESULT stepAcu( INSTRUCTION *pcurrentinst );
		void getCh();
		int nonBlank();
		int getNum();
		int getWord();
		int skipCh( char c );
		int opClass( int c );
		int error( char *msg, int lineNo, int instNo );
		int CallFunction( int funcid );
	};

};

#endif // __SCRIPT_MACHINE_H__
