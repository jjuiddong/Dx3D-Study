//-----------------------------------------------------------------------------//
// 2008-03-29  programer: jaejung ┏(⊙д⊙)┛
// 
//-----------------------------------------------------------------------------//

#if !defined(__UICONTROL_H__)
#define __UICONTROL_H__



// 컨트롤 일반화 클래스
class CUIControl
{
public:
	CUIControl();
	virtual ~CUIControl();
protected:
	int m_Type;
	int m_Id;
	RECT m_Rect;
	BOOL m_bVisible;

public:
	virtual BOOL Create( SControl *pCtrl );
	virtual void Render() {}
	virtual void Animate( int nTimeDelta ) {}
	virtual BOOL MsgProc( SUIMsg *pMsg ) { return TRUE; }
	virtual void SetVisible( BOOL bVisible ) { m_bVisible=bVisible; }
	virtual void SetRect( RECT *pRect ) { m_Rect = *pRect; }

	void SendMessage( SUIMsg *pMsg );
	int GetId() const { return m_Id; }
	int GetType() const { return m_Type; }
	BOOL InRect( POINT *pt );
	BOOL GetVisible() const { return m_bVisible; }
	void GetRect( RECT *pRect ) { *pRect = m_Rect; }
	
};



// 이미지 출력 클래스
class CUIImage : public CUIControl
{
public:
	CUIImage();
	virtual ~CUIImage();
protected:
	IDirect3DTexture9 *m_pTex;
	int m_nIdx;					// 이미지 인덱스 (default = 0)
	SVtxRhwTex m_V[ 6];
	int m_Width;				// 이미지 총너비
	int m_Height;				// 이미지 총높이

public:
	virtual BOOL Create( SControl *pCtrl );
	virtual void Render();
	virtual void Animate( int nTimeDelta );
	virtual BOOL MsgProc( SUIMsg *pMsg );
	virtual void SetRect( RECT *pRect );

	void SetImageIndex( int nIdx );
	int GetImageIndex() const { return m_nIdx; }

};



// 버튼 클래스
class CUIButton : public CUIControl
{
public:
	CUIButton();
	virtual ~CUIButton();
protected:
	CUIImage *m_pImg;

public:
	virtual BOOL Create( SControl *pCtrl );
	virtual void Render();
	virtual void Animate( int nTimeDelta );
	virtual BOOL MsgProc( SUIMsg *pMsg );

};


#endif // __UICONTROL_H__
