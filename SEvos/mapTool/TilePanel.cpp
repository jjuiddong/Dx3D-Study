// TilePanel.cpp : implementation file
//

#include "stdafx.h"
#include "maptool2.h"

#include "common/utility.h"
#include "mapview.h"
#include "terrain/terrainCursor.h"
#include "terrain/terraineditor.h"
#include "common/utility.h"

#include "TilePanel.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTilePanel dialog


CTilePanel::CTilePanel(CWnd* pParent /*=NULL*/)
	: CPanelBase(TILE, CTilePanel::IDD, pParent), 
	m_pCurrentChunk(NULL),
	m_bDrag(FALSE)
{
	//{{AFX_DATA_INIT(CTilePanel)
	m_strBrushCenter = _T("");
	m_strBrushSize = _T("");
	m_strBrushSpeed = _T("");
	m_strChunkPos = _T("");
	m_strMaxLayerCount = _T("");
	m_strTileDirPath = _T("");
	m_chkEditTileMode = FALSE;
	m_strCenterAlpha = _T("");
	m_strEdgeAlpha = _T("");
	m_chkErase = FALSE;
	m_chkMatchTexture = FALSE;
	//}}AFX_DATA_INIT
}


void CTilePanel::DoDataExchange(CDataExchange* pDX)
{
	CPanelBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTilePanel)
	DDX_Control(pDX, IDC_LIST_LAYER, m_LayerList);
	DDX_Control(pDX, IDC_SLIDER_BRUSHSPEED, m_BrushSpeed);
	DDX_Control(pDX, IDC_SLIDER_BRUSHSIZE, m_BrushSize);
	DDX_Control(pDX, IDC_SLIDER_BRUSHCENTER, m_BrushCenter);
	DDX_Control(pDX, IDC_LIST_TILE, m_TileList);
	DDX_Text(pDX, IDC_STATIC_BRUSHCENTER, m_strBrushCenter);
	DDX_Text(pDX, IDC_STATIC_BRUSHSIZE, m_strBrushSize);
	DDX_Text(pDX, IDC_STATIC_BRUSHSPEED, m_strBrushSpeed);
	DDX_Text(pDX, IDC_STATIC_CHUNKPOS, m_strChunkPos);
	DDX_Text(pDX, IDC_STATIC_MAXLAYERCOUNT, m_strMaxLayerCount);
	DDX_Text(pDX, IDC_STATIC_TILE_DIRPATH, m_strTileDirPath);
	DDX_Check(pDX, IDC_CHECK_TILEEDIT, m_chkEditTileMode);
	DDX_Text(pDX, IDC_EDIT_CENTER_ALPHA, m_strCenterAlpha);
	DDX_Text(pDX, IDC_EDIT_EDGE_ALPHA, m_strEdgeAlpha);
	DDX_Check(pDX, IDC_CHECK_ERASEBRUSH, m_chkErase);
	DDX_Check(pDX, IDC_CHECK_MATCH_TEXTURE, m_chkMatchTexture);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTilePanel, CPanelBase)
	//{{AFX_MSG_MAP(CTilePanel)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, OnButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_DELETELAYER, OnButtonDeletelayer)
	ON_LBN_SELCHANGE(IDC_LIST_TILE, OnSelchangeListTile)
	ON_BN_CLICKED(IDC_CHECK_TILEEDIT, OnCheckTileedit)
	ON_EN_CHANGE(IDC_EDIT_CENTER_ALPHA, OnChangeEditCenterAlpha)
	ON_EN_CHANGE(IDC_EDIT_EDGE_ALPHA, OnChangeEditEdgeAlpha)
	ON_BN_CLICKED(IDC_BUTTON_SHOWLAYER, OnButtonShowlayer)
	ON_BN_CLICKED(IDC_BUTTON_EDITLAYER, OnButtonEditlayer)
	ON_BN_CLICKED(IDC_BUTTON_HIDELAYER, OnButtonHidelayer)
	ON_BN_CLICKED(IDC_BUTTON_LOCKLAYER, OnButtonLocklayer)
	ON_BN_CLICKED(IDC_CHECK_ERASEBRUSH, OnCheckErasebrush)
	ON_BN_CLICKED(IDC_CHECK_MATCH_TEXTURE, OnCheckMatchTexture)
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_BRUSHSIZE, &CTilePanel::OnNMCustomdrawSliderBrushsize)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_BRUSHCENTER, &CTilePanel::OnNMCustomdrawSliderBrushcenter)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_BRUSHSPEED, &CTilePanel::OnNMCustomdrawSliderBrushspeed)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTilePanel message handlers

