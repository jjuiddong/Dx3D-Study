
#ifndef __TERRAINCURSOR_H__
#define __TERRAINCURSOR_H__


class CTerrain;
class CTerrainCursor
{
public:
	CTerrainCursor();
	CTerrainCursor(CTerrain *pterrain);
	virtual ~CTerrainCursor();

protected:
	enum 
	{ 
		ERASE=0x01,		// 지우개 상태
		MATCH=0x20,		// 같은 텍스쳐를 찾아서 그리기 상태
	};

	BOOL m_Load;					// 타일 텍스쳐가 설정되었다면 TRUE
	BOOL m_IsTerrainUp;				// TRUE이면 지형 높이를 높이고, 아니면 낮춘다.
	DWORD m_Flag;					// ERASE, MATCH etc..
	float m_OutterRadius;			// 브러쉬 원의 반지름 (alpha값 적용)	(0 ~ 1) 최대 청크 크기까지
	float m_InnerRadius;			// 브러쉬 중심 반지름 (alpha=255 적용)	(0 ~ 1)
	float m_OutterAlpha;			// 큰 원의 알파값 (0 ~ 1)
	float m_InnerAlpha;				// 작은원(중심원)의 알파값 (0 ~ 1)
	float m_Offset;					// 지형 높이 편집값 (offset값 만큼 값을 바꾼다) (초당 높이가 바뀌는 속도)
	char m_TextureName[ MAX_PATH];	// 브러쉬로 사용되는 텍스쳐(타일) 파일이름

	POINT m_ScreenPos;				// 스크린상의 위치
	Vector3 m_CursorPos;			// Terrain 에서의 위치

	CTerrain *m_pTerrain;
	CMesh m_InnerCursor;
	CMesh m_OutterCursor;

public:
	BOOL Init();
	void Render();
	void Update( int nElapseTick );
	void UpdateCursor(POINT mousePos);
	void UpdateCursor();
	void Clear();

	BOOL IsLoad() const { return m_Load; }
	void SetTerrain(CTerrain *pterrain) { m_pTerrain = pterrain; }
	void SetEraseMode(BOOL isErase);
	void SetMatchMode(BOOL isMatch);
	void SetOutterRadius( float outterradius );
	void SetInnerRadius( float innerradius );
	void SetOutterAlpha( float outteralpha);
	void SetInnerAlpha( float innerralpha);
	void SetHeightIncrementOffset( float offset );
	void SetTextureName( char *texturename );
	void SetTerrainUp(BOOL isUp) { m_IsTerrainUp = isUp; }

	float GetOutterRadius() { return m_OutterRadius; }
	float GetInnerRadius() { return m_InnerRadius; }
	float GetOutterAlpha() { return m_OutterAlpha; }
	float GetInnerAlpha() { return m_InnerAlpha; }
	BOOL IsEraseMode() { return m_Flag & ERASE; }
	BOOL IsMatchMode() { return m_Flag & MATCH; }
	float GetHeightIncrementOffset() { return (m_IsTerrainUp)? m_Offset : -m_Offset; }
	char* GetTextureName() { return m_TextureName; }

	POINT GetScreenPos() { return m_ScreenPos; }	// screen 좌표를 리턴한다. (2차원)
	Vector3 GetCursorPos() { return m_CursorPos; }	// map 상에서의 위치를 리턴한다. (3차원)

protected:
	void _UpdateCursor(Vector3 cursorPos);

};

#endif
