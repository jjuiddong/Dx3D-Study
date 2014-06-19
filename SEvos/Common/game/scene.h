//-----------------------------------------------------------------------------//
// 2008-03-25  programer: jaejung ┏(⊙д⊙)┛
// 
// UserInterface에서 씬을 구현한 클래스다.
//-----------------------------------------------------------------------------//

#if !defined(__SCENE_H__)
#define __SCENE_H__

// User Interface Type
enum UITYPE
{
	UI_BUTTON,
	UI_IMAGE,
};

enum UICMD_TYPE
{
	UICMD_SETIMAGEINDEX,	// CUIImage::SetImageIndex
};


// UI 메세지
enum UIMSG_TYPE
{
	// Scene에서 처리된다.
	UIMSG_COMMAND,		// 컨트롤에 관련된 메세지
						// wparam: UICMD_TYPE
						// lparam: argument

	UIMSG_SWAPPED,		// 스왑된 씬에게 메세지를 보낸다.


	// Control에서 처리
	UIMSG_LCLICK,		// 마우스를 클릭했을때 Control에게 메세지를 보낸다.
	UIMSG_LCLICKUP,		// 클릭된 상태에서 Up되었을때 메세지를 보낸다.
	UIMSG_SETVISIBLE,	// Visible 상태 설정
						// wparam: visible값

	// Image에서 처리
	UIMSG_SETINDEX,
						// wparam: ImageIndex


};


// 씬클래스에서 사용되는 메세지 형태
typedef struct _tagSUIMsg
{
	int classid;
	int msg;
	int wparam;
	int lparam;
	_tagSUIMsg() {}
	_tagSUIMsg( int c, int m, int w, int l ):classid(c), msg(m), wparam(w), lparam(l) {}

} SUIMsg;


// 씬 클래스
class CUIControl;
class CScene
{
public:
	CScene( int Id );
	virtual ~CScene();

protected:
	int m_Id;
	std::queue< SUIMsg > m_MsgQ;
	std::list< CScene* > m_ChildScene;
	std::list< CUIControl* > m_Control;
	CScene *m_pCurScene;
	CScene *m_pParent;

	CUIControl *m_pFocusCtrl;

public:
	BOOL Create( char *pUIFileName, CScene *pParent=NULL );
	BOOL Create( SScene *pScene, CScene *pParent=NULL );
	virtual void Render();
	virtual void Animate( int nDelta );
	virtual void MessageProc( MSG *pMsg );
	virtual void KeyDown( int Key );
	virtual void ButtonDown( int ButtonId );
	virtual void ButtonUp( int ButtonId );
	virtual void AddControl( SControl *pCtrl );
	virtual void Destroy();

	int GetID() const { return m_Id; }
	void SendMessage( SUIMsg *pMsg );
	void PostMessage( SUIMsg *pMsg );
	void SwapScene( int CurSceneId, int NextSceneId );
	CScene* FindClass( int Id );
	CUIControl* FindUIControl( int Id );

protected:
	BOOL UIMsgProc();
	virtual BOOL DefaultMsgProc( SUIMsg *pMsg );
	BOOL AddChildScene( CScene *pScene );
	void RegisterScene( CScene *pScene );

};

// 매크로
// 외부에서 씬클래스를 생성할수있게 하기위한 매크로
#define SCENE_BEGIN	static CScene* g_SceneTable[] = {
#define SCENE_CLASS( name, id )	{ new name(id) }, 
#define SCENE_END	};

#endif // __SCENE_H__
