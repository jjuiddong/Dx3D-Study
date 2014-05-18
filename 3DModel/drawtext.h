//-----------------------------------------------------------------------------//
// 2008-03-13  programer: jaejung ┏(⊙д⊙)┛
// 
//-----------------------------------------------------------------------------//

#if !defined(__DRAWTEXT_H__)
#define __DRAWTEXT_H__


class CDrawText
{
public:
	CDrawText();
	virtual ~CDrawText();

protected:
	typedef struct _tagSText
	{
		int x, y;
		IDirect3DTexture9 *ptex;
		SVtxDiff v[ 6];

//		SVector3
	} SText;
	typedef std::map< int, SText* > TextMap;	// key = hiword(x)+loword(y)

	LPD3DXFONT m_pDxFont;
	TextMap m_Textmap;				// 출력될 텍스트를 저장한다. x,y위치값이 키다.
	std::list< SText* > m_Garbage;	// 출력이 끝난후 버리는 텍스트를 저장해서 메모리를 돌려쓴다.

public:
	BOOL Create();

	BOOL CreateFont( int FontId, char *pFontName, int Size );
	void DrawText( int X, int Y, char *pString, int FontId=0 );
	void Render();
	void Clear();

protected:
	void SetText( int FontId, SText *pTextData, char *pString );

};

#endif // __DRAWFONT_H__