void CTilePanel::OnButtonRefresh() 
{
	RefreshTileList( (char*)g_szTilePath );
}

BOOL CTilePanel::OnInitDialog() 
{
	CPanelBase::OnInitDialog();
	GetClientRect( m_PannelViewPort );
	
	m_strTileDirPath = CString("Directory: ") + g_szTilePath;
	RefreshTileList( (char*)g_szTilePath );

	// Slider
	m_BrushCenter.SetRange( 1, 100 );
	m_BrushSize.SetRange( 1, 100 );
	m_BrushSpeed.SetRange( 1, 100 );

	CTerrainCursor *pcursor = g_pTerrainView->GetTerrainCursor();
	if (!pcursor) return FALSE;

	m_BrushCenter.SetPos( (int)pcursor->GetInnerRadius() );
	m_BrushSize.SetPos( (int)pcursor->GetOutterRadius() );
	m_BrushSpeed.SetPos( (int)pcursor->GetHeightIncrementOffset() );
	
	m_strBrushCenter.Format( "%d", m_BrushCenter.GetPos() );
	m_strBrushSize.Format( "%d", m_BrushSize.GetPos() );
	m_strBrushSpeed.Format( "%d", m_BrushSpeed.GetPos() );

	m_strCenterAlpha.Format( "%1.1f", pcursor->GetOutterAlpha() );
	m_strEdgeAlpha.Format( "%1.1f", pcursor->GetInnerAlpha() );

	m_chkErase = pcursor->IsEraseMode();
	m_chkMatchTexture = pcursor->IsMatchMode();

	m_strMaxLayerCount.Format( "max layer count: %d", CChunk::MAX_LAYER_CNT );
	m_strChunkPos.Format( "chunk pos: {x, y}" );

	m_LayerList.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	UpdateData( FALSE );

	return TRUE;
}


//-----------------------------------------------------------------------------//
// 타일파일리스트를 업데이트 한다.
//-----------------------------------------------------------------------------//
void CTilePanel::RefreshTileList( char *szDirectory )
{
	list<string> extlist, filelist;
	extlist.push_back( "bmp" );
	extlist.push_back( "tga" );
	extlist.push_back( "jpg" );
	extlist.push_back( "tng" );

	CollectFile( &extlist, szDirectory, &filelist );

	while( LB_ERR != m_TileList.DeleteString(0) ) {} // List Clear

	list<string>::iterator it = filelist.begin();
	while( filelist.end() != it )
		m_TileList.AddString( it++->c_str() );
}

void CTilePanel::OnSelchangeListTile() 
{
	CString filename;
	m_TileList.GetText( m_TileList.GetCurSel(), filename );

	CTerrainCursor *pcursor = g_pTerrainView->GetTerrainCursor();
	if (!pcursor) return;

	CString fullFileName = g_szTilePath + CString("/") + filename;
		pcursor->SetTextureName((char*)(LPCTSTR)fullFileName);

}

void CTilePanel::OnCheckTileedit() 
{
	UpdateData();
	Update(); // listner 에게 알린다.
}

void CTilePanel::OnChangeEditCenterAlpha() 
{
	UpdateData();
	float alpha = (float)atof( m_strCenterAlpha );
	if( 0.f > alpha ) alpha = 0.f;
	if( 1.f < alpha ) alpha = 1.f;

	CTerrainCursor *pcursor = g_pTerrainView->GetTerrainCursor();
	if (!pcursor) return;
	pcursor->SetInnerAlpha(alpha);
}

void CTilePanel::OnChangeEditEdgeAlpha() 
{
	UpdateData();
	float alpha = (float)atof( m_strEdgeAlpha );
	if( 0.f > alpha ) alpha = 0.f;
	if( 1.f < alpha ) alpha = 1.f;
	CTerrainCursor *pcursor = g_pTerrainView->GetTerrainCursor();
	if (!pcursor) return;
	pcursor->SetOutterAlpha(alpha);
}


