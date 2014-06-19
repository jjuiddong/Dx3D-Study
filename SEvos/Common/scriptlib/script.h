//-----------------------------------------------------------------------------//
// 2008-01-03  programer: jaejung ┏(⊙д⊙)┛
// 
// 2008-02-27
//		define 기능 추가
//-----------------------------------------------------------------------------//

#if !defined(__SCRIPT_H__)
#define __SCRIPT_H__

#include "script_global.h"

namespace ns_script
{
	class CMachine;
	class CProcessor;

	class CScript
	{
	public:
		CScript();
		virtual ~CScript();
	protected:
		CMachine *m_pMachine;
		int m_IncTime;			// 일정 프레임간격으로 스크립트를 실행하기 하기위해 필요함
	public:
		BOOL Execute( char *pFileName, void func (int,ns_script::CProcessor*), char *pArgument=NULL );
		int Run( int nDelta );
	protected:
		BOOL Compile( char *pFileName, void func (int,ns_script::CProcessor*),  
			ns_script::CTypeTable *pType=NULL, ns_script::CPreCompiler *pPreC=NULL );
	};
}

#endif // __SCRIPT_H__
