
#include "stdafx.h"

#include "unit.h"
#include "dbg.h"
#include "state.h"


CStateMachine* CState::GetStateMachine() 
{ 
	return m_pUnit->GetStateMachine();
}