//-----------------------------------------------------------------------------//
// 맵에서 선택된 청크를 타일패널에 출력한다.
//-----------------------------------------------------------------------------//
void CTilePanel::SetCurrentChunk( CChunk *pchunk )
{
//	if( !pchunk ) return;
//	if( !m_pCurrentChunk )
//		m_pCurrentChunk = pchunk;
//	else if( (m_pCurrentChunk->x == pchunk->x) && (m_pCurrentChunk->y == pchunk->y) )
//		return;

	m_pCurrentChunk = pchunk;
	m_LayerList.SetCurrentChunk( pchunk );
	if (pchunk)
	{
		Vector2 chunkPos = pchunk->GetPos();
		m_strChunkPos.Format( "chunk pos: {%d, %d}", (int)chunkPos.x, (int)chunkPos.y );
	}
	else
	{
		m_strChunkPos.Format( "chunk pos: {x, y}" );
	}

	m_LayerList.DeleteAllItems();

	if( pchunk )
	{
		for( int i=0; i < pchunk->GetLayerCount(); ++i )
		{
			CChunk::SLayer *player = pchunk->GetLayer(i);
			m_LayerList.InsertItem( i, MakeLayerListString(i, player) );
		}
	}

	UpdateData( FALSE );
}

void CTilePanel::OnButtonDeletelayer() 
{
	if( !m_pCurrentChunk ) return;
	CTerrainEditor *pterrain = g_pTerrainView->GetTerrain();

	POSITION pos = m_LayerList.GetFirstSelectedItemPosition();
	if( !pos ) return;
	int ndelcnt = 0;
	while( pos )
	{
		int nItem = m_LayerList.GetNextSelectedItem( pos );
		m_pCurrentChunk->DeleteLayer( nItem - ndelcnt++ );
	}

	// Update Layer List
	SetCurrentChunk( m_pCurrentChunk );
}

void CTilePanel::OnButtonShowlayer() 
{
	if( !m_pCurrentChunk ) return;
	SetLayerFlag( m_pCurrentChunk, CChunk::VISIBLE, TRUE );
}

void CTilePanel::OnButtonEditlayer() 
{
	if( !m_pCurrentChunk ) return;
	SetLayerFlag( m_pCurrentChunk, CChunk::EDIT, TRUE );	
}

void CTilePanel::OnButtonHidelayer() 
{
	if( !m_pCurrentChunk ) return;
	SetLayerFlag( m_pCurrentChunk, CChunk::VISIBLE, FALSE );
}

void CTilePanel::OnButtonLocklayer() 
{
	if( !m_pCurrentChunk ) return;
	SetLayerFlag( m_pCurrentChunk, CChunk::EDIT, FALSE );	
}

//-----------------------------------------------------------------------------//
// LayerList에 출력된 레이어 정보 스트링을 만든다.
//-----------------------------------------------------------------------------//
CString CTilePanel::MakeLayerListString( int layeridx, CChunk::SLayer *player )
{
	CString num;
	num.Format( "[%d]", layeridx );
	CString flag( "<" );
	if( CChunk::VISIBLE & player->flag ) flag += 'V';
	if( CChunk::EDIT & player->flag ) flag += " E";
	flag += '>';

	char fileName[ MAX_PATH];
	GetFileNameNoPath(player->ptexturename, fileName);
	return num + flag + CString(" ") + fileName;
}


//-----------------------------------------------------------------------------//
// 레이어리스트에 선택된 레이어들을 일괄적으로 플래그값을 설정한다.
// pchunk: 레이어가 저장된 청크
// flag: 설정할 플래그 종류
// onoff: 플래그를 On시키려면 TRUE, Off시키려면 FALSE
//-----------------------------------------------------------------------------//
void CTilePanel::SetLayerFlag( CChunk *pchunk, DWORD flag, BOOL onoff )
{
	if( !pchunk ) return;

	POSITION pos = m_LayerList.GetFirstSelectedItemPosition();
	if( !pos ) return;

	while( pos )
	{
		int nItem = m_LayerList.GetNextSelectedItem( pos );
		CChunk::SLayer *player = pchunk->GetLayer(nItem);
		CChunk::SLayer *pcurrentLayer = m_pCurrentChunk->GetLayer(nItem);
		player->flag = 
			(onoff)? pcurrentLayer->flag | flag : (pcurrentLayer->flag & ~flag);
		m_LayerList.SetItemText( nItem, 0, MakeLayerListString(nItem, pcurrentLayer) );
	}
}

void CTilePanel::OnCheckErasebrush() 
{
	UpdateData();
	CTerrainCursor *pcursor = g_pTerrainView->GetTerrainCursor();
	if (!pcursor) return;
	pcursor->SetEraseMode(m_chkErase);
}

