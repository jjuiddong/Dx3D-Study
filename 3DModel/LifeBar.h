//-----------------------------------------------------------------------------//
// 2008-04-11  programer: jaejung ┏(⊙д⊙)┛
// 
// 주인공, 적들의 HP,SP 를 출력하는 클래스다.
//-----------------------------------------------------------------------------//

#if !defined(__LIFEBAR_H__)
#define __LIFEBAR_H__


class CCharacter;
class CLifeBar
{
public:
	CLifeBar();
	virtual ~CLifeBar();

protected:
	CHARACTER_TYPE m_Type;
	CCharacter *m_pCharacter;
	int m_Hp;
	int m_Sp;
	int m_MaxHp;
	int m_MaxSp;
	SVtxRhwDiff m_HPBar[ 6];
	SVtxRhwDiff m_SPBar[ 6];

public:
	BOOL Create( CCharacter *pChar, int MaxHp, int MaxSp, CHARACTER_TYPE Type );
	void Render();
	void Animate( int nDelta );
	void SetHp( int Hp );
	void SetSp( int Sp );
	int GetHp();
	int GetSp();

protected:
	void CreateBar( SVtxRhwDiff Bar[ 6], int X, int Y, int W, int H, DWORD Color );
	void MoveBar( SVtxRhwDiff Bar[ 6], int X, int Y );
	void AdjustBar( SVtxRhwDiff Bar[ 6], int W );

};

#endif // __LIFEBAR_H__