void CTilePanel::OnCheckMatchTexture() 
{
	UpdateData();
	CTerrainCursor *pcursor = g_pTerrainView->GetTerrainCursor();
	if (!pcursor) return;
	pcursor->SetMatchMode(m_chkMatchTexture);
}

BOOL CTilePanel::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	m_PannelViewPort.OffsetRect( 0, (0<zDelta)? 20 : -20 );
	m_PannelViewPort.left = 0;
	if( 0 < m_PannelViewPort.top ) m_PannelViewPort.top = 0;
	MoveWindow( m_PannelViewPort );

	return CPanelBase::OnMouseWheel(nFlags, zDelta, pt);
}

void CTilePanel::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// WM_MOUSEWHEEL 메세지를 받기위해서 다른 컨트롤에 포커스를 맞춘다.
	CWnd *pwnd = GetDlgItem( IDC_STATIC_CHUNKPOS );
	pwnd->SetFocus();
	m_bDrag = TRUE;
	GetCursorPos( &m_ptClickPos );
	SetCapture();
	CPanelBase::OnLButtonDown(nFlags, point);
}

void CTilePanel::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( m_bDrag ) ReleaseCapture();
	m_bDrag = FALSE;
	CPanelBase::OnLButtonUp(nFlags, point);
}

void CTilePanel::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( m_bDrag )
	{
		CPoint pos;
		GetCursorPos( &pos );
		CPoint offset = pos - m_ptClickPos;
		offset.x = 0;
		m_PannelViewPort.OffsetRect( offset );
		m_PannelViewPort.left = 0;
		if( 0 < m_PannelViewPort.top ) m_PannelViewPort.top = 0;
		MoveWindow( m_PannelViewPort );

		m_ptClickPos = pos;
	}	
	
	CPanelBase::OnMouseMove(nFlags, point);
}


//------------------------------------------------------------------------
// outter radius 슬라이더 이동
// [2011/2/14 jjuiddong]
//------------------------------------------------------------------------
void CTilePanel::OnNMCustomdrawSliderBrushsize(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

	UpdateData();
	CTerrainCursor *pcursor = g_pTerrainView->GetTerrainCursor();
	if (!pcursor) return;
	pcursor->SetOutterRadius((float)m_BrushSize.GetPos());
	pcursor->UpdateCursor();

	m_strBrushSize.Format( "%d", m_BrushSize.GetPos() );
	UpdateData( FALSE );	

	*pResult = 0;
}


//------------------------------------------------------------------------
// Inner radius 슬라이더 이동
// [2011/2/14 jjuiddong]
//------------------------------------------------------------------------
void CTilePanel::OnNMCustomdrawSliderBrushcenter(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

	UpdateData();
	CTerrainCursor *pcursor = g_pTerrainView->GetTerrainCursor();
	if (!pcursor) return;
	pcursor->SetInnerRadius((float)m_BrushCenter.GetPos());
	pcursor->UpdateCursor();

	m_strBrushCenter.Format( "%d", m_BrushCenter.GetPos() );
	UpdateData( FALSE );

	*pResult = 0;
}


//------------------------------------------------------------------------
// Brush Speed 슬라이더 이동
// [2011/2/14 jjuiddong]
//------------------------------------------------------------------------
void CTilePanel::OnNMCustomdrawSliderBrushspeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

	UpdateData();
	CTerrainCursor *pcursor = g_pTerrainView->GetTerrainCursor();
	if (!pcursor) return;
	pcursor->SetHeightIncrementOffset((float)m_BrushSpeed.GetPos());

	m_strBrushSpeed.Format( "%d", m_BrushSpeed.GetPos() );
	UpdateData( FALSE );

	*pResult = 0;
}


//------------------------------------------------------------------------
// 
// [2011/2/26 jjuiddong]
//------------------------------------------------------------------------
BOOL CTilePanel::IsEditTileMode()
{
	UpdateData();
	return m_chkEditTileMode;
}


//------------------------------------------------------------------------
// 
// [2011/3/1 jjuiddong]
//------------------------------------------------------------------------
void CTilePanel::OnShowWindow()
{
	
}


//------------------------------------------------------------------------
// 
// [2011/3/1 jjuiddong]
//------------------------------------------------------------------------
void CTilePanel::OnHideWindow()
{
	m_chkEditTileMode = FALSE;
	UpdateData(FALSE);
}
